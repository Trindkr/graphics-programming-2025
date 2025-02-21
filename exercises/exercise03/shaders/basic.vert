#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

// (todo) 03.X: Add the out variables here
out vec3 Normal;
out vec3 Position;

uniform mat4 WorldMatrix = mat4(1.0f);
uniform mat4 ViewProjMatrix = mat4(1.0f);
// (todo) 03.X: Add the uniforms here

void main()
{
    gl_Position = ViewProjMatrix * WorldMatrix * vec4(VertexPosition, 1.0);
    Normal = (WorldMatrix * vec4(VertexNormal, 0.0)).xyz;

    //Position = VertexPosition; // Calculate the position in local space
    //Position = (WorldMatrix * vec4(VertexPosition, 1.0)).xyz; // Calculate the position in world space
    Position = (ViewProjMatrix * WorldMatrix * vec4(VertexPosition, 1.0)).xyz; // Calculate the position in view space
}
