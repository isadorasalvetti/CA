#version 330 core
smooth in vec2 TexCoords;
smooth in vec3 ParticleColor;

out vec4 color;


void main(){
    color = vec4(ParticleColor, 1);
}
