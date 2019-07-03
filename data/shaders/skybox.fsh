out vec3 gPosition;
out vec3 gNormal;
out vec4 gAmbient;
out vec3 gDiffuse;
out vec4 gSpecular;

uniform vec3 uColor1;
uniform vec3 uColor2;
uniform float sunAngle;
uniform float sunVisibility;
uniform float moonAngle;
uniform float moonVisibility;
uniform vec3 sunColor;


in vec3 vPosition;
out vec4 oFragColor;


void main() {

    float r = sqrt(pow(vPosition.x, 2) + pow(vPosition.y, 2) + pow(vPosition.z, 2));
    float thetaSun =  -sunAngle - 4.712388; //acos(vPosition.y / r);
    float phi = 0.785398163397448; //atan(vPosition.x, vPosition.z);
    float thetaMoon =  -moonAngle - 4.712388;

    
    float xSun = r * sin(thetaSun) * sin(phi);
    float ySun = r * cos(thetaSun);
    float zSun = r * sin(thetaSun) * cos(phi);

    float xMoon = r * sin(thetaMoon) * sin(phi);
    float yMoon = r * cos(thetaMoon);
    float zMoon = r * sin(thetaMoon) * cos(phi);
     
    float ratio = 1 - abs(vPosition.y) / 100;
    ratio = ratio * ratio;
    vec3 color = ratio * uColor1 + (1 - ratio) * uColor2;
    if(distance(vec3(xSun, ySun, zSun), vPosition) < 10)
        color = sunVisibility * sunColor + (1 - sunVisibility) * color;
    else if(distance(vec3(xMoon, yMoon, zMoon), vPosition) < 10)
        color = moonVisibility * vec3(1) + (1 - moonVisibility) * color;
        
    gPosition = vPosition;

    gNormal = normalize(-vPosition);

    gAmbient.rgb = color;
    gAmbient.a = 0;

    gDiffuse.rgb = vec3(0);

    gSpecular.rgb = vec3(0);
    gSpecular.a = 1;

}
