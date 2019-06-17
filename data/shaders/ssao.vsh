#version 330 core
in vec2 aPosition;

out vec2 TexCoords;

void main()
{
    TexCoords = aPosition;
    gl_Position = vec4(aPosition * 2 - 1, 0, 1);
}
