#version 330 core
out vec4 FragColor;

uniform vec3 object_color;
// in vec2 tex_coord;
// unform sampler2D texture;


void main() 
{
    FragColor = vec4(object_color, 1.0f);
}