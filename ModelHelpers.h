#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/scene.h>

namespace ModelHelpers {
    // Função para converter uma matriz do Assimp para o formato glm
    glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from) {
        return glm::mat4(
            from.a1, from.b1, from.c1, from.d1,
            from.a2, from.b2, from.c2, from.d2,
            from.a3, from.b3, from.c3, from.d3,
            from.a4, from.b4, from.c4, from.d4
        );
    }
}
