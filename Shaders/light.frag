#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D tex0;
uniform vec4 lightColor;

void main()
{
	FragColor = lightColor * texture(tex0, texCoord) * 3.0;
}