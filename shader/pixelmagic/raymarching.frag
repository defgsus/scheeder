// use the Box model for this shader
#version 140
// input from vertex shader
in vec2 v_pos;
// output to rasterizer
out vec4 color;
// shader uniforms (default)
uniform mat4 u_view;
uniform float u_time;
// shader uniforms (user)
uniform vec3 u_light_pos;
uniform vec3 u_light_color;

const vec3 sky_color = vec3(0.3,0.6,0.9);
const int num_ray_steps = 50;
const int num_shadow_steps = 20;

// distance to scene (scalar field)
float scene_dist(in vec3 pos)
{
	// a displacement function for the floor
	vec3 disp = vec3(0,sin(pos.x*10.)*sin(pos.z*10.)*0.1,0);
	// a floor plane, with normal 0,1,0
	float d = dot(pos + disp, vec3(0,1,0));
	// a sphere at -1,1,-5
	d = min(d, distance(pos, vec3(-1,1,-5))-1 );

	return d;
} 

// returns the normal of the scalar field at any position
vec3 scene_normal(in vec3 pos)
{
	const vec2 eps = vec2(0.01, 0.);
	return normalize(vec3(
		scene_dist(pos + eps.xyy) - scene_dist(pos - eps.xyy),
		scene_dist(pos + eps.yxy) - scene_dist(pos - eps.yxy),
		scene_dist(pos + eps.yyx) - scene_dist(pos - eps.yyx)
		));
}

// cast a shadow ray from pos along dir
// returns 0.0 for complete shader - 1.0 for no shadow
float shadow_ray(in vec3 pos, in vec3 dir)
{
	float t = 0.08;
	float shadow = 1.0;
	for (int i=0; i<num_shadow_steps; ++i)
	{
		vec3 p = pos + t * dir;
		float d = scene_dist(p);
		if (d<0.001) return 0.0;

		shadow = min(shadow, d * 1.5 / t);

		t += d * (1.0 + 0.1 * t);
	}

	return clamp(shadow, 0.0, 1.0);
}

vec3 scene_color(in vec3 pos, in vec3 norm)
{
	// normal to lightsource
	vec3 light_norm = normalize(u_light_pos - pos);
	// shadow factor 
	float shadow = shadow_ray(pos, light_norm);
	// base color with shadow
	vec3 col = vec3(0.5,0.5,0.5) * (0.5 + 0.5 * shadow);
	// light factor
	float d = pow(max(0., dot(norm, light_norm)), 1.5);
	// basecolor + light
	col += d * u_light_color * shadow;
	return col;
}

vec3 ray_color(in vec3 start, in vec3 dir)
{
	float t = 0.;
	for (int i=0; i<num_ray_steps; ++i)
	{
		vec3 pos = start + dir * t;
		float d = scene_dist(pos);
		if (d<=0.001)
		{
			vec3 norm = scene_normal(pos);
			vec3 col = scene_color(pos, norm);
			return mix(col, sky_color, min(1.0, t/10.));
		}

		t += d;
	}
	return sky_color;
}

void main()
{
	// position of camera
	vec3 pos = vec3(0., 1., u_view[3].z);
	
	// initial direction of the rays for each pixel
	// (this is a cheap approx. of a camera frustum)
	vec3 idir = normalize(vec3(v_pos, 2.));
	// final ray direction put through view-matrix
	vec3 dir = vec3(vec4(idir,0) * u_view);
	
	// raytrace
	vec3 col = ray_color(pos, dir);

	// output
 	color = vec4(clamp(col, 0.0, 1.0), 1.0);
}

