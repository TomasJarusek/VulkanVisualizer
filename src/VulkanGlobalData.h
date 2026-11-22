
#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

// Verify that padding is 16B and the struct size is a multiple ob 16B.
// No vec3s. Bundle stuff into vec4s if possible.
struct VulkanGlobalData
{
    // Camera.
    glm::mat4 cameraView_;
    glm::mat4 cameraProjection_;
    glm::vec4 cameraPosition_;

    // Light source.
    glm::vec4 lightSourcePosition_;
    glm::vec4 lightSourceColor_;

    // Phong reflection model.
    glm::vec4 phongParameters_;

    auto ToTuple() const
    {
        // Returns provided values as a tuple of references.
        return std::tie(this->cameraView_, this->cameraProjection_, this->cameraPosition_, this->lightSourcePosition_, this->lightSourceColor_, this->phongParameters_);
    }
};
