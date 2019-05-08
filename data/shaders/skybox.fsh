uniform vec4 uColor1;
uniform vec4 uColor2;

in vec3 vPosition;
out vec4 oFragColor;


void main() {
    float a = normalize(vPosition).y + 0.25;
    if (a < 0) a = -a / 0.75;
    if (a > 0) a = a / 1.25;
    vec4 color = a * uColor1 + (1 - a) * uColor2;
    oFragColor = vec4(pow(color.rgb, vec3(2.224)), 1.0);
}
