// Fragment Shader � file "minimal.frag"

#version 130

precision highp float; // needed only for version 1.30

in  vec3 ex_Color;
out vec4 out_Color;

void main(void)
{
	out_Color = vec4(0.0, 1.0, 0.0, 1.0);
	//out_Color = vec4(ex_Color, 1.0);
	//Try replacing the above with the following:
	//vec3 tmp_Color;
	//tmp_Color = ex_Color.rrr;	
	//out_Color = vec4(tmp_Color,1.0);
}