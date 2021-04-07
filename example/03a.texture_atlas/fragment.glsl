#version 330 core
in vec2 v_tex;
out vec4 FragColor;

<texture_atlas>

void main()
{
	vec2 tex = vec2(v_tex.x, 1-v_tex.y);
	FragColor = <fetch>;
}