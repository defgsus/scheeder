#version 140
// vertex attributes
in vec3 a_normal;
in vec4 a_color;
in vec4 a_position;
// shader uniforms (application specific)
uniform mat4 u_projection;
uniform mat4 u_view;
uniform float u_time;
// output of vertex shader
out vec3 v_normal;
out vec4 v_color;
out vec3 v_pos;

void main()
{
	// create a modulated version of the vertex position
	vec4 modp = a_position + vec4(
			sin(u_time+a_position.y), 
			cos(u_time+a_position.z), 
			sin(u_time+a_position.x), 0);
	// pass attributes to fragment shader
 	v_normal = a_normal; 
 	v_color = a_color;
 	v_pos = modp.xyz;
	// set final vertex position
 	gl_Position = u_projection * u_view * modp;
}
