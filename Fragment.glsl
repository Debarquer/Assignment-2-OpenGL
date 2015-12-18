#version 400
out vec4 Out_Color;

in VertexGF {
    vec4 color;
	vec2 texPos;
	vec3 normal;
} vertexGF;

uniform mat4 world;
uniform sampler2D ourTexture;
uniform vec3 lightPosition;

in vec3 gPosGF;

void main()
{
	//Create a vector from the light to the point
	vec3 lightVector = lightPosition - gPosGF;

	//Normalize the vectors
	vec3 lightNor = normalize(lightVector);
	vec3 posNor = normalize(vertexGF.normal);

	//Calculate the angle between the light and the point
	float angle = max(dot(lightNor, posNor), 0.0);

	Out_Color = angle * texture(ourTexture, vertexGF.texPos);
	//Out_Color = vertexGF.color * angle;

	if(Out_Color.w < 0.1)
		discard;
	//Out_Color = vec4(vertexGF.normal, 1.0);
 }