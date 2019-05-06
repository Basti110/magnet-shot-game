uniform mat4 uTransform;

in vec3 aPosition;
out vec3 vPosition;


void main() {
    vPosition = 250 * aPosition;
    gl_Position = uTransform * vec4(vPosition, 1.0);
}
