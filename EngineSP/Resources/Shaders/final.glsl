
#version 410 core
layout (location = 0) in vec4 a_quad;

out vec2 uv;

void main() 
{
  gl_Position = vec4(a_quad.x, a_quad.y, 0.0, 1.0); 
  uv = vec2(a_quad.z, a_quad.w);
}
//VERTEX//

#version 410 core
out vec4 out_fragment;

#define GAMMA 2.5
#define EXPOSURE 10.0
#define EPS 0.00000000001

in vec2 uv;

uniform sampler2D u_map;
uniform sampler2D u_bloom;

void main() 
{
	vec3 result = texture(u_map, uv).rgb + texture(u_bloom, uv).rgb;

	// sample color from map
	result = pow(result, vec3(GAMMA));

	// process exposure
	result = vec3(1.0) - exp(-result * EXPOSURE); 

	// gamma correction
	result = pow(result, vec3(1.0 / max(GAMMA, EPS)));

	// fragment color
	out_fragment = vec4(result, 1.0); 

	//to view shadowmap 
	// float result = texture(u_map, uv).r;
	//out_fragment = vec4(vec3(result), 1.0);
}
//FRAGMENT//