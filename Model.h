#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "AssimpGLMHelpers.h"
#include "Camera.h"
#include "Mesh.h"

struct LoadedTexture {
    std::string path;
    Texture texture;
};

class Model {
public:
    Model(const std::string& file);
    void Draw(Shader& shader, Camera& camera);
    std::vector<unsigned char> getData();

private:
    std::string file;
    const aiScene* scene; // Guarda a cena carregada pelo Assimp
    std::vector<Mesh> meshes;
    std::vector<glm::mat4> matricesMeshes;
    std::vector<LoadedTexture> loadedTextures; // Guarda texturas já carregadas

    void loadModel(const std::string& file);
    std::vector<Texture> getTextures();
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& texType, const std::string& directory);
    void processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& transform);
    std::vector<glm::vec3> groupFloatsVec3(const std::vector<float>& floatVec);
    std::vector<glm::vec2> groupFloatsVec2(const std::vector<float>& floatVec);
    std::vector<Vertex> assembleVertices(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texUVs);
};

#endif
