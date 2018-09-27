#version 330 core

//attribute vec3 vertex;
//attribute vec3 normal;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat3 normalMatrix;

smooth out vec3 N;

void main(void)  {
    vec4 view_vertex = modelview * vec4(vertex, 1);
    N = normalize(normal);
    gl_Position = projection * view_vertex;
}
