#version 330 core

layout (location = 0) in vec2 ParticlePosition;
layout (location = 1) in float ParticleSize;
layout (location = 2) in float ParticleBirth;
layout (location = 3) in float ParticleDuration;
layout (location = 4) in vec4 ParticleColor;

// (todo) 02.X: Add more vertex attributes

//Add Color output variable here
out vec4 Color;


// (todo) 02.X: Add uniforms
uniform float CurrentTime;

void main()
{
	Color = ParticleColor;
	float lifeSpan = CurrentTime - ParticleBirth;
	if (lifeSpan > ParticleDuration)
	{
		gl_PointSize = 0.0;
		gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
		return;
	}

	gl_PointSize = ParticleSize;
	gl_Position = vec4(ParticlePosition, 0.0, 1.0);
}
