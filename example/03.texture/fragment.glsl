#version 330 core
in vec2 v_tex;
out vec4 FragColor;

uniform sampler2D texture_one;

void main()
{
	vec2 tex = vec2(v_tex.x, 1-v_tex.y);
	FragColor = texture(texture_one, tex);
}
