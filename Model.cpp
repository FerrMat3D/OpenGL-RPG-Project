#include "Model.h"

Model::Model(const std::string& file) : file(file) {
    loadModel(file);
}

void Model::Draw(Shader& shader, Camera& camera) {
    for (size_t i = 0; i < meshes.size(); ++i) {
        meshes[i].Draw(shader, camera, matricesMeshes[i]);
    }
}

std::vector<unsigned char> Model::getData() {
    std::ifstream fileStream(file, std::ios::binary | std::ios::ate);
    if (!fileStream.is_open()) {
        std::cerr << "Could not open file: " << file << std::endl;
        return std::vector<unsigned char>();
    }
    std::streamsize size = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(size);
    if (!fileStream.read(reinterpret_cast<char*>(buffer.data()), size)) {
        std::cerr << "Failed to read file: " << file << std::endl;
        return std::vector<unsigned char>();
    }
    return buffer;
}

void Model::loadModel(const std::string& file) {
    Assimp::Importer importer;
    scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    processNode(scene->mRootNode, scene, glm::mat4(1.0f));
}

void Model::processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform) {
    glm::mat4 nodeTransform = parentTransform * AssimpGLMHelpers::ConvertMatrixToGLMFormat(node->mTransformation);
    for (size_t i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, nodeTransform));
        matricesMeshes.push_back(nodeTransform);
    }
    for (size_t i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene, nodeTransform);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& transform) {
    std::vector<float> posVec, normalVec, texVec;
    for (size_t i = 0; i < mesh->mNumVertices; ++i) {
        posVec.insert(posVec.end(), { mesh->mVertices[i].x, -mesh->mVertices[i].y, mesh->mVertices[i].z });
        normalVec.insert(normalVec.end(), { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z });
        if (mesh->mTextureCoords[0]) {
            texVec.insert(texVec.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
        }
        else {
            texVec.insert(texVec.end(), { 0.0f, 0.0f });
        }
    }

    std::vector<Vertex> vertices = assembleVertices(groupFloatsVec3(posVec), groupFloatsVec3(normalVec), groupFloatsVec2(texVec));
    std::vector<GLuint> indices;
    for (size_t i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }
    std::vector<Texture> textures = getTextures();
    return Mesh(vertices, indices, textures);
}


std::vector<Texture> Model::getTextures() {
    std::vector<Texture> textures;

    // Obtém o diretório do arquivo do modelo
    std::string fileStr = std::string(file);
    std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

    // Percorre todos os materiais do modelo
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        aiMaterial* material = scene->mMaterials[i];

        // Carrega a textura de difusão (baseColor)
        std::vector<Texture> diffuseTextures = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse", fileDirectory);
        textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

        // Carrega a textura especular (metallicRoughness)
        std::vector<Texture> specularTextures = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular", fileDirectory);
        textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());
    }

    return textures;
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& texType, const std::string& directory) {
    std::vector<Texture> textures;

    // Percorre todas as texturas do tipo especificado
    for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
        aiString texturePath;
        mat->GetTexture(type, i, &texturePath);
        std::string texPath = directory + std::string(texturePath.C_Str());

        // Verifica se a textura já foi carregada anteriormente
        bool skip = false;
        for (const auto& loadedTexture : loadedTextures) {
            if (loadedTexture.path == texPath) {
                textures.push_back(loadedTexture.texture);
                skip = true;
                break;
            }
        }

        // Se a textura já foi carregada, pule esta iteração
        if (!skip) {
            // Cria uma nova textura e a carrega
            Texture texture = Texture(texPath.c_str(), texType.c_str(), loadedTextures.size());
            textures.push_back(texture);
            // Armazena a textura carregada junto com o caminho do arquivo para evitar recarregamento
            loadedTextures.push_back({ texPath, texture });
        }
    }

    return textures;
}


std::vector<glm::vec2> Model::groupFloatsVec2(const std::vector<float>& floatVec) {
    std::vector<glm::vec2> vectors;
    for (size_t i = 0; i < floatVec.size(); i += 2) {
        vectors.push_back(glm::vec2(floatVec[i], floatVec[i + 1]));
    }
    return vectors;
}

std::vector<glm::vec3> Model::groupFloatsVec3(const std::vector<float>& floatVec) {
    std::vector<glm::vec3> vectors;
    for (size_t i = 0; i < floatVec.size(); i += 3) {
        vectors.push_back(glm::vec3(floatVec[i], floatVec[i + 1], floatVec[i + 2]));
    }
    return vectors;
}

std::vector<Vertex> Model::assembleVertices(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texUVs) {
    std::vector<Vertex> vertices;
    for (size_t i = 0; i < positions.size(); ++i) {
        // Verifique se há coordenadas de textura suficientes para cada vértice
        if (i < texUVs.size()) {
            vertices.push_back(Vertex{ positions[i], normals[i], glm::vec3(1.0f, 1.0f, 1.0f), texUVs[i] });
        }
        else {
            // Se não houver coordenadas de textura suficientes, adicione coordenadas padrão
            vertices.push_back(Vertex{ positions[i], normals[i], glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f) });
        }
    }
    return vertices;
}
