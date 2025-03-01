#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;
in float Height;

out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D GrassTexture;
uniform sampler2D DirtTexture;
uniform sampler2D RockTexture;
uniform sampler2D SnowTexture;

uniform vec2 GrassHeightRange;
uniform vec2 RockHeightRange;
uniform vec2 SnowHeightRange;

uniform vec2 ColorTextureScale;


float interpolateHeight(vec2 range, float height) {

   return clamp((height - range.x) / (range.y - range.x), 0.0, 1.0);
}

void main()
{
    vec4 color1, color2;
    float factor;

    factor = interpolateHeight(GrassHeightRange, Height);
    color1 = texture(DirtTexture, TexCoord * ColorTextureScale);
    color2 = texture(GrassTexture, TexCoord * ColorTextureScale);
    vec4 grassDirtColor = mix(color1, color2, factor);

    factor = interpolateHeight(RockHeightRange, Height);
    color1 = texture(GrassTexture, TexCoord * ColorTextureScale);
    color2 = texture(RockTexture, TexCoord * ColorTextureScale);
    vec4 rockGrassColor = mix(color1, color2, factor);

    factor = interpolateHeight(SnowHeightRange, Height);
    color1 = texture(RockTexture, TexCoord * ColorTextureScale);
    color2 = texture(SnowTexture, TexCoord * ColorTextureScale);
    vec4 snowRockColor = mix(color1, color2, factor);

    vec4 finalColor;
    if (Height < GrassHeightRange.x) {
        finalColor = grassDirtColor;
    } else if (Height < GrassHeightRange.y) {
        finalColor = grassDirtColor;
    } else if (Height < RockHeightRange.x) {
        finalColor = rockGrassColor;
    } else if (Height < RockHeightRange.y) {
        finalColor = rockGrassColor;
    } else if (Height < SnowHeightRange.x) {
        finalColor = snowRockColor;
    } else {
        finalColor = snowRockColor;
    }

    FragColor = Color * finalColor;
}
