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
    /*float a = normalize(vPosition).y + 0.25;
    if (a < 0) a = -a / 0.75; sqrt(pow(theta * 2, 2) + pow(phi, 2)) < 0.5
    if (a > 0) a = a / 1.25;
    vec4 color = a * uColor1 + (1 - a) * uColor2;
    color =  vec4(pow(color.rgb, vec3(2.224)), 1.0);*/
    float r = sqrt(pow(vPosition.x, 2) + pow(vPosition.y, 2) + pow(vPosition.z, 2));
    float theta =  -sunAngle - 4.712388; //acos(vPosition.y / r);
    float phi = 0; //atan(vPosition.x, vPosition.z);
    /*float sunSetRatio = -0.8*pow(sunAngle - 1.5707963, 2) + 2; 
    //vec3 sun1 = vec3(210. / 255., 151. / 255., 76. / 255.);
    //vec3 sun2 = vec3(184. / 255., 1, 1);

    sunSetRatio = max(min(sunSetRatio, 1), 0);

    vec3 back1 = sunSetRatio * vec3(uColor1) + (1 - sunSetRatio) * sunsetBackground1;
    vec3 back2 = sunSetRatio * vec3(uColor2) + (1 - sunSetRatio) * sunsetBackground2;*/

    
    float x = r * sin(theta) * sin(phi);
    float y = r * cos(theta);
    float z = r * sin(theta) * cos(phi);
    
    float ratio = 1 - abs(vPosition.y) / 100;
    ratio = ratio * ratio;
    vec3 color;
    if(distance(vec3(x, y, z), vPosition) < 10)
        color = sunColor;
    else
        color =  ratio * uColor1 + (1 - ratio) * uColor2;
        

    gPosition = vPosition;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(-vPosition);
    // and the per-fragment color
    gAmbient.rgb = color;
    gAmbient.a = 0;

    gDiffuse.rgb = vec3(0);

    gSpecular.rgb = vec3(0);
    gSpecular.a = 1;
    //oFragColor = vec4(pow(color.rgb, vec3(2.224)), 1.0);
}
