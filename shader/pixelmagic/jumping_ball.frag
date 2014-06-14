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
	float d =
			// circle
			length(v_pos); 
			// box
			//max(abs(v_pos.x),abs(v_pos.y));
			// cross
			//min(abs(v_pos.x), abs(v_pos.y)); 
			// psychedelic spiral
			//length(v_pos) + 0.8*pow(sin(atan(v_pos.y, v_pos.x)*10+sin(u_time*0.21)*100-(sin(u_time*0.618)*20)*length(v_pos)), 4+sin(u_time)*3.9); 
						
	// an object of specific size
	float c = float(d >= u_ball_size);
	//float c = smoothstep(u_ball_size-0.1, u_ball_size, d); // smooth version
	vec3 col = vec3(1, c, c);
	// typical output
 	color = vec4(clamp(col, 0.0, 1.0), 1.0);
}

