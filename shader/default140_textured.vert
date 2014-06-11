#version 140
// vertex attributes
in vec3 a_normal;
in vec4 a_color;
in vec4 a_position;
in vec2 a_texcoord;
// shader uniforms (application specific)
uniform mat4 u_projection;
uniform mat4 u_view;
// output of vertex shader
out vec3 v_normal;
out vec4 v_color;
out vec3 v_pos;
out vec2 v_texcoord;

void main()
{
	// pass attributes to fragment shader
 	v_normal = a_normal; 
 	v_color = a_color;
 	v_pos = a_position.xyz;
	v_texcoord = a_texcoord;
	// set final vertex position
 	gl_Position = u_projection * u_view * a_position;
}
