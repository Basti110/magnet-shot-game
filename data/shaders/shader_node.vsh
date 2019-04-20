#version 330 core
in vec3 aPos; // the position variable has attribute position 0
in vec2 aTexCoord;
in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform vec3 color;
  

//out vec4 vertexColor; // specify a color output to the fragment shader
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    //vertexColor = vec4(color, 1.0); // set the output variable to a dark-red color
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoord = aTexCoord;
}