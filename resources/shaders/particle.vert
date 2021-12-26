#version 460

layout (location = 0) in vec4 a_position;
layout (location = 1) in vec4 a_color;

uniform float u_pointsize;
uniform vec3 u_campos;
uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
};
out vec4 v_color;

void main()
{
    float camera_dist = distance(a_position.xyz, u_campos);
    float point_scale = 1.0; // todo: change based on camera_dist
    gl_Position = u_proj * u_view * u_model * a_position;
    gl_PointSize = u_pointsize * point_scale;
    v_color = a_color;
}
