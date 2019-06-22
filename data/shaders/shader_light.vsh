#version 330 core
in vec3 aPos; // the position variable has attribute position 0

uniform vec3 color;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec4 vertexColor; // specify a color output to the fragment shader

void main()
{
    vec4 pos = view * model * vec4(aPos, 1.0);
    FragPos = pos.xyz; 
    Normal = -aPos.xyz;
    gl_Position = projection * pos;
    vertexColor = vec4(color, 1.0);
}