#include <ituGL/core/DeviceGL.h>
#include <ituGL/application/Window.h>
#include <ituGL/geometry/VertexBufferObject.h>
#include <ituGL/geometry/VertexArrayObject.h>
#include <ituGL/geometry/VertexAttribute.h>
#include <ituGL/geometry/ElementBufferObject.h>

#include <iostream>
#include <sstream> 
#include <vector>
#include <array> 

int buildShaderProgram();
void processInput(GLFWwindow* window);

void rotateVertices(std::array<float, 12>, int numVertices, float angle);
std::pair<std::vector<float>, std::vector<unsigned int>> drawCircle(float radius, int numberOfSides);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

//const float bgColor[4] = { 1.0f, 0.5f, 0.2f, 1.0f }; //change values to change color of background
const Color bgColor = Color(1.0f, 0.5f, 0.2f, 1.0f);
const float polygonColor[4] = { 0.2f, 0.3f, 0.3f, 1.0f }; //change values to change color of polygon
const float length = sqrtf(2.0f) / 2.0f;
const float PI = 3.1415;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    DeviceGL deviceGL;

    // glfw window creation
    // --------------------
    Window window(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL");
    if (!window.IsValid())
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        return -1;
    }

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    deviceGL.SetCurrentWindow(window);
    if (!deviceGL.IsReady())
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    int shaderProgram = buildShaderProgram();

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

	// Square
    std::array<float, 12> vertices{
        -0.5f, -0.5f, 0.0f, // 0 - Bottom Left
         0.5f, -0.5f, 0.0f, // 1 - Bottom Right
         0.5f,  0.5f, 0.0f, // 2 - Top Right
        -0.5f,  0.5f, 0.0f  // 3 - Top Left
    };

    std::array<unsigned int, 6> indices{
        0, 1, 2, // first triangle
        2, 0, 3  // second triangl
    };


    // Circle
    auto [circleVertices, circleIndices] = drawCircle(0.5f, 50);


    //// Using std::array instead of regular arrays makes sure we don't access out of range
    //std::array<float, 3 * (sides + 1)> vertices;
    //std::array<unsigned int, 3 * sides> indices;

    //// first vertex centered at (0, 0, 0)
    //vertices[0] = vertices[1] = vertices[2] = 0.0f;

    //// Loop over 2*PI with N sides
    //float deltaAngle = 2 * pi / sides;
    //for (int i = 0; i < sides; ++i)
    //{
    //    float angle = i * deltaAngle;
    //    vertices[3 * i + 3] = std::sin(angle) * length;
    //    vertices[3 * i + 4] = std::cos(angle) * length;
    //    vertices[3 * i + 5] = 0.0f;

    //    indices[3 * i + 0] = 0;
    //    indices[3 * i + 1] = i + 1;
    //    indices[3 * i + 2] = i + 2;
    //}

    //// Connect last index with vertex 1 to close the circle
    //indices[3 * sides - 1] = 1;

    VertexBufferObject vbo;
    VertexArrayObject vao;
    ElementBufferObject ebo;

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    vao.Bind();

    vbo.Bind();
    int totalVertices = sizeof(vertices) / sizeof(vertices[0]);

    vbo.AllocateData<float>(std::span(circleVertices));
    //vbo.AllocateData<float>(std::span(vertices));

    ebo.Bind();
    int totalIndices = sizeof(indices) / sizeof(indices[0]);
    ebo.AllocateData<unsigned int>(std::span(circleIndices));
    //ebo.AllocateData<unsigned int>(std::span(indices));


    VertexAttribute position(Data::Type::Float, 3);
    vao.SetAttribute(0, position, 0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    VertexBufferObject::Unbind();

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    VertexArrayObject::Unbind();

    // Now we can unbind the EBO as well
    ElementBufferObject::Unbind();

    float time = 0.0f;
    float rotationSpeed = 0.01f;
    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!window.ShouldClose())
    {
        // input
        // -----
        processInput(window.GetInternalWindow());

        // render
        // ------
        deviceGL.Clear(bgColor);

        // draw our first triangle
        glUseProgram(shaderProgram);
        vao.Bind(); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, totalVertices);
        glDrawElements(GL_TRIANGLES, circleIndices.size(), GL_UNSIGNED_INT, 0);

        //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        // VertexArrayObject::Unbind(); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        window.SwapBuffers();
        deviceGL.PollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    // This is now done in the destructor of DeviceGL
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// build the shader program
// ------------------------
int buildShaderProgram() 
{
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    std::stringstream fragmentShaderStream;
    fragmentShaderStream << "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4("
        << polygonColor[0] << ", "
        << polygonColor[1] << ", "
        << polygonColor[2] << ", "
        << polygonColor[3] << ");\n"
        "}\n\0";
    std::string fragmentShaderString = fragmentShaderStream.str();
    const char* fragmentShaderSource = fragmentShaderString.c_str();

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
    return shaderProgram;
}

void rotateVertices(std::array<float, 12> vertices, int numVertices, float angle)
{
    for (size_t i = 0; i < numVertices * 3; i += 3)
    {
        float x = vertices[i];
        float y = vertices[i + 1];
        vertices[i] = x * cos(angle) - y * sin(angle); // x
        vertices[i + 1] = x * sin(angle) + y * cos(angle); // y
        // z remains unchanged 
    }
}

std::pair<std::vector<float>, std::vector<unsigned int>> drawCircle(float radius, int numberOfSides)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    int totalVertices = numberOfSides + 1;
    float doublePi = 2.0f * PI;

    // Add the center of the circle
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);

    // Add the vertices of the circle
    for (int i = 0; i < numberOfSides; ++i)
    {
        float angle = i * doublePi / numberOfSides;
        vertices.push_back(radius * cos(angle)); // x
        vertices.push_back(radius * sin(angle)); // y
        vertices.push_back(0.0f);  // z
    }

    // Add the indices of the circle
    for (int i = 1; i < numberOfSides; ++i)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    indices.push_back(0);
    indices.push_back(numberOfSides);
    indices.push_back(1);

    return { vertices, indices };
}
