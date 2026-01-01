#pragma once
#include "Imports/common.h"  // Assumes raylib includes

struct AxisAngle {
    raylib::Vector3 axis;
    float angle;  // In degrees
};
namespace MathUtils {
/*
* @brief Convert Euler angles (in degrees) to Euler angles in radians.
* @param eulerAngles The Euler angles as a Vector3 (pitch, yaw, roll) in degrees.
* @returns vector of Euler angles in radians.
*/
inline raylib::Vector3 EulerToRadians(raylib::Vector3 eulerAngles) {
const float degToRad = PI / 180.0f;
return raylib::Vector3(
eulerAngles.x * degToRad,
eulerAngles.y * degToRad,
eulerAngles.z * degToRad
);
}

   /*
* @brief Convert Euler angles (in degrees) to Axis-Angle representation.
* @param eulerAngles The Euler angles as a Vector3 (pitch, yaw, roll) in degrees.
* @returns AxisAngle struct containing the axis (Vector3) and angle (float in radians).
   */
inline AxisAngle EulerToAxisAngle(raylib::Vector3 eulerAngles) {
eulerAngles = EulerToRadians(eulerAngles); // Convert to radians

// Create quaternion from Euler angles
raylib::Quaternion q = raylib::Quaternion::FromEuler(eulerAngles.x, eulerAngles.y, eulerAngles.z);

// Convert quaterinon to axis-angle
raylib::Vector3 axis;
float angle;
q.ToAxisAngle(&axis, &angle);

angle *= 180.0f / PI;  // Convert to degrees

return AxisAngle{ axis, angle };
}

/*
* @brief Apply modulo operation to each component of a Vector3.
* @param vec The input Vector3.
* @param mod The modulo value.
* @returns A Vector3 where each component is the result of the modulo operation.
*/
inline raylib::Vector3 VectorModulo(raylib::Vector3 vec, float mod) {
		return raylib::Vector3(
			fmodf(vec.x, mod),
			fmodf(vec.y, mod),
			fmodf(vec.z, mod)
		);
	}
}