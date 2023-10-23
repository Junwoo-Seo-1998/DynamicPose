#include "VQS.h"

#include <glm/gtx/quaternion.hpp>

#include "Util/Math.h"

glm::vec3 VQS::operator*(const glm::vec3& right)
{
	auto res = q * (s * glm::quat{ 0, right }) * glm::inverse(q);
	return glm::vec3{ res.x,res.y,res.z } + v;
}

VQS VQS::operator*(const VQS& right)
{
	return { *this * right.v, q * right.q, s * right.s };
}

glm::mat4 VQS::toMat() const
{
	return glm::translate(glm::mat4(1.0f), v)
		* glm::toMat4(q)
		* glm::scale(glm::mat4(1.0f), glm::vec3{ s });
}

VQS operator*(const float left, const VQS& right)
{
	return { left * right.v, left * right.q, left * right.s };
}
