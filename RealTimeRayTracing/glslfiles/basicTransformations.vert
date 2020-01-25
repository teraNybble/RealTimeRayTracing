#version 150

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

in  vec3 in_Position;  // Position coming in
in  vec3 in_Color;     // colour coming in
in  vec2 in_TexCoord;  // texture coordinate coming in

out vec3 ex_Color;     // colour leaving the vertex, this will be sent to the fragment shader
out vec2 ex_TexCoord;  // exiting texture coord

void main(void)
{
	gl_Position = /*ProjectionMatrix * ModelViewMatrix **/ vec4(in_Position, 1.0);

	ex_TexCoord = in_TexCoord;

	ex_Color = in_Color;
}