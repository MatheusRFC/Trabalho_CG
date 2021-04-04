#version 330 core

uniform sampler2D amostraTextura;

in vec3 Normal;
in vec3 Color;
in vec2 UV;

uniform vec3 LightDirection;
uniform float LightIntensity;

out vec4 OutColor;

void main()
{
	//Renormaliza a normal para evitar problemas relacionados a inerpolação linear
	vec3 N = normalize(Normal);

	//Inverte a direção da luz para calcular o vetor LightDirection
	vec3 L = -normalize(LightDirection);

	float Lambertian = max(dot(N, L), 0.0);

	vec3 CorTextura = texture(amostraTextura, UV).rgb;
	vec3 FinalColor = CorTextura * LightIntensity * Lambertian;

	OutColor = vec4(FinalColor, 1.0);
}  