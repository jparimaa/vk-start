#include "Model.hpp"
#include "Utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#include <tiny_gltf.h>

#include <string>
#include <cstring>

Model::Model(const std::string& filename)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string errorMessage;
    std::string warningMessage;

    const std::string filepath = c_modelsFolder + filename;
    const bool modelLoaded = loader.LoadBinaryFromFile(&model, &errorMessage, &warningMessage, filepath);

    if (!warningMessage.empty())
    {
        LOGW(warningMessage.c_str());
        abort();
    }

    if (!errorMessage.empty())
    {
        LOGE(errorMessage.c_str());
        abort();
    }

    CHECK(modelLoaded);
    printf("Loaded model %s\n", filepath.c_str());

    CHECK(!model.meshes.empty());
    const int positionIndex = model.meshes[0].primitives[0].attributes["POSITION"];
    const tinygltf::Accessor& positionAccessor = model.accessors[positionIndex];
    const tinygltf::BufferView& positionBufferView = model.bufferViews[positionAccessor.bufferView];
    const tinygltf::Buffer& positionBuffer = model.buffers[positionBufferView.buffer];
    vertices.resize(positionAccessor.count);

    const size_t positionOffset = positionBufferView.byteOffset + positionAccessor.byteOffset;
    const unsigned char* positionBufferPtr = &positionBuffer.data[positionOffset];
    for (size_t i = 0; i < positionAccessor.count; ++i)
    {
        CHECK(positionBufferPtr < &positionBuffer.data[positionOffset + positionBufferView.byteLength]);
        std::memcpy(&vertices[i].position, positionBufferPtr, sizeof(glm::vec3));
        positionBufferPtr += sizeof(glm::vec3) + positionBufferView.byteStride;
    }

    const tinygltf::Accessor& indicesAccessor = model.accessors[model.meshes[0].primitives[0].indices];
    const tinygltf::BufferView& indexBufferView = model.bufferViews[indicesAccessor.bufferView];
    const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];
    indices.resize(indicesAccessor.count);

    const size_t indexOffset = indexBufferView.byteOffset + indicesAccessor.byteOffset;
    const unsigned char* indexBufferPtr = &indexBuffer.data[indexOffset];
    unsigned short indexValue = 0;
    for (size_t i = 0; i < indicesAccessor.count; ++i)
    {
        CHECK(indexBufferPtr < &indexBuffer.data[indexOffset + indexBufferView.byteLength]);
        std::memcpy(&indexValue, indexBufferPtr, sizeof(unsigned short));
        indices[i] = indexValue;
        indexBufferPtr += indexBufferView.byteStride + sizeof(unsigned short);
    }
}

Model::~Model()
{
}
