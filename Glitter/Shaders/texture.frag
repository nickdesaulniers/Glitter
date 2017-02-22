#version 150

uniform sampler2D uSampler;

in vec2 vTexCoord;

out vec4 frag_color;

void main () {
  frag_color = texture2D(uSampler, vTexCoord);
  //frag_color = vec4(vTexCoord.xy, 0.0, 1.0);
}