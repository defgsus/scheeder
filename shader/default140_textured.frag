/*	Select your textures in the Textures menu.
	The sampler2D uniform will create a widget with which you
	can select a texture slot that will be bound to the sampler.
*/
#version 140
// input from vertex shader
in vec3 v_normal;
in vec4 v_color;
in vec3 v_pos;
in vec2 v_texcoord;
// shader uniforms (user)
uniform sampler2D u_tex;
uniform vec3 u_light_pos;
uniform vec3 u_light_color;
uniform float u_shinyness;
// output to rasterizer
out vec4 color;

void main()
{
	// 'ambient color' or base color
 	vec3 ambcol = v_color.xyz * texture(u_tex, v_texcoord).xyz;
	// normal to light source
	vec3 light_normal = normalize( u_light_pos - v_pos );
	// dot-product of light normal and vertex normal gives linear light influence
	float d = max(0.0, dot(v_normal, light_normal) );
	// shaping the linear light influence
	float lighting = pow(d, 1.0 + u_shinyness);
	// adding the light to the base color
	vec3 col = ambcol + lighting * u_light_color;
	// typical output of a fragment shader
 	color = vec4(clamp(col, 0.0, 1.0), 1.0);
}

