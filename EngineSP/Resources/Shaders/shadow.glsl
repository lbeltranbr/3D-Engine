
#version 410 core
layout (location = 0) in vec3 a_position;
//-----------------Uniforms-----------------//
uniform mat4 u_lightSpace;
uniform mat4 u_model;
//-----------------Main-----------------//
void main() 
{
  gl_Position = u_lightSpace * u_model * vec4(a_position, 1.0f);
}
//VERTEX//

#version 410 core

void main() 
{
  
}
//FRAGMENT//