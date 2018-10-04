#version 330 core
smooth in vec2 texCoords;

out vec4 frag_color;
uniform vec3 color;


void main(){
    if (pow(texCoords.x, 2) + pow(texCoords.y, 2) < pow(.5, 2)) {
        frag_color = vec4(1, 0, 0, 1);
    }
    else {frag_color = vec4(1, 1, 1, 0);}
}
