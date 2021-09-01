#version 330 core

in vec3 v_color;
in vec2 v_tex_coord;

uniform sampler2D texture0;

out vec4 frag_color;

void main()
{
  frag_color = texture(texture0, v_tex_coord) * vec4(v_color, 1.0);
}
