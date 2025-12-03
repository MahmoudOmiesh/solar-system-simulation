#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out vec3 currentPos;
out vec3 normal;
out vec3 color;
out vec2 texCoord;


uniform mat4 model;
uniform mat4 cameraMatrix;

void main()
{
   currentPos = vec3(model * vec4(aPos, 1.0));

   mat3 normalMatrix = mat3(transpose(inverse(model)));
   normal = normalize(normalMatrix * aNormal);

   color = aColor;
   texCoord = aTex;

   gl_Position = cameraMatrix * vec4(currentPos, 1.0);
}