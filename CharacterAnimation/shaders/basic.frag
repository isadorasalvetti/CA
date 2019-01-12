#version 330 core

out vec4 frag_color;
uniform vec3 color;
uniform bool useLight;

smooth in float normalColor;

void main (void) {
   // write Total Color:
    if (useLight) frag_color = vec4(color*0.6+color*normalColor*0.4, 1);
    else frag_color = vec4(color, 1);
}
