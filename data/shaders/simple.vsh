uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform float uScale;

in vec3 aPos;


void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPos * uScale, 1.0);
}
