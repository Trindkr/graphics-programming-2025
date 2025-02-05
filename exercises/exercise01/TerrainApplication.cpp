#include "TerrainApplication.h"
#include "ituGL/geometry/VertexAttribute.h"

#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"

#include <vector>
#include <cmath>
#include <iostream>



// Helper structures. Declared here only for this exercise
struct Vector2
{
    Vector2() : Vector2(0.f, 0.f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    float x, y;
};

struct Vector3
{
    Vector3() : Vector3(0.f,0.f,0.f) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    float x, y, z;

    Vector3 Normalize() const
    {
        float length = std::sqrt(1 + x * x + y * y);
        return Vector3(x / length, y / length, z / length);
    }
};

// (todo) 01.8: Declare an struct with the vertex format


TerrainApplication::TerrainApplication(unsigned int x, unsigned int y)
    : Application(1024, 1024, "Terrain demo"), m_gridX(x), m_gridY(y), m_shaderProgram(0)
{
}

void TerrainApplication::Initialize()
{
    Application::Initialize();

    // Build shaders and store in m_shaderProgram
    BuildShaders();

    std::vector<Vector3> vertices;
    std::vector<unsigned int> indices;

    std::vector<Vector2> uvs;


	float scale_x = 1.0f / m_gridX;
	float scale_y = 1.0f / m_gridY;

    float frequency = 5.0f; 
    float amplitude = 0.1f;
    float lacunarity = 2.0f; 
    int octaves = 4;

    // create verticies and uvs
    for (int y = 0; y <= m_gridY; y++)
    {
        for (int x = 0; x <= m_gridX; x++)
        {
            float x_scaled = (x * scale_x) - 0.5f;
            float y_scaled = (y * scale_y) - 0.5f;
            float z_scaled = 0.0f;

            float freq = frequency;
            float amp = amplitude;
            for (int i = 0; i < octaves; i++)
            {
                z_scaled += stb_perlin_noise3(x_scaled * freq, y_scaled * freq, 0.0f, 0, 0, 0) * amp;
                freq *= lacunarity;
                amp *= 0.5f; // usually amplitude is halved for each octave
            }

            vertices.push_back(Vector3(x_scaled, y_scaled, z_scaled));
            uvs.push_back(Vector2(x , y));
			
        }
    }

    for (int y = 0; y < m_gridY; y++)
    {
        for (int x = 0; x < m_gridX; x++)
        {
            unsigned int topLeft = y * (m_gridX + 1) + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (y + 1) * (m_gridX + 1) + x;
            unsigned int bottomRight = bottomLeft + 1;

            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);

            // Second triangle
            indices.push_back(topLeft);
            indices.push_back(bottomRight);
            indices.push_back(topRight);
        }
    }


	VAO.Bind();

	VBO.Bind();
    size_t verticiesSize = vertices.size() * sizeof(Vector3);
    size_t uvsSize = uvs.size() * sizeof(Vector2);
	VBO.AllocateData(verticiesSize + uvsSize); // Allocate memory for vertices and uvs

	VBO.UpdateData(std::span<const Vector3>(vertices), 0); // Update with vertex data
	VBO.UpdateData(std::span<const Vector2>(uvs), verticiesSize); // Update with uv data, offset by vertexDataSize

	// Set position attribute
    VertexAttribute position(Data::Type::Float, 3);
    VAO.SetAttribute(0, position, 0);

	// Set UV attribute
    VertexAttribute uv(Data::Type::Float, 2);
    VAO.SetAttribute(1, uv, verticiesSize);

    // Initialize EBO
    EBO.Bind();
    EBO.AllocateData<unsigned int>(indices);
    
	// Unbind VAO, VBO and EBO
	VAO.Unbind();
	VBO.Unbind();
    EBO.Unbind(); 

}

void TerrainApplication::Update()
{
    Application::Update();

    UpdateOutputMode();
}

void TerrainApplication::Render()
{
    Application::Render();

    // Clear color and depth
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);

    // Set shader to be used
    glUseProgram(m_shaderProgram);

	VAO.Bind();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Draw in wireframe polygons
	glDrawElements(GL_TRIANGLES, (m_gridX * m_gridY * 6), GL_UNSIGNED_INT, 0);
	VAO.Unbind();

}

void TerrainApplication::Cleanup()
{
    Application::Cleanup();
}


void TerrainApplication::BuildShaders()
{
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "layout (location = 2) in vec3 aColor;\n"
        "layout (location = 3) in vec3 aNormal;\n"
        "uniform mat4 Matrix = mat4(1);\n"
        "out vec2 texCoord;\n"
        "out vec3 color;\n"
        "out vec3 normal;\n"
        "void main()\n"
        "{\n"
        "   texCoord = aTexCoord;\n"
        "   color = aColor;\n"
        "   normal = aNormal;\n"
        "   gl_Position = Matrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "uniform uint Mode = 0u;\n"
        "in vec2 texCoord;\n"
        "in vec3 color;\n"
        "in vec3 normal;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   switch (Mode)\n"
        "   {\n"
        "   default:\n"
        "   case 0u:\n"
        "       FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
        "       break;\n"
        "   case 1u:\n"
        "       FragColor = vec4(fract(texCoord), 0.0f, 1.0f);\n"
        "       break;\n"
        "   case 2u:\n"
        "       FragColor = vec4(color, 1.0f);\n"
        "       break;\n"
        "   case 3u:\n"
        "       FragColor = vec4(normalize(normal), 1.0f);\n"
        "       break;\n"
        "   case 4u:\n"
        "       FragColor = vec4(color * max(dot(normalize(normal), normalize(vec3(1,0,1))), 0.2f), 1.0f);\n"
        "       break;\n"
        "   }\n"
        "}\n\0";

    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    m_shaderProgram = shaderProgram;
}

void TerrainApplication::UpdateOutputMode()
{
    for (int i = 0; i <= 4; ++i)
    {
        if (GetMainWindow().IsKeyPressed(GLFW_KEY_0 + i))
        {
            int modeLocation = glGetUniformLocation(m_shaderProgram, "Mode");
            glUseProgram(m_shaderProgram);
            glUniform1ui(modeLocation, i);
            break;
        }
    }
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_TAB))
    {
        const float projMatrix[16] = { 0, -1.294f, -0.721f, -0.707f, 1.83f, 0, 0, 0, 0, 1.294f, -0.721f, -0.707f, 0, 0, 1.24f, 1.414f };
        int matrixLocation = glGetUniformLocation(m_shaderProgram, "Matrix");
        glUseProgram(m_shaderProgram);
        glUniformMatrix4fv(matrixLocation, 1, false, projMatrix);
    }
}
