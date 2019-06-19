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
    int shininess;
}; 

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;
in vec3 VertexColor;

uniform sampler2D uTexture;
uniform float uAlpha;
uniform bool colorRatio;
uniform bool useVertexColors;
uniform Material material;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    vec3 texColor = vec3(0);

    if (colorRatio)
        texColor = texture(uTexture, TexCoord).rgb;
    else if (useVertexColors)
        texColor = VertexColor;

    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // and the per-fragment color
    gAmbient.rgb = material.ambient + texColor * 0.2;
    gAmbient.a = uAlpha;

    gDiffuse.rgb = material.diffuse + texColor * 0.8;

    gSpecular.rgb = material.specular;
    gSpecular.a = material.shininess;
}