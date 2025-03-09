#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D ColorTexture;

uniform vec3 AmbientColor;
uniform float AmbientReflection;

uniform float DiffuseReflection;
uniform vec3 LightColor;
uniform vec3 LightPosition;

vec3 GetAmbientReflection(vec4 color)
{

	return AmbientColor * AmbientReflection * color.rgb;
}

vec3 GetDiffuseReflection(vec4 color, vec3 lightDirection, vec3 worldNormal)
{
	return LightColor * DiffuseReflection * color.rgb * clamp(dot(worldNormal, lightDirection), 0, 1);
}

vec3 GetBlinnPhongReflection(vec4 color, vec3 lightDirection, vec3 worldNormal)
{

	return GetAmbientReflection(color) + GetDiffuseReflection(color, lightDirection, worldNormal);
}

void main()
{

	vec3 lightDirection = normalize(LightPosition - WorldPosition);
	vec3 worldNormal = normalize(WorldNormal);

	vec4 color = texture(ColorTexture, TexCoord) * Color;
	vec3 blinnPhongReflection = GetBlinnPhongReflection(color, lightDirection, worldNormal);
	FragColor = vec4(blinnPhongReflection, 1);
}






