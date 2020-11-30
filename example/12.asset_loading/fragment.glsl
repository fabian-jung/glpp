#version 330 core
in vec3 v_norm;
in vec2 v_uv;
out vec4 FragColor;

uniform sampler2D tile_tex;


void main()
{
// 	FragColor.xy = v_uv;
	FragColor = texture(tile_tex, vec2(v_uv.x, 1-v_uv.y));
}

