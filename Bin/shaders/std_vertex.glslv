#version 330 core

uniform mat4 viewMat;
uniform mat4 modelViewProjectionMatrix;

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

out vec3 pos;
out vec3 viewSpacePos;
out vec3 normal;
out vec2 texCoord;

void main()
{
    pos = vertexPosition.xyz;
    viewSpacePos = (viewMat * vec4(vertexPosition, 1.0)).xyz;
    normal = vertexNormal;
    texCoord = vertexTexCoord;
    gl_Position = modelViewProjectionMatrix * vec4(vertexPosition, 1.0);
}
