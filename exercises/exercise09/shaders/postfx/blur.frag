//Inputs
in vec2 TexCoord;

//Outputs
out vec4 FragColor;

//Uniforms
uniform sampler2D SourceTexture;
uniform vec2 Scale; // Scale to adjust to the resolution, and to select direction
uniform float BlurStrength;

// Offset (in pixels) where to sample the neighbors. We sample between texels to take advantage of the linear filtering
const float offsets[3] = float[](0.0, 1.3846153846f, 3.2307692308f);
//const float offsets[5] = float[](0.0, 1.0, 2.0, 3.0, 4.0);

// Weights of each sample. Notice that the first one gets added once, but the others are added twice, for positive and negative. Total sum is 1
const float weights[3] = float[](0.2270270270f, 0.3162162162f, 0.0702702703f);
//const float weights[5] = float[](0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f);


void main()
{
    if (BlurStrength <= 0.0001)
    {
        FragColor = texture(SourceTexture, TexCoord);
        return;
    }

   // Sample the pixel at the center
   vec4 color = texture(SourceTexture, TexCoord) * weights[0];

   // Sample the pixel at the sides
   //for (int i = 1; i < 5; i++)
   for (int i = 1; i < 3; i++)
   {
      vec2 scaledOffset = Scale * offsets[i] * BlurStrength;
      color += texture(SourceTexture, TexCoord + scaledOffset) * weights[i];
      color += texture(SourceTexture, TexCoord - scaledOffset) * weights[i];
   }

   FragColor = color;
}
