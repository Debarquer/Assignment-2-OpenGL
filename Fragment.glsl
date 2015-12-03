#version 400
out vec4 Out_Color;

in VertexGF {
    vec4 color;
} vertexGF;

void main()
{
  Out_Color = vertexGF.color;
}