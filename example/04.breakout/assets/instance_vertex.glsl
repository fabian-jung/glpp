#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex;

uniform ivec2 bounds;
uniform sampler2D level;

out vec2 v_tex;

void main()
{
	vec2 instance_pos =
		vec2(
			gl_InstanceID-(gl_InstanceID / bounds.x)*bounds.x,
			gl_InstanceID/bounds.x
		);
	vec2 tex_pos = (instance_pos+vec2(.5, .5))/bounds;
	vec2 screen_pos = vec2(-1,-1)+instance_pos*2/vec2(bounds);
	if(texture(level, tex_pos).r < 0.5) {
		gl_Position.xy = vec2(0,0);
		return;
	}
	gl_Position.xy = pos+screen_pos;
	v_tex = tex;

}
