#version 400
in vec4 vertex_Position;
in vec4 vertex_Normal;
 
out Vertex
{
    vec4 normal;
    vec4 color;
} vertex;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
 
void main()
{
  gl_Position = vertex_Position;
  vertex.normal = vertex_Normal;
  vertex.color =  vec4(1.0, 1.0, 0.0, 1.0);
}