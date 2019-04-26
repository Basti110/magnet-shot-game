uniform float uAspectRatio;
uniform float uScale;

in vec4 aPosition;

void main()
{
    vec4 position = aPosition;
    position.x = position.x / uAspectRatio * uScale;
    position.y = position.y * uScale;
    gl_Position = position;
}
