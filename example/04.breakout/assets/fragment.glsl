#version 330 core
in vec2 v_tex;
out vec4 FragColor;

uniform sampler2D texture_unit;

void main()
{
// 	FragColor = vec4(tex, 0, 1);
	FragColor = texture(texture_unit, v_tex);//*vec4(0, 0, 1, 0);
	if(FragColor == vec4(1.0, 0.0, 1.0, 1.0)) discard;
}
