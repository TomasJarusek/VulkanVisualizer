
#pragma once

#include <vulkan/vulkan.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <concepts>
#include <numbers>

#include "VulkanHelperEnums.h"


struct VulkanImageState;
struct VulkanBufferState;


vk::ImageMemoryBarrier2 SetupImageMemoryBarrier(vk::Image image, const VulkanImageState& srcImageState, const VulkanImageState& dstImageState, const vk::ImageSubresourceRange& subresourceRange);

vk::BufferMemoryBarrier2 SetupBufferMemoryBarrier(vk::Buffer buffer, const VulkanBufferState& srcBufferState, const VulkanBufferState& dstBufferState, vk::DeviceSize size, vk::DeviceSize offset);

template<typename T>
void CombineHashes(std::size_t& seed, const T& value) noexcept
{
    using cleanT = std::remove_cvref_t<T>;

    // Generic approach for combining hashes used by a boost library.
    // Golden ratio derived mixing constant (0x9e3779b97f4a7c15) is commonly used by hash-combine algorithms.
    // The shifts and XOR mix the accumulated hash with the new value.
    seed ^= std::hash<cleanT>{}(value)+0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
}

// GLM types.
// Types are listed explicitly because glm::to_string() declares that it accepts any type. 
template<typename T>
concept CanBePrintedByGLM = std::same_as<T, glm::vec2> || std::same_as<T, glm::vec3> || std::same_as<T, glm::vec4> || std::same_as<T, glm::mat3> || std::same_as<T, glm::mat4>;

// Vulkan types.
template<typename T>
concept CanBePrintedByVulkan = requires(T value)
{
    {vk::to_string(value)} -> std::convertible_to<std::string>;
};

// Custom string converters.
template<typename T>
concept CanBePrintedByCustomImplementation = requires(T value)
{
    ToString(value);
};

template<typename T>
std::string GetPrintableValue(const T& value)
{
    if constexpr (CanBePrintedByGLM<T>)
    {
        return glm::to_string(value);
    }
    else if constexpr (CanBePrintedByVulkan<T>)
    {
        return vk::to_string(value);
    }
    else if constexpr (CanBePrintedByCustomImplementation<T>)
    {
        return std::string(ToString(value));
    }
    else
    {
        return std::format("{}", value);
    }
}

template <typename... Ts>
std::string TupleToString(const std::tuple<Ts...>& tuple)
{
    std::string resultString;

    auto printTuple = [&resultString]<typename... Ts>(const Ts&... tupleValues)
    {
        // Two steps to avoid a temporary from being created.
        ((resultString += GetPrintableValue(tupleValues), resultString += ", "), ...);
    };

    std::apply(printTuple, tuple);

    if (resultString.empty() == false)
    {
        resultString.pop_back();
    }

    if (resultString.empty() == false)
    {
        resultString.pop_back();
    }

    return resultString;
}

// Uses Newton's method on x^2 - value = 0.
// Not a bulletproof function by any means.
// Large values will take a long time, intermediate values can overflow, NaN isn't handled, etc.
template<typename T>
constexpr T ConstexprSqrt(T value)
{
    if (value < T(0))
    {
        throw std::domain_error("Exception: Square root can only be applied to non-negative numbers.");
    }
    else if (value == T(0) || value == T(1))
    {
        return value;
    }
    else
    {
        T root = value;
        T previousRoot = 0.0f;

        size_t iterations = 0;
        while (root != previousRoot && iterations < 32)
        {
            previousRoot = root;
            root = T(0.5)*(root + value/root);
            ++iterations;
        }

        return root;
    }
}

// Non-glm vector helpers.
template<typename T, size_t N>
constexpr std::array<T, N> Add(const std::array<T, N>& firstVector, const std::array<T, N>& secondVector)
{
    std::array<T, N> returnVector{};

    for (size_t i = 0; i < N; ++i)
    {
        returnVector[i] = firstVector[i] + secondVector[i];
    }

    return returnVector;
}

template<typename T, size_t N>
constexpr std::array<T, N> Subtract(const std::array<T, N>& firstVector, const std::array<T, N>& secondVector)
{
    std::array<T, N> returnVector{};

    for (size_t i = 0; i < N; ++i)
    {
        returnVector[i] = firstVector[i] - secondVector[i];
    }

    return returnVector;
}

template<typename T, size_t N>
constexpr std::array<T, N> Multiply(const std::array<T, N>& vector, T scalar)
{
    std::array<T, N> returnVector{};

    for (size_t i = 0; i < N; ++i)
    {
        returnVector[i] = vector[i]*scalar;
    }

    return returnVector;
}

template<typename T>
constexpr std::array<T, 3> Cross(const std::array<T, 3>& firstVector, const std::array<T, 3>& secondVector)
{
    return {firstVector[1]*secondVector[2] - firstVector[2]*secondVector[1], firstVector[2]*secondVector[0] - firstVector[0]*secondVector[2], firstVector[0]*secondVector[1] - firstVector[1]*secondVector[0]};
}

template<typename T, size_t N>
constexpr T SquaredLength(const std::array<T, N>& vector)
{
    T squaredLength(0);

    for (size_t i = 0; i < N; ++i)
    {
        squaredLength += vector[i]*vector[i];
    }

    return squaredLength;
}

template<typename T, size_t N>
constexpr std::array<T, N> Normalize(const std::array<T, N>& vector)
{
    std::array<T, N> returnVector{};

    const T length = ConstexprSqrt<T>(SquaredLength(vector));

    for (size_t i = 0; i < N; ++i)
    {
        returnVector[i] = vector[i]/length;
    }

    return returnVector;
}

template<typename T, typename U, size_t N>
constexpr U Convert(const std::array<T, N>& vector)
{
    U returnVector{};

    // glm does not use size_t for indexes.
    using IndexType = std::conditional_t<requires{typename U::length_type;}, typename U::length_type, size_t>;

    for (size_t i = 0; i < N; ++i)
    {
        returnVector[static_cast<IndexType>(i)] = vector[i];
    }

    return returnVector;
}

constexpr float ConstexprFmod(float x, float y)
{
    int quotient = static_cast<int>(x/y);
    return x - (static_cast<float>(quotient)*y);
}

// Expects whole number type in range [0, inf) for the exponent.
template<typename T, typename U>
constexpr T ConstexprPow(T base, U exponent)
{
    static_assert(std::is_integral_v<U>, "Exponent must be a whole number.");

    // Exponentiation by squaring algorithm. 
    T result(1);
    while (exponent > U(0))
    {
        if (exponent % U(2) != U(0))
        {
            result = result*base;
        }

        base = base*base;
        exponent /= U(2);
    }

    return result;
}

// Constexpr sine calculation using Taylor series.
// Expected to be used with input in range of [0, pi/2].
constexpr float ConstexprTaylorSin(float x)
{
    float term = x;
    float sum = x;
    float squaredX = x*x;

    // For float precision, checking up to i = 13 should be plenty.
    for (size_t i = 3; i <= 13; i += 2)
    {
        // Previous term is used to construct next term.
        term = -term*(squaredX/static_cast<float>(i*(i - 1)));

        // Break early if the term can no longer alter a 32-bit float value.
        if ((term >= 0.0f ? term : -term) < 1e-7f)
        {
            break;
        }

        sum += term;
    }

    return sum;
}

// Works on any input by collapsing it to [0, pi/2]. 
constexpr float ConstexprSin(float x)
{
    // x is reduced to [0, 2*pi).
    float reducedX = ConstexprFmod(x, 2.0f*std::numbers::pi_v<float>);
    if (reducedX < 0.0f)
    {
        // Negative value is shifted 2*pi to the the positive direction.
        reducedX += 2.0f*std::numbers::pi_v<float>;
    }

    float sign = 1.0f;

    // Quadrants 3 and 4 are collapsed to 1 and 2.
    if (reducedX > std::numbers::pi_v<float>)
    {
        // Shift towards negative by pi plus flip.
        reducedX -= std::numbers::pi_v<float>;
        sign = -1.0f;
    }

    // Quadrant 2 is collapsed to quadrant 1.
    if (reducedX > std::numbers::pi_v<float>/2.0f)
    {
        // Mirror with respect to pi/2.
        reducedX = std::numbers::pi_v<float> - reducedX;
    }

    // Taylor series is used to calculate final value.
    return sign*ConstexprTaylorSin(reducedX);
}

// Uses shifted sine.
constexpr float ConstexprCos(float x)
{
    return ConstexprSin(x + (std::numbers::pi_v<float>/2.0f));
}
