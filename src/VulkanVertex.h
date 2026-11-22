
#pragma once

#include <tuple>
#include <format>

#include "VulkanHelperFunctions.h"


struct VulkanVertex
{
    glm::vec3 position_;
    glm::vec4 color_;
    // This offers a substantial reduction to vertex size (40B -> 28B) which in turn
    // results in a proportional speedup of vertex transfer to GPU.
    // However, because the scene is fully reconstructed each frame, the conversion
    // from float to uint8_t on a CPU completely drowns out any benefit of the transfer speedup.
    // The only solution would be change the whole API to accept uint8_ts, but I want
    // floats to be used across the board from a consistency perspective.
    // glm::u8vec4 color_;
    glm::vec3 normal_;

    static vk::VertexInputBindingDescription GetVertexInputBindingDescription(uint32_t binding = 0)
    {
        return vk::VertexInputBindingDescription(binding, sizeof(VulkanVertex), vk::VertexInputRate::eVertex);
    }

    static std::array<vk::VertexInputAttributeDescription, 3> GetVertexInputAttributeDescription(uint32_t binding = 0)
    {
        uint32_t location = 0;

        return
        {{
            vk::VertexInputAttributeDescription(location++, binding, vk::Format::eR32G32B32Sfloat, offsetof(VulkanVertex, position_)),
            vk::VertexInputAttributeDescription(location++, binding, vk::Format::eR32G32B32A32Sfloat, offsetof(VulkanVertex, color_)),
            vk::VertexInputAttributeDescription(location++, binding, vk::Format::eR32G32B32Sfloat, offsetof(VulkanVertex, normal_))
        }};
    }

    auto ToTuple() const
    {
        // Returns provided values as a tuple of references.
        return std::tie(this->position_, this->color_, this->normal_);
    }

    std::size_t Hash() const
    {
        std::size_t seed = 0;

        // auto combineHashes = [&seed]<typename... Ts>(const Ts&... tupleValues)
        // {
        //     (CombineHashes(seed, tupleValues), ...);
        // };

        // Applies lambda to the tuple.
        // std::apply(combineHashes, this->ToTuple());
        

        // Measurably faster, but still requires glm::hash. (#define GLM_ENABLE_EXPERIMENTAL, #include <glm/gtx/hash.hpp>)
        // CombineHashes(seed, this->position_);
        // CombineHashes(seed, this->color_);
        // CombineHashes(seed, this->normal_);

        // Even faster.
        CombineHashes(seed, this->position_.x);
        CombineHashes(seed, this->position_.y);
        CombineHashes(seed, this->position_.z);
        CombineHashes(seed, this->color_.r);
        CombineHashes(seed, this->color_.g);
        CombineHashes(seed, this->color_.b);
        CombineHashes(seed, this->color_.a);
        CombineHashes(seed, this->normal_.x);
        CombineHashes(seed, this->normal_.y);
        CombineHashes(seed, this->normal_.z);

        return seed;
    }

    // Equality operator for std::unordered map.
    bool operator==(const VulkanVertex& vertex) const
    {
        // return this->ToTuple() == vertex.ToTuple();


        // Measurably faster.
        return this->position_.x == vertex.position_.x && this->position_.y == vertex.position_.y && this->position_.z == vertex.position_.z && this->color_.x == vertex.color_.x && this->color_.y == vertex.color_.y && this->color_.z == vertex.color_.z;
    }
};

// Prepares map for another use.
template<typename... Ts>
inline void InitVerticesMap(std::unordered_map<Ts...>& uniqueVertices, bool resetReservedMemory = false)
{
    uniqueVertices.clear();

    if (resetReservedMemory == true)
    {
        uniqueVertices.rehash(0);
    }
}

// Adds new vertex into a vertex/index vectors.
// InitVerticesMap() must be called beforehand.
template<typename... Ts>
inline void AddVertexUnique(std::unordered_map<Ts...>& uniqueVertices, std::vector<VulkanVertex>& vertices, std::vector<uint32_t>& indexes, const VulkanVertex& vertex)
{
    // If vertex does not exist, it is inserted into the map. Index starts at 0 and is incremented with each unique vertex.
    // If vertex exists, nothing happens.
    auto [it, inserted] = uniqueVertices.try_emplace(vertex, static_cast<uint32_t>(vertices.size()));

    if (inserted == true)
    {
        // Vertex isn't in the map.

        vertices.push_back(vertex);
    }

    // Index is always pushed into the vector. Either index of an existing vertex or the index belonging to the newly added vertex.
    indexes.emplace_back(it->second);
}

// Adds new vertex into a vertex vector.
inline void AddVertex(std::vector<VulkanVertex>& vertices, const glm::vec3& pos, const glm::vec4& color, const glm::vec3& normal)
{
    vertices.emplace_back(pos, color, normal);
}

inline void AddVertex(std::vector<VulkanVertex>& vertices, const VulkanVertex& vertex)
{
    vertices.push_back(vertex);
}

// Fills vertex/index vectors at once.
// InitVerticesMap() must be called beforehand.
// duplicatedVertices must be different from vertices.
template<typename... Ts>
inline void DeduplicateVertices(std::unordered_map<Ts...>& uniqueVertices, std::span<const VulkanVertex> duplicatedVertices, std::vector<VulkanVertex>& vertices, std::vector<uint32_t>& indexes)
{
    // 75% reduction won't probably be achieved, which will lead to some reallocations, but the reserve should't be too conservative either.
    vertices.reserve(duplicatedVertices.size()/4);
    // There will be as many indexes as duplicated vertices.
    indexes.reserve(duplicatedVertices.size());

    for (const VulkanVertex& vertex: duplicatedVertices)
    {
        AddVertexUnique(uniqueVertices, vertex, vertices, indexes);
    }
}
