//Inputs
in vec2 TexCoord;

//Outputs
out vec4 FragColor;

//Uniforms
uniform sampler2D SourceTexture;
uniform float Exposure;
uniform float Contrast;
uniform float HueShift;
uniform float Saturation;
uniform vec3 ColorFilter;
uniform sampler2D BloomTexture;

void main()
{
	vec3 color = texture(SourceTexture, TexCoord).rgb;
	vec3 bloom = texture(BloomTexture, TexCoord).rgb;

	//Bloom
	color += bloom;

	// Exposure
	vec3 exposedColor = 1.0 - exp(-color * Exposure).rgb;
	
	// Contrast
	vec3 contrastColor = clamp((exposedColor.rgb - vec3(0.5)) * Contrast + vec3(0.5), vec3(0.0), vec3(1.0));
	
	// Hue Shift
	vec3 hsv = RGBToHSV(contrastColor);
	hsv.x += HueShift;
	hsv.x = fract(hsv.x);
	vec3 hueShiftColor = HSVToRGB(hsv);

	// Saturation
	float luminanceColor = GetLuminance(hueShiftColor);
	vec3 saturationColor = clamp((hueShiftColor-vec3(luminanceColor))*Saturation + vec3(luminanceColor), vec3(0.0), vec3(1.0));

	// Color Filter
	vec3 colorFiltered = saturationColor * ColorFilter;

	FragColor = vec4(colorFiltered,1.0);
}
