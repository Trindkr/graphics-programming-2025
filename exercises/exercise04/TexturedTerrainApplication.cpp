#include "TexturedTerrainApplication.h"

#include <ituGL/geometry/VertexFormat.h>
#include <ituGL/texture/Texture2DObject.h>

#include <glm/gtx/transform.hpp>  // for matrix transformations

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cmath>
#include <iostream>
#include <numbers>  // for PI constant

TexturedTerrainApplication::TexturedTerrainApplication()
	: Application(1024, 1024, "Textures demo")
	, m_gridX(128), m_gridY(128)
	, m_vertexShaderLoader(Shader::Type::VertexShader)
	, m_fragmentShaderLoader(Shader::Type::FragmentShader)
{

}

void TexturedTerrainApplication::Initialize()
{
	Application::Initialize();

	// Build textures and keep them in a list
	InitializeTextures();

	// Build materials and keep them in a list
	InitializeMaterials();

	// Build meshes and keep them in a list
	InitializeMeshes();

	//Enable depth test
	GetDevice().EnableFeature(GL_DEPTH_TEST);

	//Enable wireframe
	//GetDevice().SetWireframeEnabled(true);
}

void TexturedTerrainApplication::Update()
{
	Application::Update();

	const Window& window = GetMainWindow();

	glm::vec2 mousePosition = window.GetMousePosition(true) * 10.f;


	// Camera movement speed
	float cameraSpeed = 50.0f * GetDeltaTime();


	if (window.IsKeyPressed(GLFW_KEY_W)) // Very wonky camera movement
	{
		m_cameraPosition += cameraSpeed * glm::vec3(0.0f, 0.0f, -1.0f);
	}
	if (window.IsKeyPressed(GLFW_KEY_S))
	{
		m_cameraPosition += cameraSpeed * glm::vec3(0.0f, 0.0f, 1.0f);
	}
	if (window.IsKeyPressed(GLFW_KEY_A))
	{
		m_cameraPosition += cameraSpeed * glm::vec3(-1.0f, 0.0f, 0.0f);
	}
	if (window.IsKeyPressed(GLFW_KEY_D))
	{
		m_cameraPosition += cameraSpeed * glm::vec3(1.0f, 0.0f, 0.0f);
	}

	m_camera.SetViewMatrix(m_cameraPosition, glm::vec3(mousePosition, 0.0f));

	int width, height;
	window.GetDimensions(width, height);
	float aspectRatio = static_cast<float>(width) / height;
	m_camera.SetPerspectiveProjectionMatrix(1.0f, aspectRatio, 0.1f, 100.0f);




}


void TexturedTerrainApplication::Render()
{
	Application::Render();

	// Clear color and depth
	GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);


	glm::mat4 scale = glm::scale(glm::vec3(10.0f));

	glm::mat4 t1 = glm::translate(glm::vec3(0, 0, -1.0f));
	glm::mat4 t2 = glm::translate(glm::vec3(-1.0f, 0, -1.0f));
	glm::mat4 t3 = glm::translate(glm::vec3(-1.0f, 0, 0));
	glm::mat4 t4 = glm::translate(glm::vec3(0, 0, 0));

	float waterLevel = 0.35f;

	glm::mat4 t1_1 = glm::translate(glm::vec3(0, waterLevel, -1.0f));
	glm::mat4 t2_1 = glm::translate(glm::vec3(-1.0f, waterLevel, -1.0f));
	glm::mat4 t3_1 = glm::translate(glm::vec3(-1.0f, waterLevel, 0));
	glm::mat4 t4_1 = glm::translate(glm::vec3(0, waterLevel, 0));

	// Draw terrain planes
	DrawObject(m_terrainPatch, *m_terrainMaterials[1], scale * t1);
	DrawObject(m_terrainPatch, *m_terrainMaterials[3], scale * t2);
	DrawObject(m_terrainPatch, *m_terrainMaterials[2], scale * t3);
	DrawObject(m_terrainPatch, *m_terrainMaterials[0], scale * t4);


	//Add water planes
	DrawObject(m_terrainPatch, *m_waterMaterial, scale * t1_1);
	DrawObject(m_terrainPatch, *m_waterMaterial, scale * t2_1);
	DrawObject(m_terrainPatch, *m_waterMaterial, scale * t3_1);
	DrawObject(m_terrainPatch, *m_waterMaterial, scale * t4_1);

}

void TexturedTerrainApplication::InitializeTextures()
{
	m_defaultTexture = CreateDefaultTexture();
	m_grassTexture = LoadTexture("textures/grass.jpg");
	m_dirtTexture = LoadTexture("textures/dirt.png");
	m_rockTexture = LoadTexture("textures/rock.jpg");
	m_snowTexture = LoadTexture("textures/snow.jpg");

	for (int i = 0; i < 4; ++i)
	{
		m_heightMapTextures.push_back(CreateHeightMap(m_gridX, m_gridY, coords[i]));
	}

	m_waterTexture = LoadTexture("textures/water.png");

}

void TexturedTerrainApplication::InitializeMaterials()
{
	// Default shader program
	Shader defaultVS = m_vertexShaderLoader.Load("shaders/default.vert");
	Shader defaultFS = m_fragmentShaderLoader.Load("shaders/default.frag");
	std::shared_ptr<ShaderProgram> defaultShaderProgram = std::make_shared<ShaderProgram>();
	defaultShaderProgram->Build(defaultVS, defaultFS);


	// Default material
	m_defaultMaterial = std::make_shared<Material>(defaultShaderProgram);
	m_defaultMaterial->SetUniformValue("Color", glm::vec4(1.0f));


	// Add terrain shader and material here
	Shader terrainVS = m_vertexShaderLoader.Load("shaders/terrain.vert");
	Shader terrainFS = m_fragmentShaderLoader.Load("shaders/terrain.frag");
	std::shared_ptr<ShaderProgram> terrainShaderProgram = std::make_shared<ShaderProgram>();
	terrainShaderProgram->Build(terrainVS, terrainFS);




	// Terrain material

	m_terrainMaterials.resize(4);

	m_terrainMaterials[0] = std::make_shared<Material>(terrainShaderProgram);
	m_terrainMaterials[0]->SetUniformValue("GrassTexture", m_grassTexture);
	m_terrainMaterials[0]->SetUniformValue("DirtTexture", m_dirtTexture);
	m_terrainMaterials[0]->SetUniformValue("RockTexture", m_rockTexture);
	m_terrainMaterials[0]->SetUniformValue("SnowTexture", m_snowTexture);
	m_terrainMaterials[0]->SetUniformValue("ColorTextureScale", glm::vec2(0.05f));
	m_terrainMaterials[0]->SetUniformValue("Color", glm::vec4(1.0f));
	m_terrainMaterials[0]->SetUniformValue("GrassHeightRange", glm::vec2(0.3f, 0.4f));
	m_terrainMaterials[0]->SetUniformValue("RockHeightRange", glm::vec2(0.4f, 0.6f));
	m_terrainMaterials[0]->SetUniformValue("SnowHeightRange", glm::vec2(0.6f, 0.8f));


	Material material = Material(*m_terrainMaterials[0].get());

	for (int i = 1; i < 4; ++i)
	{
		m_terrainMaterials[i] = std::make_shared<Material>(material);
		m_terrainMaterials[i]->SetUniformValue("Heightmap", m_heightMapTextures[i]);
	}

	m_terrainMaterials[0]->SetUniformValue("Heightmap", m_heightMapTextures[0]);

	//Add water shader and material here

	// Water shader program
	Shader waterVS = m_vertexShaderLoader.Load("shaders/water.vert");
	Shader waterFS = m_fragmentShaderLoader.Load("shaders/water.frag");
	std::shared_ptr<ShaderProgram> waterShaderProgram = std::make_shared<ShaderProgram>();
	waterShaderProgram->Build(waterVS, waterFS);

	// Water material
	m_waterMaterial = std::make_shared<Material>(waterShaderProgram);
	m_waterMaterial->SetUniformValue("WaterTexture", m_waterTexture);
	m_waterMaterial->SetUniformValue("ColorTextureScale", glm::vec2(0.05f));
	m_waterMaterial->SetUniformValue("Color", glm::vec4(1.0f, 1.0f, 1.0f, .5f));
	m_waterMaterial->SetBlendEquation(Material::BlendEquation::Add);
	m_waterMaterial->SetBlendParams(Material::BlendParam::SourceAlpha, Material::BlendParam::OneMinusSourceAlpha);
}

void TexturedTerrainApplication::InitializeMeshes()
{
	CreateTerrainMesh(m_terrainPatch, m_gridX, m_gridY);
}

std::shared_ptr<Texture2DObject> TexturedTerrainApplication::CreateDefaultTexture()
{
	std::shared_ptr<Texture2DObject> texture = std::make_shared<Texture2DObject>();

	int width = 4;
	int height = 4;
	std::vector<float> pixels;
	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			pixels.push_back(1.0f);
			pixels.push_back(0.0f);
			pixels.push_back(1.0f);
			pixels.push_back(1.0f);
		}
	}

	texture->Bind();
	texture->SetImage<float>(0, width, height, TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA, pixels);
	texture->GenerateMipmap();

	return texture;
}

std::shared_ptr<Texture2DObject> TexturedTerrainApplication::LoadTexture(const char* path)
{
	std::shared_ptr<Texture2DObject> texture = std::make_shared<Texture2DObject>();

	int width = 0;
	int height = 0;
	int components = 0;


	// Load the texture data here
	unsigned char* data = stbi_load(path, &width, &height, &components, 4);

	if (data == nullptr)
	{
		std::cerr << "Failed to load texture: " << path << std::endl;
		return nullptr;
	}

	texture->Bind();
	texture->SetImage(0, width, height, TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA, std::span<const unsigned char>(data, width * height * 4));


	// Generate mipmaps
	texture->GenerateMipmap();

	// Release texture data
	stbi_image_free(data);

	return texture;
}

std::shared_ptr<Texture2DObject> TexturedTerrainApplication::CreateHeightMap(unsigned int width, unsigned int height, glm::ivec2 coords)
{
	std::shared_ptr<Texture2DObject> heightmap = std::make_shared<Texture2DObject>();

	float minHeight = std::numeric_limits<float>::max();
	float maxHeight = std::numeric_limits<float>::lowest();

	std::vector<float> pixels;
	for (unsigned int j = 0; j < height; ++j)
	{
		for (unsigned int i = 0; i < width; ++i)
		{
			// Add pixel data
			float x = i / static_cast<float>(width - 1);
			float y = j / static_cast<float>(height - 1);
			float z = stb_perlin_ridge_noise3(x + coords.x, y + coords.y, 0.0f, 2.0f, 0.5f, 1.0f, 6);

			//float z = std::sin(0.1f * i) * 0.1f;

			if (z < minHeight) minHeight = z;
			if (z > maxHeight) maxHeight = z;

			pixels.push_back(z);

		}
	}

	heightmap->Bind();
	heightmap->SetImage<float>(0, width, height, TextureObject::FormatR, TextureObject::InternalFormatR16F, pixels);
	heightmap->GenerateMipmap();

	return heightmap;
}

void TexturedTerrainApplication::DrawObject(const Mesh& mesh, Material& material, const glm::mat4& worldMatrix)
{
	material.Use();

	ShaderProgram& shaderProgram = *material.GetShaderProgram();
	ShaderProgram::Location locationWorldMatrix = shaderProgram.GetUniformLocation("WorldMatrix");
	material.GetShaderProgram()->SetUniform(locationWorldMatrix, worldMatrix);
	ShaderProgram::Location locationViewProjMatrix = shaderProgram.GetUniformLocation("ViewProjMatrix");
	material.GetShaderProgram()->SetUniform(locationViewProjMatrix, m_camera.GetViewProjectionMatrix());

	mesh.DrawSubmesh(0);
}

void TexturedTerrainApplication::CreateTerrainMesh(Mesh& mesh, unsigned int gridX, unsigned int gridY)
{
	// Define the vertex structure
	struct Vertex
	{
		Vertex() = default;
		Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2 texCoord)
			: position(position), normal(normal), texCoord(texCoord) {
		}
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
	};

	// Define the vertex format (should match the vertex structure)
	VertexFormat vertexFormat;
	vertexFormat.AddVertexAttribute<float>(3);
	vertexFormat.AddVertexAttribute<float>(3);
	vertexFormat.AddVertexAttribute<float>(2);

	// List of vertices (VBO)
	std::vector<Vertex> vertices;

	// List of indices (EBO)
	std::vector<unsigned int> indices;

	// Grid scale to convert the entire grid to size 1x1
	glm::vec2 scale(1.0f / (gridX - 1), 1.0f / (gridY - 1));

	// Number of columns and rows
	unsigned int columnCount = gridX;
	unsigned int rowCount = gridY;

	// Iterate over each VERTEX
	for (unsigned int j = 0; j < rowCount; ++j)
	{
		for (unsigned int i = 0; i < columnCount; ++i)
		{
			// Vertex data for this vertex only
			glm::vec3 position(i * scale.x, 0.0f, j * scale.y);
			glm::vec3 normal(0.0f, 1.0f, 0.0f);
			glm::vec2 texCoord(i, j);
			vertices.emplace_back(position, normal, texCoord);

			// Index data for quad formed by previous vertices and current
			if (i > 0 && j > 0)
			{
				unsigned int top_right = j * columnCount + i; // Current vertex
				unsigned int top_left = top_right - 1;
				unsigned int bottom_right = top_right - columnCount;
				unsigned int bottom_left = bottom_right - 1;

				//Triangle 1
				indices.push_back(bottom_left);
				indices.push_back(bottom_right);
				indices.push_back(top_left);

				//Triangle 2
				indices.push_back(bottom_right);
				indices.push_back(top_left);
				indices.push_back(top_right);
			}
		}
	}

	mesh.AddSubmesh<Vertex, unsigned int, VertexFormat::LayoutIterator>(Drawcall::Primitive::Triangles, vertices, indices,
		vertexFormat.LayoutBegin(static_cast<int>(vertices.size()), true /* interleaved */), vertexFormat.LayoutEnd());
}