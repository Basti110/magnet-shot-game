uniform sampler2D uTexColor;
uniform sampler2D uTexDepth;
uniform bool uShowPostProcess;

in vec2 vPosition;
out vec3 fColor;

void main()
{
    vec3 color = texture(uTexColor, vPosition).rgb;
    float depth = texture(uTexDepth, vPosition).x;

    // conversion to sRGB
    color = pow(color, vec3(1 / 2.224));

    if (uShowPostProcess && depth < 1)
        color = 1 - color; // invert color for foreground if true

    fColor = color;
}
