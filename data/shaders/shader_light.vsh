#version 330 core
in vec3 aPos; // the position variable has attribute position 0

uniform vec3 color;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
  
out vec4 vertexColor; // specify a color output to the fragment shader

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexColor = vec4(color, 1.0); // set the output variable to a dark-red color
}