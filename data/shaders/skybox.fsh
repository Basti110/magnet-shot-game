out vec3 gPosition;
out vec3 gNormal;
out vec4 gAmbient;
out vec3 gDiffuse;
out vec4 gSpecular;

uniform vec3 uColor1;
uniform vec3 uColor2;
uniform float sunAngle;
uniform vec3 sunColor;


in vec3 vPosition;
out vec4 oFragColor;


void main() {

    float r = sqrt(pow(vPosition.x, 2) + pow(vPosition.y, 2) + pow(vPosition.z, 2));
    float theta =  -sunAngle - 4.712388; //acos(vPosition.y / r);
    float phi = 0; //atan(vPosition.x, vPosition.z);

    float x = r * sin(theta) * cos(phi);
    float z = r * sin(theta) * sin(phi);
    float y = r * cos(theta);
     
    float ratio = 1 - abs(vPosition.y) / 100;
    ratio = ratio * ratio;
    vec3 color;
    if(distance(vec3(x, y, z), vPosition) < 10)
        color = sunColor;
    else
        color =  ratio * uColor1 + (1 - ratio) * uColor2;
        
    gPosition = vPosition;

    gNormal = normalize(-vPosition);

    gAmbient.rgb = color;
    gAmbient.a = 0;

    gDiffuse.rgb = vec3(0);

    gSpecular.rgb = vec3(0);
    gSpecular.a = 1;

}
