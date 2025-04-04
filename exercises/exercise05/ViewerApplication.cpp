#include "ViewerApplication.h"

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/asset/ModelLoader.h>
#include <ituGL/asset/Texture2DLoader.h>
#include <ituGL/shader/Material.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <imgui.h>

ViewerApplication::ViewerApplication()
    : Application(1024, 1024, "Viewer demo")
    , m_cameraPosition(0, 30, 30)
    , m_cameraTranslationSpeed(20.0f)
    , m_cameraRotationSpeed(0.5f)
    , m_cameraEnabled(false)
    , m_cameraEnablePressed(false)
    , m_mousePosition(GetMainWindow().GetMousePosition(true))
	, m_ambientColor(0.25f)
	, m_lightColor(1.0f)
	, m_lightIntensity(1.0f)
	, m_lightPosition(-10.0f, 20.0f, 10.0f)
	, m_specularExponent(100.0f)
{
}

void ViewerApplication::Initialize()
{
    Application::Initialize();

    // Initialize DearImGUI
    m_imGui.Initialize(GetMainWindow());

    InitializeModel();
    InitializeCamera();
    InitializeLights();

    DeviceGL& device = GetDevice();
    device.EnableFeature(GL_DEPTH_TEST);
    device.SetVSyncEnabled(true);
}

void ViewerApplication::Update()
{
    Application::Update();

    // Update camera controller
    UpdateCamera();
}

void ViewerApplication::Render()
{
    Application::Render();

    // Clear color and depth
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);
	RenderGUI();
    m_model.Draw();
}

void ViewerApplication::Cleanup()
{
    // Cleanup DearImGUI
    m_imGui.Cleanup();

    Application::Cleanup();
}

void ViewerApplication::InitializeModel()
{
    // Load and build shader
    //Shader vertexShader = ShaderLoader::Load(Shader::VertexShader, "shaders/unlit.vert");
    //Shader fragmentShader = ShaderLoader::Load(Shader::FragmentShader, "shaders/unlit.frag");

    Shader vertexShader = ShaderLoader::Load(Shader::VertexShader, "shaders/blinn-phong.vert");
    Shader fragmentShader = ShaderLoader::Load(Shader::FragmentShader, "shaders/blinn-phong.frag");
    std::shared_ptr<ShaderProgram> shaderProgram = std::make_shared<ShaderProgram>();
    shaderProgram->Build(vertexShader, fragmentShader);

    // Filter out uniforms that are not material properties
    ShaderUniformCollection::NameSet filteredUniforms;
    filteredUniforms.insert("WorldMatrix");
    filteredUniforms.insert("ViewProjMatrix");
    filteredUniforms.insert("AmbientColor");
    filteredUniforms.insert("LightColor");
    filteredUniforms.insert("LightPosition");



    // Create reference material
    std::shared_ptr<Material> material = std::make_shared<Material>(shaderProgram, filteredUniforms);
    material->SetUniformValue("Color", glm::vec4(1.0f));
	material->SetUniformValue("AmbientReflection", 1.0f);
	material->SetUniformValue("DiffuseReflection", 1.0f);
	material->SetUniformValue("SpecularReflection", 1.0f); 
	material->SetUniformValue("SpecularExponent", m_specularExponent);

    // Setup function

    ShaderProgram::Location worldMatrixLocation = shaderProgram->GetUniformLocation("WorldMatrix");
    ShaderProgram::Location viewProjMatrixLocation = shaderProgram->GetUniformLocation("ViewProjMatrix");
	ShaderProgram::Location ambientColorLocation = shaderProgram->GetUniformLocation("AmbientColor");
	ShaderProgram::Location lightColorLocation = shaderProgram->GetUniformLocation("LightColor");
	ShaderProgram::Location lightPositionLocation = shaderProgram->GetUniformLocation("LightPosition");
    ShaderProgram::Location cameraPositionLocation = shaderProgram->GetUniformLocation("CameraPosition");

    material->SetShaderSetupFunction([=](ShaderProgram& shaderProgram) 
        {
            //Set camera and light uniforms
            shaderProgram.SetUniform(worldMatrixLocation, glm::scale(glm::vec3(0.1f))); 
            shaderProgram.SetUniform(viewProjMatrixLocation, m_camera.GetViewProjectionMatrix());
			shaderProgram.SetUniform(ambientColorLocation, m_ambientColor);
			shaderProgram.SetUniform(lightColorLocation, m_lightColor * m_lightIntensity);
			shaderProgram.SetUniform(lightPositionLocation, m_lightPosition);
            shaderProgram.SetUniform(cameraPositionLocation, m_cameraPosition);


        });

    // Configure model loader
    ModelLoader loader(material);
    loader.SetMaterialAttribute(VertexAttribute::Semantic::Position, "VertexPosition");
    loader.SetMaterialAttribute(VertexAttribute::Semantic::Normal, "VertexNormal");
    loader.SetMaterialAttribute(VertexAttribute::Semantic::TexCoord0, "VertexTexCoord");

	loader.SetCreateMaterials(true);
   
    // Load model
    m_model = loader.Load("models/mill/Mill.obj");
    
    // (todo) 05.1: Load and set textures

	// Configure texture loader
    Texture2DLoader textureLoader(TextureObject::Format::FormatRGBA, TextureObject::InternalFormat::InternalFormatRGBA8);
    textureLoader.SetGenerateMipmap(true);
	textureLoader.SetFlipVertical(true);

	// Load textures
	std::shared_ptr<Texture2DObject> millTexture = textureLoader.LoadShared("models/mill/MillCat_color.jpg");
    std::shared_ptr<Texture2DObject> groundColorTexture = textureLoader.LoadShared("models/mill/Ground_color.jpg");
    std::shared_ptr<Texture2DObject> groundShadowTexture = textureLoader.LoadShared("models/mill/Ground_shadow.jpg");

	m_model.GetMaterial(0).SetUniformValue("ColorTexture", groundShadowTexture); 
	m_model.GetMaterial(1).SetUniformValue("ColorTexture", groundColorTexture); 
	m_model.GetMaterial(2).SetUniformValue("ColorTexture", millTexture);



}

void ViewerApplication::InitializeCamera()
{
    // Set view matrix, from the camera position looking to the origin
    m_camera.SetViewMatrix(m_cameraPosition, glm::vec3(0.0f));

    // Set perspective matrix
    float aspectRatio = GetMainWindow().GetAspectRatio();
    m_camera.SetPerspectiveProjectionMatrix(1.0f, aspectRatio, 0.1f, 1000.0f);
}

void ViewerApplication::InitializeLights()
{
    // (todo) 05.X: Initialize light variables

}

void ViewerApplication::RenderGUI()
{
    m_imGui.BeginFrame();

    //Add debug controls for light properties
	if (ImGui::CollapsingHeader("Light properties"))
	{
		ImGui::ColorEdit3("Ambient color", &m_ambientColor[0]);
		ImGui::ColorEdit3("Light color", &m_lightColor[0]);
		ImGui::SliderFloat("Light intensity", &m_lightIntensity, 0.0f, 10.0f);
		ImGui::SliderFloat3("Light position", &m_lightPosition[0], -100.0f, 100.0f);
        ImGui::SliderFloat("Specular Exponent", &m_specularExponent, 0.0f, 200.0f);
	}


    m_imGui.EndFrame();
}

void ViewerApplication::UpdateCamera()
{
    Window& window = GetMainWindow();

    // Update if camera is enabled (controlled by SPACE key)
    {
        bool enablePressed = window.IsKeyPressed(GLFW_KEY_SPACE);
        if (enablePressed && !m_cameraEnablePressed)
        {
            m_cameraEnabled = !m_cameraEnabled;

            window.SetMouseVisible(!m_cameraEnabled);
            m_mousePosition = window.GetMousePosition(true);
        }
        m_cameraEnablePressed = enablePressed;
    }

    if (!m_cameraEnabled)
        return;

    glm::mat4 viewTransposedMatrix = glm::transpose(m_camera.GetViewMatrix());
    glm::vec3 viewRight = viewTransposedMatrix[0];
    glm::vec3 viewForward = -viewTransposedMatrix[2];

    // Update camera translation
    {
        glm::vec2 inputTranslation(0.0f);

        if (window.IsKeyPressed(GLFW_KEY_A))
            inputTranslation.x = -1.0f;
        else if (window.IsKeyPressed(GLFW_KEY_D))
            inputTranslation.x = 1.0f;

        if (window.IsKeyPressed(GLFW_KEY_W))
            inputTranslation.y = 1.0f;
        else if (window.IsKeyPressed(GLFW_KEY_S))
            inputTranslation.y = -1.0f;

        inputTranslation *= m_cameraTranslationSpeed;
        inputTranslation *= GetDeltaTime();

        // Double speed if SHIFT is pressed
        if (window.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
            inputTranslation *= 2.0f;

        m_cameraPosition += inputTranslation.x * viewRight + inputTranslation.y * viewForward;
    }

    // Update camera rotation
   {
        glm::vec2 mousePosition = window.GetMousePosition(true);
        glm::vec2 deltaMousePosition = mousePosition - m_mousePosition;
        m_mousePosition = mousePosition;

        glm::vec3 inputRotation(-deltaMousePosition.x, deltaMousePosition.y, 0.0f);

        inputRotation *= m_cameraRotationSpeed;

        viewForward = glm::rotate(inputRotation.x, glm::vec3(0,1,0)) * glm::rotate(inputRotation.y, glm::vec3(viewRight)) * glm::vec4(viewForward, 0);
    }

   // Update view matrix
   m_camera.SetViewMatrix(m_cameraPosition, m_cameraPosition + viewForward);
}
