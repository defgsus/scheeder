// use the Box model for this shader
#version 140
// input from vertex shader
in vec2 v_pos;
// output to rasterizer
out vec4 color;
// shader uniforms (default)
uniform float u_time;
// shader uniforms (user)
uniform float u_ball_size;

void main()
{
	// distance to center
	float d = length(v_pos);
	// a sphere of specific size 
	vec3 col = vec3(d >= u_ball_size);
	// typical output
 	color = vec4(clamp(col, 0.0, 1.0), 1.0);
}

