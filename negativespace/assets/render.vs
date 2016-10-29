#version 410
in vec4 ciPosition;

uniform mat4 ciModelViewProjection;

void main(void)
{
  gl_Position = ciModelViewProjection * ciPosition;
}
