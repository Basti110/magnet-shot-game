out vec3 gPosition;
out vec3 gNormal;
out vec4 gAmbient;
out vec3 gDiffuse;
out vec4 gSpecular;

uniform vec4 uColor1;
uniform vec4 uColor2;

in vec3 vPosition;
out vec4 oFragColor;


void main() {
    float a = normalize(vPosition).y + 0.25;
    if (a < 0) a = -a / 0.75;
    if (a > 0) a = a / 1.25;
    vec4 color = a * uColor1 + (1 - a) * uColor2;
    color =  vec4(pow(color.rgb, vec3(2.224)), 1.0);

    gPosition = vPosition;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(-vPosition);
    // and the per-fragment color
    gAmbient.rgb = vec3(color) * 0.2;
    gAmbient.a = 0;

    gDiffuse.rgb = vec3(color) * 0.8;

    gSpecular.rgb = vec3(0);
    gSpecular.a = 1;
    //oFragColor = vec4(pow(color.rgb, vec3(2.224)), 1.0);
}
