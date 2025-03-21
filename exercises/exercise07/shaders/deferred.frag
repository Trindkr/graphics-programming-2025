//Inputs
in vec2 TexCoord;

//Outputs
out vec4 FragColor;

//Uniforms
uniform sampler2D DepthTexture;
uniform sampler2D AlbedoTexture;
uniform sampler2D NormalTexture;
uniform sampler2D OthersTexture;
uniform mat4 InvViewMatrix;
uniform mat4 InvProjMatrix;


void main()
{
	//FragColor = texture(AlbedoTexture, TexCoord);
	//FragColor = vec4(GetImplicitNormal(texture(NormalTexture, TexCoord).xy), 1.0);

    vec3 viewPosition = ReconstructViewPosition(DepthTexture, TexCoord, InvProjMatrix);

	 // Compute the view vector in view space
    vec3 viewVector = viewPosition - vec3(0);

	// Transform the position and view vector to world coordinates
    vec4 worldPosition = InvViewMatrix * vec4(viewPosition, 1.0);
    vec4 worldViewVector = InvViewMatrix * vec4(viewVector, 0.0);

	//FragColor = vec4(viewPosition, 1.0);
	vec4 inverseNormal = InvViewMatrix * vec4(GetImplicitNormal(texture(NormalTexture, TexCoord).xy), 0.0);

	vec4 others = texture(OthersTexture, TexCoord);

	SurfaceData data;
	data.normal = inverseNormal.xyz;
	data.reflectionColor = texture(AlbedoTexture, TexCoord).rgb;
	data.ambientReflectance = others.x;
	data.diffuseReflectance = others.y;
	data.specularReflectance = others.z;
	data.specularExponent = (1 / others.w)+1;

	// Compute the lighting
    vec3 lighting = ComputeLighting(worldPosition.xyz, data, worldViewVector.xyz, true);

	FragColor = vec4(lighting, 1.0);


}
