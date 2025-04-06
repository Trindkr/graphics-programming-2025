//Inputs
in vec2 TexCoord;

//Outputs
out vec4 FragColor;

//Uniforms
uniform sampler2D SourceTexture;
uniform vec2 Range;
uniform float Intensity;

void main()
{
	vec4 color = texture(SourceTexture, TexCoord);
	float luminance = GetLuminance(color.rgb);

	float mapped = smoothstep(Range.x, Range.y, luminance);

	FragColor = vec4(color.rgb * mapped * Intensity, color.a);
}
