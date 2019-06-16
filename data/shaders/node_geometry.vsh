#version 330 core
in vec3 aPos;
in vec3 aNormal;
in vec3 aTagent;
in vec2 aTexCoord;
in vec3 aColor;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out vec3 VertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 pos = model * vec4(aPos, 1.0);
    FragPos = pos.xyz; 
    TexCoords = aTexCoord;
    
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    Normal = normalMatrix * aNormal;
    
    VertexColor = aColor;
    gl_Position = projection * view * pos;
}