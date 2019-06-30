#version 120

varying vec2 texcoord;
varying vec4 color;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
    texcoord = gl_TexCoord[0].st;
    color = gl_Color;
    gl_Position = ftransform();
}
