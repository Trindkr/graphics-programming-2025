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

uniform float SpecularReflection;
uniform float SpecularExponent;

uniform vec3 CameraPosition;

vec3 GetAmbientReflection(vec4 color)
{

	return AmbientColor * AmbientReflection * color.rgb;
}

vec3 GetDiffuseReflection(vec4 color, vec3 lightDirection, vec3 worldNormal)
{
	return LightColor * DiffuseReflection * color.rgb * clamp(dot(worldNormal, lightDirection), 0, 1);
}

vec3 GetSpecularReflection(vec3 halfVector)
{
	return LightColor * SpecularReflection * pow(clamp(dot(WorldNormal, halfVector), 0, 1), SpecularExponent);
}

vec3 GetBlinnPhongReflection(vec4 color, vec3 lightDirection, vec3 worldNormal, vec3 halfVector)
{

	return GetAmbientReflection(color) + GetDiffuseReflection(color, lightDirection, worldNormal) + GetSpecularReflection(halfVector);
}

void main()
{

	vec3 lightDirection = normalize(LightPosition - WorldPosition);
	vec3 worldNormal = normalize(WorldNormal);
	vec3 viewVector = normalize(CameraPosition - WorldPosition);
	vec3 halfVector = normalize(lightDirection + viewVector);

	vec4 color = texture(ColorTexture, TexCoord) * Color;
	vec3 blinnPhongReflection = GetBlinnPhongReflection(color, lightDirection, worldNormal, halfVector);
	FragColor = vec4(blinnPhongReflection, 1);
}






