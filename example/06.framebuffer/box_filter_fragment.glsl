#version 330 core
in vec2 v_tex;
out vec4 FragColor;

uniform sampler2D slot;
uniform vec2 resolution;
uniform bool direction;

void main()
{
	vec2 offset;
	if(direction) {
		offset = vec2(1,0)/resolution;
	} else {
		offset = vec2(0,1)/resolution;
	}
	const int width = 25;
	const int middle = int(ceil(float(width)/2.0));
	float kernel[width];
	float sum = 0;
	for(int i = 0; i < width; ++i) {
		kernel[i] = pow(min(i+1,width-i),1.15);
		sum += kernel[i];
	}

    vec4 color = vec4(0.0);
	for(int i = -width/2; i <= width/2; ++i) {
		color += (1.0/sum)*kernel[i+width/2]*texture(slot, v_tex+offset*i);
	}

	FragColor = color;
}


