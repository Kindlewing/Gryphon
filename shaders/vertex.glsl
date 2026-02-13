#version 330 core
layout (location = 0) in vec3 pos;
out vec4 color;

uniform mat4 projection;
uniform mat4 transform;

void main() {
    gl_Position = projection * transform * vec4(pos, 1.0);
    color = vec4(0.5, 0.0, 0.0, 1.0);
}
