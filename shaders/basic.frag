#version 330 core

out vec4 frag_color;
uniform vec3 color;

smooth in float normalColor;

void main (void) {
   // write Total Color:
   frag_color = vec4(color*normalColor, 1);
}
