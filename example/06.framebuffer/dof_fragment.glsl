#version 330 core
in vec2 v_tex;
out vec4 FragColor;

uniform sampler2D original;
uniform sampler2D blurred;
uniform sampler2D depth;

uniform float depth_in_focus;
uniform float near_plane = 0.1;
uniform float far_plane = 10.0;

float linear_depth(float depthSample)
{
    float z = depthSample * 2.0 - 1.0; // Back to NDC
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
	float depth = linear_depth(texture(depth, v_tex).x)/far_plane;
	float offset = min(5*abs(depth-depth_in_focus), 1.0);
	gl_FragDepth = depth;
	FragColor = mix(texture(original, v_tex), texture(blurred, v_tex), offset);
}



