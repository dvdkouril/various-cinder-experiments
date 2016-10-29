#version 410
in vec4 ciPosition;
in vec2 ciTexCoord0;
out vec2 TexCoords;

uniform mat4 ciModelViewProjection;

void main(void)
{
  TexCoords = ciTexCoord0;
  gl_Position = ciModelViewProjection * ciPosition;
}
