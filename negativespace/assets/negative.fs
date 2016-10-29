#version 410
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D tex;

void main(void) {
  vec4 col = texture(tex, TexCoords);
  //FragColor = vec4(1-col.r,1-col.g,1-col.b,1);
  vec2 center = vec2(0.5, 0.5);
  vec2 v = TexCoords - center;
  if (length(v) < 0.2)
  {
    FragColor = vec4(1-col.r,1-col.g,1-col.b,1);
  }
  else
  {
    FragColor = col;
  }
}
