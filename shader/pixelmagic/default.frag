// use the Box model for this shader
#version 140
// input from vertex shader
in vec2 v_pos;
// output to rasterizer
out vec4 color;

void main()
{
	// simply display x and y as red and green
	vec3 col = vec3(v_pos, 0.0);
	// typical output
 	color = vec4(clamp(col, 0.0, 1.0), 1.0);
}

