#version 400
out vec4 Out_Color;

in VertexGF {
    vec4 color;
	vec2 texPos;
} vertexGF;

uniform sampler2D ourTexture;

void main()
{
	Out_Color = texture(ourTexture, vertexGF.texPos);
	//Out_Color = vec4(1.0, 0.0, 0.0, 1.0);
 }