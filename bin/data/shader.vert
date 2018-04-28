#version 150

// these are from the programmable pipeline system
uniform mat4 modelViewProjectionMatrix;
uniform float scale;
in vec4 position;
in vec2 texcoord;

// this is how we receive the texture
uniform sampler2DRect tex0;
uniform sampler2DRect tex1;

out vec2 texCoordVarying;

void main()
{
    // get the position of the vertex relative to the modelViewProjectionMatrix
    vec4 modifiedPosition = modelViewProjectionMatrix * position;

    // we need to scale up the values we get from the texture
    

    // here we get the red channel value from the texture
    // to use it as vertical displacement
    float displacementY = texture(tex0, texcoord).r;

    // use the displacement we created from the texture data
    // to modify the vertex position
    // modifiedPosition.y += (displacementY * scale) ;
    modifiedPosition.w += (displacementY * scale);
    // this is the resulting vertex position
    gl_Position = modifiedPosition;

    // pass the texture coordinates to the fragment shader
    texCoordVarying = texcoord;
}
