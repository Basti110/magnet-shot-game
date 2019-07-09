uniform sampler2D uTexColor;
uniform sampler2D uTexDepth;
uniform float uFocus;
uniform float uAperture;
uniform bool uShowPostProcess;
uniform bool uFXAA;
uniform vec2 uBufferSize;

in vec2 vPosition;
out vec3 fColor;

const float FXAA_REDUCE_MIN = (1.0/ 128.0);
const float FXAA_REDUCE_MUL = (1.0 / 8.0);
const float FXAA_SPAN_MAX = 8.0;

vec3 fxaa() {
    vec4 color;
    vec3 rgbNW = texture(uTexColor, vPosition.xy + vec2(-1.0, -1.0) / uBufferSize).xyz;
    vec3 rgbNE = texture(uTexColor, vPosition.xy + vec2(1.0, -1.0) / uBufferSize).xyz;
    vec3 rgbSW = texture(uTexColor, vPosition.xy + vec2(-1.0, 1.0) / uBufferSize).xyz;
    vec3 rgbSE = texture(uTexColor, vPosition.xy + vec2(1.0, 1.0) / uBufferSize).xyz;
    vec4 texColor = texture(uTexColor, vPosition.xy);
    vec3 rgbM  = texColor.xyz;
    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    mediump vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) *
                        (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);

    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
            max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
            dir * rcpDirMin) / uBufferSize);

    vec3 rgbA = 0.5 * (
        texture(uTexColor, vPosition.xy + dir * (1.0 / 3.0 - 0.5)).xyz +
        texture(uTexColor, vPosition.xy + dir * (2.0 / 3.0 - 0.5)).xyz);
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture(uTexColor, vPosition.xy + dir * -0.5).xyz +
        texture(uTexColor, vPosition.xy + dir * 0.5).xyz);

    float lumaB = dot(rgbB, luma);
    if ((lumaB < lumaMin) || (lumaB > lumaMax))
        color = vec4(rgbA, texColor.a);
    else
        color = vec4(rgbB, texColor.a);
    return color.xyz;
}

float depth2dist(float depth)
{
    float near = 0.1; // TODO uniform
    float far = 500.0; // TODO uniform

    float normalized = 2.0 * depth - 1.0;
    return 2.0 * near * far / (far + near - normalized * (far - near));
}

float normpdf(float x, float sigma)
{
    return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}

void main()
{
    // vec3 color = uFXAA ? fxaa() : texture(uTexColor, vPosition.xy).rgb;
    // float depth = texture(uTexDepth, vPosition).x;

    vec3 color = vec3(0);
    float depth = texture(uTexDepth, vPosition).x;

    if (uAperture > 0) {
        ivec2 size = textureSize(uTexColor, 0);
        float objectDistance = depth2dist(depth);

        float imageDistance = 1;
        float focalLength = 1 / ((1 / uFocus) + (1 / imageDistance));
        focalLength *= 4;

        float numerator = focalLength * (uFocus - objectDistance);
        float denominator = objectDistance * (uFocus - focalLength);
        float coc = abs(uAperture * numerator / denominator);
        coc = clamp(coc, 0, 20);

        // declare stuff
        const int mSize = 11;
        const int kSize = (mSize-1)/2;
        float kernel[mSize];

        // create the 1-D kernel
        float sigma = 7.0;
        for (int j = 0; j <= kSize; ++j) {
                kernel[kSize+j] = kernel[kSize-j] = normpdf(float(j), sigma);
        }

        // get the normalization factor
        float Z = 0.0;
        for (int j = 0; j < mSize; ++j) {
            Z += kernel[j];
        }

        //read out the texels
        for (int i=-kSize; i <= kSize; ++i) {
            for (int j=-kSize; j <= kSize; ++j) {
                vec2 offset = vec2(i, j) / kSize / size * coc;
                color += kernel[kSize+j]*kernel[kSize+i]*texture(uTexColor, vPosition + offset).rgb;
            }
        }
        color /= Z * Z;
    }
    else {
        color = texture(uTexColor, vPosition).rgb;
    }
    // conversion to sRGB
    color = pow(color, vec3(1 / 2.224));

    if (uShowPostProcess && depth < 1)
        color = 1 - color; // invert color for foreground if true

    fColor = color;
}
