#version 330 core

in	vec3 vcolor; 
out vec4 color;

void main()
{
    color = vec4(vcolor.x, vcolor.y, vcolor.z, 1.0f);
} 