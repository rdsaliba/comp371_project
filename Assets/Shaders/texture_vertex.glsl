#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNorm;
layout (location = 3) in vec2 aUV;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix = mat4(1.0);  // default value for view matrix (identity)
uniform mat4 projectionMatrix = mat4(1.0);
uniform mat4 lightSpaceMatrix;

out vec3 vertexColor;
out vec3 norm;
out vec3 fragPos;
out vec4 fragPosLightSpace;
out vec2 vertexUV;

void main()
{
   vertexColor = aColor;
   fragPos = vec3(worldMatrix * vec4(aPos, 1.0));
   norm = mat3(transpose(inverse(worldMatrix))) * aNorm;
   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;
   fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   vertexUV = aUV;
};