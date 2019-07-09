uniform sampler2D uTexColor;
uniform sampler2D uTexDepth;
uniform float uFocus;
uniform float uAperture;

in vec2 vPosition;
out vec3 fColor;


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
    vec3 color = vec3(0);

    if (uAperture > 0) {
        ivec2 size = textureSize(uTexColor, 0);

        float depth = texture(uTexDepth, vPosition).x;
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

        // vec3 centerColor = texture(uTexColor, vPosition).rgb;

        //read out the texels
        for (int i=-kSize; i <= kSize; ++i) {
            for (int j=-kSize; j <= kSize; ++j) {
                vec2 offset = vec2(i, j) / (kSize * size) * coc;
                float weight = kernel[kSize+j] * kernel[kSize+i];
                color += weight * texture(uTexColor, vPosition + offset).rgb;
                // float sampleDepth = texture(uTexDepth, vPosition + offset).x;
                // if (abs(sampleDepth - depth) < 0.01) {
                //     color += weight * texture(uTexColor, vPosition + offset).rgb;
                // }
                // else {
                //     color += weight * centerColor;
                // }
            }
        }
        color /= (Z * Z);
    }
    else {
        color = texture(uTexColor, vPosition).rgb;
    }

    fColor = color;
}
