#version 330 core
out vec4 FragColor;
in vec2 c;

uniform sampler2D texture_one;

void main()
{
	FragColor = texture(texture_one, c);
}
