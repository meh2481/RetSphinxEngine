#pragma once

#define GLM_META_PROG_HELPERS

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace glmx {

inline glm::vec3 orthogonalize(const glm::vec3 ref, const glm::vec3 toOrtho, glm::vec3 *third)
{
	*third = glm::normalize(glm::cross(ref, toOrtho));
	return glm::cross(*third, ref);
}

template<typename T>
inline T to(const T self, const T other)
{
    return other - self;
}

template<typename T>
inline float lensqr(const T v)
{
    return glm::dot(v, v);
}



} // end namespace glmx

