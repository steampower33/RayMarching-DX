#pragma once

struct Vertex
{
    float x, y, z;
};

Vertex quad_vertices[] =
{
    // Triangle 1
    { -1.0f,  1.0f, 0.0f }, // Top-Left
    {  1.0f,  1.0f, 0.0f }, // Top-Right
    { -1.0f, -1.0f, 0.0f }, // Bottom-Left

    // Triangle 2
    { -1.0f, -1.0f, 0.0f }, // Bottom-Left
    {  1.0f,  1.0f, 0.0f }, // Top-Right
    {  1.0f, -1.0f, 0.0f }, // Bottom-Right
};