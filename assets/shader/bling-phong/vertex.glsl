#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 noraml;

out vec3 frag_pos;
out vec3 frag_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    frag_pos = vec3(model * vec4(pos, 1.0f)); // use world coordinate to compute lighting.
    frag_normal = mat3(transpose(inverse(model))) * noraml;
    gl_Position = projection * view * vec4(frag_pos, 1.0f);
}