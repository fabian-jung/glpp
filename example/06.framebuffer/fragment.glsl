#version 330 core
in vec3 v_norm;
in vec4 v_pos;
out vec3 FragColor;

void main()
{
	FragColor = (1-v_pos.z/30)*0.5*(v_norm+vec3(1,1,1));
}

