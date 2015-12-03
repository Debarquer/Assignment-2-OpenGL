#version 400
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

out VertexVG
{
    vec4 color;
}vertexVG;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 world;
 
void main()
{
  gl_Position = vec4(vertex_position, 1.0);
  vertexVG.color =  vec4(vertex_color, 1.0);
}