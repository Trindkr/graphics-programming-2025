#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D WaterTexture;
uniform vec2 ColorTextureScale;
uniform vec4 Color;

void main()
{
	FragColor = Color * texture(WaterTexture, TexCoord * ColorTextureScale);
}
