uniform mat4 uTransform;

in vec3 aPos;
out vec3 vPosition;


void main() {
    vPosition = 100 * aPos;
    gl_Position = uTransform * vec4(vPosition, 1.0);
}
