#pragma once

#include <ituGL/application/Application.h>

#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/geometry/Mesh.h>
#include <glm/gtx/transform.hpp>
#include <ituGL/camera/camera.h>

class GearsApplication : public Application
{
public:
    GearsApplication(int width, int height);

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;

private:
    // Initialize the gear meshes
    void InitializeGeometry();

    // Load, compile and link shaders
    void InitializeShaders();

    // Create a new gear mesh:
    // cogCount: Number of cogs (teeth)
    // innerRadius: Radius of the small circle inside the gear
    // pitchRadius: Radius of the gear, at the distance where it makes contact, in the middle of the cog length
    // addendum: Height of the cogs
    // depth: Size of the cog in Z-coordinate
    void CreateGearMesh(Mesh& mesh, unsigned int cogCount, float innerRadius, float pitchRadius, float addendum, float cogRatio, float depth);

    // Draw a gear mesh with a specific world matrix and color
    void DrawGear(const Mesh& mesh, const glm::mat4& worldMatrix, const Color& color);

    // Helper function to encapsulate loading and compiling a shader
    void LoadAndCompileShader(Shader& shader, const char* path);

private:

    float aspectRatio;

    const float m_rotationSpeed = 1.0f;
	const float m_smallGearRotationOffset = glm::radians(8.5f); // Initial rotation offset for the gears

    // Gear meshes
    Mesh m_largeGear;
    Mesh m_mediumGear;
    Mesh m_smallGear;

    // Shader program
    ShaderProgram m_shaderProgram;

    // Shader uniform locations
	ShaderProgram::Location m_colorUniform; // Color uniform
	ShaderProgram::Location m_worldMatrixUniform; // World matrix uniform
	ShaderProgram::Location m_viewProjMatrixUniform; //viewProj matrix uniform

    

	// Camera
	Camera m_camera;

};
