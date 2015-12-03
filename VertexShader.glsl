#version 400
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;
layout(location = 2) in vec2 texture_position;

out VertexVG
{
    vec4 color;
	vec2 texPos;
}vertexVG;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 world;
uniform sampler2D ourTexture;
 
void main()
{
  gl_Position = vec4(vertex_position, 1.0);
  vertexVG.color =  vec4(vertex_color, 1.0);
  vertexVG.texPos = vec2(1.0 - texture_position.x, texture_position.y);
}