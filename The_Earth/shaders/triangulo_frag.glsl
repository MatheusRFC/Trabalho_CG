#version 330 core

uniform sampler2D amostraTextura;
uniform sampler2D CloudsTexture;

uniform float time;
uniform vec2 CloudsRotationSpeed = vec2(0.0008, 0.0);

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

	//Vetor V 
	vec3 ViewDirection = vec3(0.0, 0.0, -1.0);
	vec3 V = -ViewDirection;

	//Vetor R 
	vec3 R = reflect(-L, N);

	//Termo Especular R . V ^ Alpha
	float alpha = 50.0;
	float Especular = pow(dot(R, V), alpha);
	Especular = max(Especular, 0.0);

	vec3 EarthColor = texture(amostraTextura, UV+ time * CloudsRotationSpeed*2).rgb;
	vec3 CloudColour = texture(CloudsTexture, UV + time * CloudsRotationSpeed).rgb;
	vec3 FinalColor = (EarthColor+CloudColour) * LightIntensity * Lambertian + Especular;

	OutColor = vec4(FinalColor, 1.0);
}  