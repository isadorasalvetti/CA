#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

smooth out vec2 TexCoords;
smooth out vec3 ParticleColor;

uniform mat4 projection;
uniform vec3 color;

void main(){
    float scale = 10.0f;
    TexCoords = vertex.zw;
    ParticleColor = color;
    gl_Position = projection * vec4((vertex.xy * scale), 0.0, 1.0);
}
