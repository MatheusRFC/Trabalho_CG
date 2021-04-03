#version 330 core

uniform sampler2D amostraTextura;

in vec3 Color;
out vec4 OutColor;
in vec2 UV;

void main()
{

	vec3 CorTextura = texture(amostraTextura, UV).rgb;

	OutColor = vec4(CorTextura, 1.0);
}  