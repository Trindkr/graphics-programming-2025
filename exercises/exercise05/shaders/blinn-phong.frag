#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D ColorTexture;
uniform vec3 AmbientColor;
uniform float AmbientReflection;

vec3 GetAmbientReflection(vec4 color)
{

	return AmbientColor * AmbientReflection * color.rgb;
}

vec3 GetBlinnPhongReflection(vec4 color)
{

	return GetAmbientReflection(color);
}

void main()
{
	vec4 color = texture(ColorTexture, TexCoord) * Color;
	vec3 blinnPhongReflection = GetBlinnPhongReflection(color);
	FragColor = vec4(blinnPhongReflection, 1);
}






