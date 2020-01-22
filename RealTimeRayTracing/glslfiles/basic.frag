// Fragment Shader 

#version 150

in  vec3 ex_Color;  //colour arriving from the vertex shader
out vec4 out_Color; //colour for the pixel to be sent out of the fragment shader

void main(void)
{
	out_Color = vec4(ex_Color,1.0);
}