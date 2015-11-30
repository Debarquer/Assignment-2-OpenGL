#version 400
in vec4 vertex_color;
out vec4 Out_Color;

in Vertex {
    vec4 normal;
    vec4 color;
} VertexIn;

void main()
{
  Out_Color = VertexIn.color;
}