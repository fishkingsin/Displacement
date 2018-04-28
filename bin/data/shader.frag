#version 150
uniform sampler2DRect tex0;
uniform sampler2DRect tex1;

in vec2 texCoordVarying;
out vec4 outputColor;
void main()
{
    float intensity = texture(tex0, texCoordVarying).r;
    vec4 color;
	if (intensity > 0.75)

		color = vec4(2.0,2.0,2.0,1.0);
	else if (intensity > 0.7 && intensity < 0.75)
		color = vec4(1.7,1.7,1.7,1.0);
    else if (intensity > 0.51 && intensity < 0.6)
		color = vec4(1.5,1.5,1.5,1.0);
    else if (intensity < 0.5 )
		color = vec4(0.9,0.9,0.9,1.0);
	else
		color = vec4(1.0,1.0,1.0,1.0);
    float brightness = cos(intensity);
    outputColor = texture(tex1, texCoordVarying) * color * vec4(brightness,brightness,brightness,1.0);
}
