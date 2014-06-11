// use the Box model for this shader
#version 140
// input from vertex shader
in vec2 v_pos;
// output to rasterizer
out vec4 color;  

uniform sampler2D u_tex1;
uniform sampler2D u_tex2;
uniform sampler2D u_tex3;

void main()
{
	// texture coordinate [0,1]
	vec2 uv = v_pos * 0.5 + 0.5;

	// read the three textures
	vec3 tex1 = texture(u_tex1, uv).xyz;
	vec3 tex2 = texture(u_tex2, uv).xyz;
	vec3 tex3 = texture(u_tex3, uv).xyz;

	// combine them
	vec3 col = tex1 + tex2 + tex3;

	// typical output
 	color = vec4(clamp(col, 0.0, 1.0), 1.0);
}

