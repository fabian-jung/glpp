#version 330 core
in vec2 v_tex;
out vec4 FragColor;

uniform sampler2D texture_one;

void main()
{
	FragColor = texture(texture_one, v_tex);
}
