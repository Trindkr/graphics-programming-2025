#include "GearsApplication.h"

#include <ituGL/shader/Shader.h>
#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/geometry/VertexFormat.h>
#include <ituGL/camera/camera.h>
#include <cassert>  // for asserts
#include <array>    // to get shader error messages
#include <fstream>  // shader loading
#include <sstream>  // shader loading
#include <iostream> // to print messages to the console
#include <vector>   // to store vertices and indices
#include <numbers>  // for PI constant
#include <glm/gtx/transform.hpp>  // for matrix transformations


GearsApplication::GearsApplication(int width, int height)
    : Application(width, height, "Gears demo")  // Use the defined width and height
    , m_colorUniform(-1)
    , m_worldMatrixUniform(-1)
    , m_viewProjMatrixUniform(-1)
{
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

void GearsApplication::Initialize()
{
    Application::Initialize();

    InitializeGeometry();

    InitializeShaders();

    GetDevice().EnableFeature(GL_DEPTH_TEST);
}

void GearsApplication::Update()
{
    Application::Update();

    const Window& window = GetMainWindow();
    int width, height;
    window.GetDimensions(width, height);
    aspectRatio = static_cast<float>(width) / static_cast<float>(height); // Update aspect ratio 

    // Update the camera matrices
    //m_camera.SetOrthographicProjectionMatrix(glm::vec3(-aspectRatio, -1.0f, -5.0f), glm::vec3(aspectRatio, 1.0f, 5.0f));
    m_camera.SetPerspectiveProjectionMatrix((3.1454 / 2.f), aspectRatio, 0.1f, 20.0f);
    glm::vec2 mousePosition = window.GetMousePosition(true);
    glm::vec3 lookAt = glm::vec3(mousePosition.x, mousePosition.y, 0.0f); // Center of the scene
    m_camera.SetViewMatrix(glm::vec3(0.0f, 0.5f, 1.5f), lookAt); // Set the camera position and look at the center of the scene
}

void GearsApplication::Render()
{
    // Clear background
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f), true, 1.0f);

    // Set our shader program
    m_shaderProgram.Use();

    m_shaderProgram.SetUniform(m_viewProjMatrixUniform, m_camera.GetViewProjectionMatrix()); // Set the value of the ViewProjMatrix uniform

    glm::mat4 centerGearMatrix(1.0f);
    float currentTime = static_cast<float>(glfwGetTime());
    glm::vec3 rotationAxis(0.0f, 0.0f, 1.0f);  // rotate around z


    // Draw large gear to the right
    float largeGear_rotationAngle = m_rotationSpeed * currentTime; // Rotate 1 degree per second
    glm::mat4 largeGear_rotationMatrix = glm::rotate(glm::mat4(1.0f), largeGear_rotationAngle, rotationAxis);

    DrawGear(m_largeGear, largeGear_rotationMatrix, Color(1.0f, 1.0f, 1.0f)); // Draw large gear

    //Draw medium gear to the right
    float mediumGear_rotationAngle = m_rotationSpeed * currentTime * (16.0f / 8.0f);; // Rotate 1 degree per second. big gear has 16 cogs, medium gear has 8 cogs
    glm::mat4 mediumGear_transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.75f, 0.0f, 0.0f)); // Translate to the right
    mediumGear_transformMatrix = glm::rotate(mediumGear_transformMatrix, -mediumGear_rotationAngle, rotationAxis); // Rotate in the opposite direction

    DrawGear(m_mediumGear, mediumGear_transformMatrix, Color(0.0f, 1.0f, 0.0f)); // Draw medium gear

    //  Draw small gear at the top-left corner

    float smallGear_rotationAngle = -m_rotationSpeed * currentTime * (16.0f / 30.0f);;

    glm::mat4 smallGear_transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, 0.0f)); // Translate to the top-left corner
    smallGear_transformMatrix = glm::scale(smallGear_transformMatrix, glm::vec3(7.5f, 7.5f, 7.5f)); // scale
    smallGear_transformMatrix = glm::rotate(smallGear_transformMatrix, smallGear_rotationAngle + m_smallGearRotationOffset, rotationAxis); // Rotate

    DrawGear(m_smallGear, smallGear_transformMatrix, Color(1.0f, 0.0f, 1.0f)); // Draw small gear

    // Draw small gear linked to the center gear
    glm::mat4 smallLinkedGear_transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.2f, 0.0f)); // Translate to the center
    smallLinkedGear_transformMatrix = largeGear_rotationMatrix * smallLinkedGear_transformMatrix;
    smallLinkedGear_transformMatrix = glm::rotate(smallLinkedGear_transformMatrix, smallGear_rotationAngle, rotationAxis);

    DrawGear(m_smallGear, smallLinkedGear_transformMatrix, Color(1.f, 1.f, .2f)); // Draw small gear linked to the center gear

    Application::Render();
}

// Create the meshes that we will use during the exercise
void GearsApplication::InitializeGeometry()
{
    CreateGearMesh(m_largeGear, 16, 0.2f, 0.5f, 0.1f, 0.5f, 0.1f);
    CreateGearMesh(m_mediumGear, 8, 0.1f, 0.25f, 0.1f, 0.5f, 0.1f);
    CreateGearMesh(m_smallGear, 30, 0.05f, 0.121f, 0.0125f, 0.5f, 0.015f);
}

// Load, compile and Build shaders
void GearsApplication::InitializeShaders()
{
    // Load and compile vertex shader
    Shader vertexShader(Shader::VertexShader);
    LoadAndCompileShader(vertexShader, "shaders/basic.vert");

    // Load and compile fragment shader
    Shader fragmentShader(Shader::FragmentShader);
    LoadAndCompileShader(fragmentShader, "shaders/basic.frag");

    // Attach shaders and link
    if (!m_shaderProgram.Build(vertexShader, fragmentShader))
    {
        std::cout << "Error linking shaders" << std::endl;
        return;
    }

    m_colorUniform = m_shaderProgram.GetUniformLocation("Color");
    m_worldMatrixUniform = m_shaderProgram.GetUniformLocation("WorldMatrix");
    m_viewProjMatrixUniform = m_shaderProgram.GetUniformLocation("ViewProjMatrix");

}

// Draw a gear mesh with a specific world matrix and color
void GearsApplication::DrawGear(const Mesh& mesh, const glm::mat4& worldMatrix, const Color& color)
{
    m_shaderProgram.SetUniform(m_colorUniform, static_cast<glm::vec3>(color));
    m_shaderProgram.SetUniform(m_worldMatrixUniform, worldMatrix); // Set the value of the WorldMatrix uniform

    mesh.DrawSubmesh(0);
}

void GearsApplication::CreateGearMesh(Mesh& mesh, unsigned int cogCount, float innerRadius, float pitchRadius, float addendum, float cogRatio, float depth)
{
    // Define the vertex structure
    struct Vertex
    {
        Vertex() = default;
        Vertex(const glm::vec3& position, const glm::vec3& normal) : position(position), normal(normal) {}
        glm::vec3 position;
        glm::vec3 normal;
    };

    // Define the vertex format (should match the vertex structure)
    VertexFormat vertexFormat;
    vertexFormat.AddVertexAttribute<float>(3);
    vertexFormat.AddVertexAttribute<float>(3);

    // List of vertices (VBO)
    std::vector<Vertex> vertices;

    // List of indices (EBO)
    std::vector<unsigned short> indices;

    // Mesh will have 2 sides for each cog
    unsigned int sides = 2 * cogCount;

    // Base delta angle for each side is 2*PI / sides
    float deltaAngle = 2 * static_cast<float>(std::numbers::pi) / sides;

    // Useful radius
    float outerRadius = pitchRadius - addendum * 0.5f;
    float teethRadius = pitchRadius + addendum * 0.5f;

    // Offsets to ensure the cogs fit
    float offsetOuterAngle = -deltaAngle * (0.5f - cogRatio / (1 + outerRadius / teethRadius));
    float offsetTeethAngle = deltaAngle * (0.5f - cogRatio / (1 + teethRadius / outerRadius));

    // Centered at 0. Half depth on each side
    float halfDepth = 0.5f * depth;

    // Loop over all the sides creating the vertices
    for (unsigned int i = 0; i < sides; ++i)
    {
        // Detect if it is a cog or a hole
        bool isCog = i % 2 == 1;
        int cogSign = isCog ? 1 : -1;

        // Add vertices
        {
            // Precompute normals
            glm::vec3 frontNormal = glm::vec3(0.0f, 0.0f, 1.0f);
            glm::vec2 teethNormal2D(halfDepth * (1.0f - cogRatio), addendum);
            teethNormal2D = glm::normalize(teethNormal2D);
            glm::vec3 teethFrontNormal(0);
            glm::vec3 teethBackNormal(0);

            // Inner circle
            float innerAngle = i * deltaAngle;
            glm::vec2 innerDirection(std::sin(innerAngle), std::cos(innerAngle));
            glm::vec3 innerFrontPosition = glm::vec3(innerDirection * innerRadius, halfDepth);
            glm::vec3 innerBackPosition = glm::vec3(innerDirection * innerRadius, -halfDepth);
            vertices.emplace_back(innerFrontPosition, frontNormal); // 0
            vertices.emplace_back(innerBackPosition, -frontNormal); // 1
            // Break for center hole normals
            vertices.emplace_back(innerFrontPosition, glm::vec3(-innerDirection, 0.0f)); // 2
            vertices.emplace_back(innerBackPosition, glm::vec3(-innerDirection, 0.0f));  // 3

            // Outer circle
            float outerAngle = innerAngle + offsetOuterAngle * cogSign;
            glm::vec2 outerDirection(std::sin(outerAngle), std::cos(outerAngle));
            glm::vec3 outerFrontPosition = glm::vec3(outerDirection * outerRadius, halfDepth);
            glm::vec3 outerBackPosition = glm::vec3(outerDirection * outerRadius, -halfDepth);
            vertices.emplace_back(outerFrontPosition, frontNormal);  // 4
            vertices.emplace_back(outerBackPosition, -frontNormal);  // 5
            // Break for teeth front normals
            teethFrontNormal = glm::vec3(outerDirection * teethNormal2D.x, teethNormal2D.y);
            teethBackNormal = glm::vec3(outerDirection * teethNormal2D.x, -teethNormal2D.y);
            vertices.emplace_back(outerFrontPosition, teethFrontNormal); // 6
            vertices.emplace_back(outerBackPosition, teethBackNormal);   // 7
            // Break for teeth hole normals
            vertices.emplace_back(outerFrontPosition, glm::vec3(outerDirection, 0.0f)); // 8
            vertices.emplace_back(outerBackPosition, glm::vec3(outerDirection, 0.0f));  // 9
            // Break for teeth side normals
            float sideAngle = offsetTeethAngle - offsetOuterAngle;
            glm::vec3 teethSideNormal(std::sin(sideAngle) * innerDirection - cogSign * std::cos(sideAngle) * glm::vec2(innerDirection.y, -innerDirection.x), 0.0f);
            vertices.emplace_back(outerFrontPosition, teethSideNormal); // 10
            vertices.emplace_back(outerBackPosition, teethSideNormal);  // 11

            // Teeth circle
            float teethAngle = innerAngle + offsetTeethAngle * cogSign;
            glm::vec2 teethDirection(std::sin(teethAngle), std::cos(teethAngle));
            glm::vec3 teethFrontPosition = glm::vec3(teethDirection * teethRadius, halfDepth * cogRatio);
            glm::vec3 teethBackPosition = glm::vec3(teethDirection * teethRadius, -halfDepth * cogRatio);
            teethFrontNormal = glm::vec3(teethDirection * teethNormal2D.x, teethNormal2D.y);
            teethBackNormal = glm::vec3(teethDirection * teethNormal2D.x, -teethNormal2D.y);
            vertices.emplace_back(teethFrontPosition, teethFrontNormal); // 12
            vertices.emplace_back(teethBackPosition, teethBackNormal);   // 13
            // Break for teeth cap normals
            vertices.emplace_back(teethFrontPosition, glm::vec3(teethDirection, 0.0f)); // 14
            vertices.emplace_back(teethBackPosition, glm::vec3(teethDirection, 0.0f));  // 15
            // Break for teeth side normals
            vertices.emplace_back(teethFrontPosition, teethSideNormal); // 16
            vertices.emplace_back(teethBackPosition, teethSideNormal);  // 17
        }

        // Add triangles
        {
            unsigned vertexPerSide = 18;
            unsigned short index0 = vertexPerSide * i;
            unsigned short index1 = vertexPerSide * ((i + 1) % sides);

            // Body (Front)
            indices.push_back(index0 + 0); indices.push_back(index1 + 0); indices.push_back(index0 + 4);
            indices.push_back(index0 + 4); indices.push_back(index1 + 0); indices.push_back(index1 + 4);
            // Body (Back)
            indices.push_back(index0 + 1); indices.push_back(index1 + 1); indices.push_back(index0 + 5);
            indices.push_back(index0 + 5); indices.push_back(index1 + 1); indices.push_back(index1 + 5);
            // Center hole
            indices.push_back(index0 + 3); indices.push_back(index1 + 3); indices.push_back(index0 + 2);
            indices.push_back(index0 + 2); indices.push_back(index1 + 3); indices.push_back(index1 + 2);

            // Cog side
            indices.push_back(index0 + 10); indices.push_back(index0 + 16); indices.push_back(index0 + 11);
            indices.push_back(index0 + 11); indices.push_back(index0 + 16); indices.push_back(index0 + 17);
            if (isCog)
            {
                // Cog (Front)
                indices.push_back(index0 + 6); indices.push_back(index1 + 6); indices.push_back(index0 + 12);
                indices.push_back(index0 + 12); indices.push_back(index1 + 6); indices.push_back(index1 + 12);
                // Cog (Back)
                indices.push_back(index0 + 7); indices.push_back(index1 + 7); indices.push_back(index0 + 13);
                indices.push_back(index0 + 13); indices.push_back(index1 + 7); indices.push_back(index1 + 13);
                // Cog cap
                indices.push_back(index0 + 15); indices.push_back(index0 + 14); indices.push_back(index1 + 15);
                indices.push_back(index1 + 15); indices.push_back(index0 + 14); indices.push_back(index1 + 14);
            }
            else
            {
                //Cog hole
                indices.push_back(index0 + 9); indices.push_back(index0 + 8); indices.push_back(index1 + 9);
                indices.push_back(index1 + 9); indices.push_back(index0 + 8); indices.push_back(index1 + 8);
            }
        }
    }

    // Finally create the new submesh with all the data
    mesh.AddSubmesh<Vertex, unsigned short, VertexFormat::LayoutIterator>(Drawcall::Primitive::Triangles, vertices, indices,
        vertexFormat.LayoutBegin(static_cast<int>(vertices.size()), true /* interleaved */), vertexFormat.LayoutEnd());
}

void GearsApplication::LoadAndCompileShader(Shader& shader, const char* path)
{
    // Open the file for reading
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cout << "Can't find file: " << path << std::endl;
        std::cout << "Is your working directory properly set?" << std::endl;
        return;
    }

    // Dump the contents into a string
    std::stringstream stringStream;
    stringStream << file.rdbuf() << '\0';

    // Set the source code from the string
    shader.SetSource(stringStream.str().c_str());

    // Try to compile
    if (!shader.Compile())
    {
        // Get errors in case of failure
        std::array<char, 256> errors;
        shader.GetCompilationErrors(errors);
        std::cout << "Error compiling shader: " << path << std::endl;
        std::cout << errors.data() << std::endl;
    }
}