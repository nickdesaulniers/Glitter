#version 150

in vec2 position;
in vec3 a_color;
out vec4 v_color;
void main() {
  gl_Position = vec4(position, 0.0, 1.0);
  v_color = vec4(a_color, 1.0);
}