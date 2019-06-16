#version 330 core
out vec3 gPosition;
out vec3 gNormal;
out vec4 gAmbient;
out vec3 gDiffuse;
out vec4 gSpecular;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 VertexColor;

uniform float uAlpha;
uniform float colorRatio;
uniform Material material;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // and the per-fragment color
    gAmbient.rgb = material.ambient;
    gAmbient.a = uAlpha;

    gDiffuse.rgb = material.diffuse;

    gSpecular.rgb = material.specular;
    gSpecular.a = material.shininess;
}