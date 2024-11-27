#include "./engine/3D/gameObject.h"

namespace JCAT {
    using id_t = unsigned int;

    glm::mat4 TransformObject::transformationMatrix() {
        /*
            How the transformation matrix is structured:

        */

        // Rotation about the Z-axis is done.
        const float cz = glm::cos(rotation.z);
        const float sz = glm::sin(rotation.z);

        // Rotation about the x-axis is done.
        const float cx = glm::cos(rotation.x);
        const float sx = glm::sin(rotation.x);

        // Rotation about the y-axis is done.
        const float cy = glm::cos(rotation.y);
        const float sy = glm::sin(rotation.y);

        // Creating transformation matrix
        glm::mat4 transformationMatrix = {
            {
                scale.x * (cy * cz + sy * sx * sz), scale.x * (cx * sz), scale.x * (cy * sx * sz - cz * sy), 0.0f,
            },
            {
                scale.y * (cz * sy * sx - cy * sz), scale.y * (cx * cz), scale.y * (cy * cz * sx + sy * sz), 0.0f,
            },
            {
                scale.z * (cx * sy), scale.z * (-sx), scale.z * (cy * cx), 0.0f,
            },
            {
                translation.x, translation.y, translation.z, 1.0f,
            }
        };

        return transformationMatrix;
    }

    GameObject GameObject::createGameObject() {
        static id_t currentId = 0;
        return GameObject{ currentId++ };
    }

    GameObject::GameObject(id_t objId) : id{ objId } {}

    id_t GameObject::getObjectId() {
        return id;
    }
};