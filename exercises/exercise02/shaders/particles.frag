#version 330 core

out vec4 FragColor;

// (todo) 02.5: Add Color input variable here


void main()
{
	// (todo) 02.3: Compute alpha using the built-in variable gl_PointCoord

	vec2 coord = gl_PointCoord * 2.0 - vec2(1.0); // Convert the range from (0, 1) to (-1, 1)
    float intensity = 1.0 - length(coord);
	FragColor = vec4(1.0, 1.0, 1.0, intensity);

}

