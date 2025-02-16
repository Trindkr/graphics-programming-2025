#version 330 core

// (todo) 02.X: Add more vertex attributes
layout (location = 0) in vec2 ParticlePosition;
layout (location = 1) in float ParticleSize;
layout (location = 2) in float ParticleBirth;
layout (location = 3) in float ParticleDuration;
layout (location = 4) in vec4 ParticleColor;
layout (location = 5) in vec2 ParticleVelocity;

//Add Color output variable here
out vec4 Color;

// (todo) 02.X: Add uniforms
uniform float CurrentTime;
uniform vec2 Gravity;

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

    vec2 constantVelocity = vec2(0.0, 1.0); // Example constant velocity
    vec2 position = ParticlePosition + ParticleVelocity * lifeSpan + 0.5 * Gravity * lifeSpan * lifeSpan + constantVelocity * lifeSpan;

	gl_PointSize = ParticleSize;
	gl_Position = vec4(position, 0.0, 1.0);
}
