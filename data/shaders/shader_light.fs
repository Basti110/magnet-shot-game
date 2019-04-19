#version 330 core
out vec4 FragColor;

in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  

void main()
{
    //FragColor = texture(ourTexture, TexCoord) * (1 - colorRatio) + vertexColor * colorRatio;
	//color = vec3(texture(cubeMap, TexCoord));
    FragColor = vertexColor;
}