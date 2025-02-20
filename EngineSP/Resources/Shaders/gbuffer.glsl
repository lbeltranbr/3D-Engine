
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
uniform int u_depth;

void main() 
{

	vec3 result = texture(u_map, uv).rgb;

	result = mix(result, vec3(result.r), u_depth); //depth info is stored in r channel 

	// fragment color
	out_fragment = vec4(result, 1.0);

}
//FRAGMENT//