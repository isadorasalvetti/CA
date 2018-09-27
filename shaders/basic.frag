#version 330 core

out vec4 frag_color;

smooth in vec3 N;

void main (void) {
   // write Total Color:
   vec3 color = vec3(0.9, 0.9, 0.9) * N;
   frag_color = vec4(color, 1.0);
   frag_color = vec4(0, 0, 0, 1.0);
}
