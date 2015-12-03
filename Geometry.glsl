#version 400
layout(triangles) in;
layout (triangle_strip, max_vertices=32) out;

in VertexVG {
	vec4 color;
	vec2 texPos;
} vertexVG[];

out VertexGF {
	vec4 color;
	vec2 texPos;
}vertexGF;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 world;
uniform sampler2D ourTexture;

void main()
{
	for(int i = 0; i < gl_in.length(); i++)
	{
		gl_Position = projection * view * world * gl_in[i].gl_Position;
		vertexGF.color = vertexVG[i].color;
		vertexGF.texPos = vertexVG[i].texPos;

		EmitVertex();
	}
	EndPrimitive();

	vec3 a = vec3(gl_in[0].gl_Position)-vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position)-vec3(gl_in[1].gl_Position);
	vec3 normal = normalize(cross(a, b));

	vec4 normal4 = vec4(normal, 0.0);

	for(int i = 0; i < gl_in.length(); i++)
	{
		gl_Position = projection * view * world * (gl_in[i].gl_Position + normal4);
		vertexGF.color = vertexVG[i].color;
		vertexGF.texPos = vertexVG[i].texPos;

		EmitVertex();
	}
	EndPrimitive();
}