#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <memory>

#include "./engine/3D/model3d.h"
#include <gtc/matrix_transform.hpp>

namespace JCAT {
    struct TransformObject {
        glm::vec3 translation{};
        glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
        glm::vec3 rotation{};

        glm::mat4 transformationMatrix();
    };

    class GameObject {
        public:
            using id = unsigned int;

            static GameObject createGameObject();

            GameObject(const GameObject&) = delete;
            GameObject& operator=(const GameObject&) = delete;
            GameObject(GameObject&&) = default;
            GameObject& operator=(GameObject&&) = default;

            id getObjectId();

            std::shared_ptr<JCATModel3D> model3D;
            glm::vec3 color{};
            TransformObject transform{};
        private:
            GameObject(id objId);

            id id;
    };
}

#endif