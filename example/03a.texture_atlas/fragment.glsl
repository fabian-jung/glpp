#version 330 core
in vec2 v_tex;
out vec4 FragColor;

uniform float tex_id;
uniform int max_frames = 4;

<texture_atlas>;

void main()
{
	vec2 tex = vec2(v_tex.x, 1-v_tex.y);
	int first_index = int(mod(tex_id, max_frames));
	int second_index = first_index+1 >= max_frames ? 0 : first_index+1;
	float frac = fract(tex_id);
	FragColor = <fetch1>*(1-frac)+<fetch2>*frac;
}