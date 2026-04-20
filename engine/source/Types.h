#ifndef TYPES_H
#define TYPES_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace ENG
{

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

using usize = size_t;

using str = std::string;

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;

using ivec2 = glm::ivec2;
using ivec3 = glm::ivec3;
using ivec4 = glm::ivec4;

using uvec2 = glm::uvec2;
using uvec3 = glm::uvec3;
using uvec4 = glm::uvec4;

using mat3 = glm::mat3;
using mat4 = glm::mat4;

using quat = glm::quat;

using glm::angleAxis;
using glm::cross;
using glm::degrees;
using glm::distance;
using glm::dot;
using glm::inverse;
using glm::length;
using glm::lookAt;
using glm::mat4_cast;
using glm::normalize;
using glm::ortho;
using glm::perspective;
using glm::quat_cast;
using glm::radians;
using glm::rotate;
using glm::scale;
using glm::translate;
using glm::transpose;
using glm::value_ptr;

}  // namespace ENG

#endif  // TYPES_H
