#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 uv;

out vec3 v_norm;
out vec2 v_uv;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp*vec4(pos, 1.0);
	v_norm = norm;
	v_uv = uv;
}
