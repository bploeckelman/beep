#version 460

layout(location = 0) out vec4 color;

in vec4 v_color;

void main()
{
    color = v_color;
//    vec2 coord = (gl_PointCoord.xy - center) / radius_pix;
//    float len = distance(gl_PointCoord, vec2(0.5));
//    if (len > 0.5) {
//        discard;
//    }
//    float percent = distance(gl_PointCoord, vec2(0.5));
//    vec3 pos = vec3(coord, sqrt(1.0 - len * len));
//    color = vec4(vec3(pos.z), 1.0);
}
