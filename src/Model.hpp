#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

class Model final
{
public:
    struct Vertex
    {
        glm::vec3 position;
        //glm::vec2 uv;
        //glm::vec3 normal;
    };

    using Index = uint32_t;

    Model(const std::string& filename);
    ~Model();

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};
