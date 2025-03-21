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

    glm::mat3 TransformObject::normalMatrix() {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        const glm::vec3 invScale = 1.0f / scale;
    
        return glm::mat3{
            {
                invScale.x * (c1 * c3 + s1 * s2 * s3),
                invScale.x * (c2 * s3),
                invScale.x * (c1 * s2 * s3 - c3 * s1),
            },
            {
                invScale.y * (c3 * s1 * s2 - c1 * s3),
                invScale.y * (c2 * c3),
                invScale.y * (c1 * c3 * s2 + s1 * s3),
            },
            {
                invScale.z * (c2 * s1),
                invScale.z * (-s2),
                invScale.z * (c1 * c2),
            }
        };
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