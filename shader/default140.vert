#version 140
// vertex attributes
in vec3 a_normal;
in vec4 a_color;
in vec4 a_position;
// shader uniforms (application specific)
uniform mat4 u_projection;
uniform mat4 u_view;
// output of vertex shader
out vec3 v_normal;
out vec4 v_color;
out vec3 v_pos;

void main()
{
	// pass attributes to fragment shader
 	v_normal = a_normal; 
 	v_color = a_color;
 	v_pos = a_position.xyz;
	// set final vertex position
 	gl_Position = u_projection * u_view * a_position;
}
