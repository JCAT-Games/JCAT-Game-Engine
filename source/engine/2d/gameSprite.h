#ifndef GAME_SPRITE_H
#define GAME_SPRITE_H

#include <memory>

#include "./engine/2d/model2d.h"
#include <gtc/matrix_transform.hpp>

namespace JCAT {
    struct TransformSprite {
        glm::vec2 translation{};
        glm::vec2 scale{ 1.0f, 1.0f };
        float rotation;

        glm::mat3 transformationMatrix();
    };

    class GameSprite {
        public:
            using id_t = unsigned int;

            static GameSprite createGameSprite();

            GameSprite(const GameSprite&) = delete;
            GameSprite& operator=(const GameSprite&) = delete;
            GameSprite(GameSprite&&) = default;
            GameSprite& operator=(GameSprite&&) = default;

            id_t getSpriteId();

            std::shared_ptr<JCATModel2D> model2D;
            glm::vec3 color{};
            TransformSprite transform{};
        private:
            GameSprite(id_t sprId);

            id_t id;
    };
}

#endif