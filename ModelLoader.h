#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <string>

class ModelLoader {
public:
    ModelLoader();
    ~ModelLoader();

    void loadModel(const std::string& path);

private:
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
};

#endif // MODELLOADER_H
