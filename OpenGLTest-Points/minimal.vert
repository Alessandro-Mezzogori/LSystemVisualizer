// Vertex Shader – file "minimal.vert"

#version 130

uniform mat4 mvp;

in  vec3 in_Position;
in  vec3 in_Color;
out vec3 ex_Color;

void main(void)
{
	//ex_Color = vec3(0.0, 1.0, 0.0);
	ex_Color = in_Color;
	gl_Position = mvp * vec4(in_Position, 1.0);
}