uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec3 aPosition;
in vec3 aNormal;
in vec3 aTangent;
in vec2 aTexCoord;

out vec3 vWorldPos;
out vec3 vNormal;
out vec3 vTangent;
out vec2 vTexCoord;

void main()
{
    // assume uModel has no non-uniform scaling
    vNormal = mat3(model) * aNormal;
    vTangent = mat3(model) * aTangent;

    vTexCoord = aTexCoord;

    vWorldPos = vec3(model * vec4(aPosition, 1));
    gl_Position = projection * view * vec4(vWorldPos, 1);
}