// Fragment Shader

#version 150

in  vec2 ex_TexCoord; //texture coord arriving from the vertex

in  vec3 ex_Color;  //colour arriving from the vertex
out vec4 out_Color; //colour for the pixel

uniform sampler2D tex;

void main(void)
{
	out_Color = vec4(ex_Color,1.0) * texture(tex, ex_TexCoord);
}