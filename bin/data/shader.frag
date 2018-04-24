#version 150
uniform sampler2DRect tex0;
uniform sampler2DRect tex1;

in vec2 texCoordVarying;
out vec4 outputColor;
void main()
{
    outputColor = texture(tex1, texCoordVarying);
}
