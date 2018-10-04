#version 330 core
layout (location = 0) in vec3 vertex;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat3 normalMatrix;

smooth out vec2 TexCoords;

void main(){
    TexCoords = vertex.xz;
    vec4 view_vertex = modelview * vec4(vertex, 1);
    gl_Position = projection * view_vertex;
}
