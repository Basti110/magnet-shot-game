#version 330 core
out vec3 gPosition;
out vec3 gNormal;
out vec4 gAmbient;
out vec3 gDiffuse;
out vec4 gSpecular;

in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  
in vec3 FragPos;
in vec3 Normal;

void main()
{
    //FragColor = texture(ourTexture, TexCoord) * (1 - colorRatio) + vertexColor * colorRatio;
	//color = vec3(texture(cubeMap, TexCoord));
    //FragColor = vertexColor;
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // and the per-fragment color
    gAmbient.rgb = vec3(vertexColor);
    gAmbient.a = 0;

    gDiffuse.rgb = vec3(0);

    gSpecular.rgb = vec3(0);
    gSpecular.a = 1;
}