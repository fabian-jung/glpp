#version 330 core
in vec3 v_norm;
out vec3 FragColor;

void main()
{
	FragColor = 0.5*(v_norm+vec3(1,1,1));
}

