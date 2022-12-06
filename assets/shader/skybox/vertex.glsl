#version 330 core
layout (location = 0) in vec3 pos;

out vec3 tex_coords;

uniform mat4 projection;
uniform mat4 view;

void main() {
    tex_coords = pos;
    vec4 pos_camera = projection * view * vec4(pos, 1.0);
    gl_Position = pos_camera.xyww; // optimization to set z tp 1.0f
}