#version 330 core

in vec3 currentPos;
in vec3 normal;
in vec3 color;
in vec2 texCoord;


out vec4 FragColor;

uniform sampler2D tex0;
uniform vec3 lightPos;
uniform vec3 cameraPosition;
uniform vec4 lightColor;

void main(){
   vec3 _normal = normalize(normal);
   vec3 lightDir = normalize(lightPos - currentPos);

   float ambient = 0.2;
   float diffuse = max(dot(lightDir, _normal), 0.0);

   float specularStrength = 0.5;
   vec3 viewDir = normalize(cameraPosition - currentPos);
   vec3 reflectDir = reflect(-lightDir, _normal);
   float specularAmount = pow(max(dot(viewDir, reflectDir), 0.0), 8);
   float specular = specularStrength * specularAmount;

   FragColor = texture(tex0, texCoord) * lightColor * (ambient + diffuse + specular);
}