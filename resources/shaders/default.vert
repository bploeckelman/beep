#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;
layout (location = 2) in vec2 a_tex_coord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 v_color;
out vec2 v_tex_coord;

void main()
{
  gl_Position = projection * view * model * vec4(a_pos, 1.0);
  v_color = a_color;
  v_tex_coord = a_tex_coord;
}
