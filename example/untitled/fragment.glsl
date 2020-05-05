#version 330 core
uniform vec3 color = vec3(.7,.3,.4);
out vec4 FragColor;

void main()
{
	FragColor = vec4(color, 1.0);
}
