// use the Box model for this shader
#version 140
// vertex attributes
in vec4 a_position;
// output to fragment shader
out vec2 v_pos;
// shader uniforms (default)
uniform float u_aspect;
uniform float u_time;
// shader uniforms (user)
uniform float u_ball_size;

void main()
{
	// fit into the screen space
 	v_pos = a_position.xy * vec2(u_aspect,1);
	// jump around
	v_pos += vec2(sin(u_time), 1.0-u_ball_size-abs(sin(u_time*3)));
	// move the actual position a bit away from the camer
	// so it will not be clipped by the frustum
 	gl_Position = vec4(0,0,-3,0) + a_position;
}
