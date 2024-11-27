#include "./engine/2d/gameSprite.h"

namespace JCAT {
    using id_t = unsigned int;

    glm::mat3 TransformSprite::transformationMatrix() {
        const float c = glm::cos(rotation);
        const float s = glm::sin(rotation);

        glm::mat3 transformationMatrix = {
            {
                scale.x * c, scale.x * s, 0.0f,
            },
            {
                scale.y * -s, scale.y * c, 0.0f,
            },
            {
                translation.x, translation.y, 1.0f
            }
        };

        return transformationMatrix;
    }

    GameSprite GameSprite::createGameSprite() {
        static id_t currentId = 0;
        return GameSprite{ currentId++ };
    }

    GameSprite::GameSprite(id_t sprId) : id{ sprId } {}

    id_t GameSprite::getSpriteId() {
        return id;
    }
}