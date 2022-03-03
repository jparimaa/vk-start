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
        glm::vec2 uv;
        glm::vec3 normal;
    };

    struct Material
    {
        int baseColor;
        int metallicRoughnessImage;
        int normalImage;
        int emissiveImage;
        int occlusionImage;
    };

    struct Image
    {
        int width;
        int height;
        int components;
        int bitsPerChannel;
        std::vector<unsigned char> data;
    };

    using Index = uint32_t;

    Model(const std::string& filename);

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Material> materials;
    std::vector<Image> images;
};
