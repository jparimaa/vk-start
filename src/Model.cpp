#include "Model.hpp"
#include "Utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

#include <string>

Model::Model()
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string errorMessage;
    std::string warningMessage;

    const std::string filename = c_modelsFolder + "BoxTextured.glb";
    const bool modelLoaded = loader.LoadBinaryFromFile(&model, &errorMessage, &warningMessage, filename);

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
    printf("Loaded model %s\n", filename.c_str());
}

Model::~Model()
{
}
