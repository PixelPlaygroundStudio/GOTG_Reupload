#pragma once

#include "Vector2i.h"
#include "Vector2f.h"

#include "Vector3i.h"
#include "Vector3f.h"

#include "Vector4i.h"
#include "Vector4f.h"

#include "Rectangle.h"
#include <glm/glm.hpp>

#include <glm/gtx/rotate_vector.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <LinearMath\btVector3.h>
#include <Wiwa/ecs/components/Transform3D.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp> // for glm::angle()

namespace Wiwa
{
	namespace Math
	{
		/// <summary>
		/// Returns the angle formed by a vector
		/// </summary>
		/// <param name="vector"></param>
		/// <returns></returns>
		inline float AngleFromVec2(const glm::vec2& vector)
		{
			return glm::degrees(glm::atan(vector.x, vector.y));
		}
		inline glm::vec3 Vec3FromAngle(const float angle)
		{
			return glm::normalize(glm::vec3(glm::sin(glm::radians(angle)), 0.f, glm::cos(glm::radians(angle))));
		}

		inline float RandomFloat(float a, float b) {
			float random = ((float)rand()) / (float)RAND_MAX;
			float diff = b - a;
			float r = random * diff;
			return a + r;
		}
		inline btVector3 ToBulletVector3(const glm::vec3 &vector)
		{
			return btVector3(vector.x, vector.y, vector.z);
		}

		inline glm::vec3 Forward(const glm::mat4& transform)
		{
			if (glm::determinant(transform) == 0.0f)
			{
				// Handle the case where the matrix is not invertible
				// Return a default forward vector or an appropriate value for your application
				return glm::vec3(0.0f, 0.0f, 1.0f);
			}
			const glm::mat4 inverted = glm::inverse(transform);
			return normalize(glm::vec3(inverted[2]));
		}

		inline glm::vec3 Up(const glm::mat4& transform)
		{
			if (glm::determinant(transform) == 0.0f)
			{
				// Handle the case where the matrix is not invertible
				// Return a default forward vector or an appropriate value for your application
				return glm::vec3(0.0f, 1.0f, 0.0f);
			}
			glm::vec3 upVector = glm::vec3(transform[1]);
			return normalize(upVector);
		}

		inline float Distance(const glm::vec3 &a, const glm::vec3 &b)
		{
			float diff_x = a.x - b.x;
			float diff_y = a.y - b.y;
			float diff_z = a.z - b.z;
			return glm::sqrt(diff_x * diff_x + diff_y * diff_y + diff_z * diff_z);
		}

		inline glm::vec3 PointAlongDirection(const glm::vec3 &origin, const glm::vec3 &direction, float distance)
		{
			return origin + glm::normalize(direction) * distance;
		}

		inline float RadToDeg(float rad)
		{
			return rad * 180.0f / PI_F;
		}
		inline float DegToRad(float deg)
		{
			return deg * PI_F / 180.0f;
		}
		inline glm::vec3 CalculateForward(const Transform3D* t3d)
		{
			glm::vec3 rotrad = glm::radians(t3d->rotation);

			glm::vec3 forward;

			forward.x = glm::cos(rotrad.x) * glm::sin(rotrad.y);
			forward.y = -glm::sin(rotrad.x);
			forward.z = glm::cos(rotrad.x) * glm::cos(rotrad.y);

			forward = glm::degrees(forward);

			return glm::normalize(forward);
		}

		inline void ScreenPosToWorldRay(
			int mouseX, int mouseY,			   // Mouse position, in pixels, from bottom-left corner of the window
			int screenWidth, int screenHeight, // Window size, in pixels
			glm::mat4 ViewMatrix,			   // Camera position and orientation
			glm::mat4 ProjectionMatrix,		   // Camera parameters (ratio, field of view, near and far planes)
			glm::vec3 &out_origin,			   // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
			glm::vec3 &out_direction		   // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
		)
		{
			// The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
			glm::vec4 lRayStart_NDC(
				((float)mouseX / (float)screenWidth - 0.5f) * 2.0f,	 // [0,1024] -> [-1,1]
				((float)mouseY / (float)screenHeight - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
				-1.0,												 // The near plane maps to Z=-1 in Normalized Device Coordinates
				1.0f);
			glm::vec4 lRayEnd_NDC(
				((float)mouseX / (float)screenWidth - 0.5f) * 2.0f,
				((float)mouseY / (float)screenHeight - 0.5f) * 2.0f,
				0.0,
				1.0f);

			// The Projection matrix goes from Camera Space to NDC.
			// So inverse(ProjectionMatrix) goes from NDC to Camera Space.
			glm::mat4 InverseProjectionMatrix = glm::inverse(ProjectionMatrix);

			// The View Matrix goes from World Space to Camera Space.
			// So inverse(ViewMatrix) goes from Camera Space to World Space.
			glm::mat4 InverseViewMatrix = glm::inverse(ViewMatrix);

			glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;
			lRayStart_camera /= lRayStart_camera.w;
			glm::vec4 lRayStart_world = InverseViewMatrix * lRayStart_camera;
			lRayStart_world /= lRayStart_world.w;
			glm::vec4 lRayEnd_camera = InverseProjectionMatrix * lRayEnd_NDC;
			lRayEnd_camera /= lRayEnd_camera.w;
			glm::vec4 lRayEnd_world = InverseViewMatrix * lRayEnd_camera;
			lRayEnd_world /= lRayEnd_world.w;

			// Faster way (just one inverse)
			// glm::mat4 M = glm::inverse(ProjectionMatrix * ViewMatrix);
			// glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
			// glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;

			glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
			lRayDir_world = glm::normalize(lRayDir_world);

			out_origin = glm::vec3(lRayStart_world);
			out_direction = glm::normalize(lRayDir_world);
		}
		inline bool TestRayOBBIntersection(
			glm::vec3 ray_origin,		 // Ray origin, in world space
			glm::vec3 ray_direction,	 // Ray direction (NOT target position!), in world space. Must be normalize()'d.
			glm::vec3 aabb_min,			 // Minimum X,Y,Z coords of the mesh when not transformed at all.
			glm::vec3 aabb_max,			 // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
			glm::mat4 ModelMatrix,		 // Transformation applied to the mesh (which will thus be also applied to its bounding box)
			float &intersection_distance // Output : distance between ray_origin and the intersection with the OBB
		)
		{

			// Intersection method from Real-Time Rendering and Essential Mathematics for Games

			float tMin = 0.0f;
			float tMax = 100000.0f;
			glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

			glm::vec3 delta = OBBposition_worldspace - ray_origin;

			// Test intersection with the 2 planes perpendicular to the OBB's X axis
			{
				glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
				float e = glm::dot(xaxis, delta);
				float f = glm::dot(ray_direction, xaxis);

				if (fabs(f) > 0.001f)
				{ // Standard case

					float t1 = (e + aabb_min.x) / f; // Intersection with the "left" plane
					float t2 = (e + aabb_max.x) / f; // Intersection with the "right" plane
					// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

					// We want t1 to represent the nearest intersection,
					// so if it's not the case, invert t1 and t2
					if (t1 > t2)
					{
						float w = t1;
						t1 = t2;
						t2 = w; // swap t1 and t2
					}

					// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
					if (t2 < tMax)
						tMax = t2;
					// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
					if (t1 > tMin)
						tMin = t1;

					// And here's the trick :
					// If "far" is closer than "near", then there is NO intersection.
					// See the images in the tutorials for the visual explanation.
					if (tMax < tMin)
						return false;
				}
				else
				{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
					if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f)
						return false;
				}
			}

			// Test intersection with the 2 planes perpendicular to the OBB's Y axis
			// Exactly the same thing than above.
			{
				glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
				float e = glm::dot(yaxis, delta);
				float f = glm::dot(ray_direction, yaxis);

				if (fabs(f) > 0.001f)
				{

					float t1 = (e + aabb_min.y) / f;
					float t2 = (e + aabb_max.y) / f;

					if (t1 > t2)
					{
						float w = t1;
						t1 = t2;
						t2 = w;
					}

					if (t2 < tMax)
						tMax = t2;
					if (t1 > tMin)
						tMin = t1;
					if (tMin > tMax)
						return false;
				}
				else
				{
					if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f)
						return false;
				}
			}

			// Test intersection with the 2 planes perpendicular to the OBB's Z axis
			// Exactly the same thing than above.
			{
				glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
				float e = glm::dot(zaxis, delta);
				float f = glm::dot(ray_direction, zaxis);

				if (fabs(f) > 0.001f)
				{

					float t1 = (e + aabb_min.z) / f;
					float t2 = (e + aabb_max.z) / f;

					if (t1 > t2)
					{
						float w = t1;
						t1 = t2;
						t2 = w;
					}

					if (t2 < tMax)
						tMax = t2;
					if (t1 > tMin)
						tMin = t1;
					if (tMin > tMax)
						return false;
				}
				else
				{
					if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f)
						return false;
				}
			}

			intersection_distance = tMin;
			return true;
		}
		inline bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &translation, glm::vec3 &rotation, glm::vec3 &scale)
		{
			// From glm::decompose in matrix_decompose.inl

			using namespace glm;
			using T = float;

			mat4 LocalMatrix(transform);

			// Normalize the matrix.
			if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
				return false;

			// First, isolate perspective.  This is the messiest.
			if (
				epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
				epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
				epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
			{
				// Clear the perspective partition
				LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
				LocalMatrix[3][3] = static_cast<T>(1);
			}

			// Next take care of translation (easy).
			translation = vec3(LocalMatrix[3]);
			LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

			vec3 Row[3]{}, Pdum3;

			// Now get scale and shear.
			for (length_t i = 0; i < 3; ++i)
				for (length_t j = 0; j < 3; ++j)
					Row[i][j] = LocalMatrix[i][j];

			// Compute X scale factor and normalize first row.
			scale.x = length(Row[0]);
			Row[0] = detail::scale(Row[0], static_cast<T>(1));
			scale.y = length(Row[1]);
			Row[1] = detail::scale(Row[1], static_cast<T>(1));
			scale.z = length(Row[2]);
			Row[2] = detail::scale(Row[2], static_cast<T>(1));

			// At this point, the matrix (in rows[]) is orthonormal.
			// Check for a coordinate system flip.  If the determinant
			// is -1, then negate the matrix and the scaling factors.

			Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
			if (dot(Row[0], Pdum3) < 0)
			{
				for (length_t i = 0; i < 3; i++)
				{
					scale[i] *= static_cast<T>(-1);
					Row[i] *= static_cast<T>(-1);
				}
			}

			rotation.y = asin(-Row[0][2]);
			if (cos(rotation.y) != 0)
			{
				rotation.x = atan2(Row[1][2], Row[2][2]);
				rotation.z = atan2(Row[0][1], Row[0][0]);
			}
			else
			{
				rotation.x = atan2(-Row[2][0], Row[1][1]);
				rotation.z = 0;
			}

			return true;
		}

		inline float RandomRange(float value01, float value02)
		{
			if (((value01 - value02) == 0) || ((value02 - value01) == 0))
				return 0;

			if (value01 > value02)
			{

				float i = value01;
				value01 = value02;
				value02 = i;
			}

			float ret = value01 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (value02 - value01)));

			return ret;
		}
		inline int RandomRange(int value01, int value02)
		{

			if (((value01 - value02) == 0) || ((value02 - value01) == 0))
				return 0;

			if (value01 > value02)
			{

				int i = value01;
				value01 = value02;
				value02 = i;
			}

			int subs = value02 - value01;

			int ret = value01 + (rand()) / (RAND_MAX / (value02 - value01));

			return ret;
		}

		inline float Repeat(float t, float length)
		{
			return std::clamp(t - std::floor(t / length) * length, 0.0f, length);
		}
		// Perform Slerp interpolation between two angles in degrees
		inline float Slerp(float startAngle, float endAngle, float t) {
			// Convert start and end angles to quaternions
			glm::quat startQuat = glm::angleAxis(glm::radians(startAngle), glm::vec3(0, 1, 0));
			glm::quat endQuat = glm::angleAxis(glm::radians(endAngle), glm::vec3(0, 1, 0));

			// Perform Slerp interpolation between the quaternions
			glm::quat resultQuat = glm::slerp(startQuat, endQuat, t);

			// Convert the resulting quaternion back to an angle in degrees
			float resultAngle = glm::degrees(glm::angle(resultQuat));

			return resultAngle;
		}
		inline float LerpAngle(float a, float b, float t)
		{
			float delta = Repeat((b - a), 360);
			if (delta > 180)
				delta -= 360;
			return a + delta * std::clamp(t, 0.0f, 1.0f);
		}

		inline glm::vec3 GetWorldPosition(const glm::mat4 &mat)
		{
			return glm::vec3(mat[3]);
		}

		inline glm::vec3 CalculateForward(const glm::vec3 &rotation)
		{
			glm::vec3 rotrad = glm::radians(rotation);

			glm::vec3 forward;

			forward.x = glm::cos(rotrad.x) * glm::sin(rotrad.y);
			forward.y = -glm::sin(rotrad.x);
			forward.z = glm::cos(rotrad.x) * glm::cos(rotrad.y);

			forward = glm::degrees(forward);

			return glm::normalize(forward);
		}

		inline void GetRightAndLeftRotatedFromForward(const glm::vec3 &forward, glm::vec3 &right, glm::vec3 &left, float angle)
		{
			glm::vec3 up(0.0f, 1.0f, 0.0f);
			float radiansAngleRight = glm::radians(angle);
			float radiansAngleLeft = glm::radians(-angle);

			right = glm::rotate(forward, radiansAngleRight, up);
			left = glm::rotate(forward, radiansAngleLeft, up);
		}

		inline void GetRightRotatedFromForward(const glm::vec3& forward, glm::vec3& right, float angle)
		{
			glm::vec3 up(0.0f, 1.0f, 0.0f);
			float radiansAngleRight = glm::radians(angle);

			right = glm::rotate(forward, radiansAngleRight, up);
		}

		inline void GetLeftRotatedFromForward(const glm::vec3& forward, glm::vec3& left, float angle)
		{
			glm::vec3 up(0.0f, 1.0f, 0.0f);
			float radiansAngleLeft = glm::radians(-angle);

			left = glm::rotate(forward, radiansAngleLeft, up);
		}

		inline bool IsPointNear(const glm::vec3 &point_1, const glm::vec3 &point_2, float threshold)
		{
			return glm::distance2(point_1, point_2) < threshold * threshold;
		}


		inline float DistanceV3(const glm::vec3& point_1, const glm::vec3& point_2)
		{
			return glm::distance2(point_1, point_2);
		}
		
		inline glm::vec3 GetRoationBetweenTwoPoints(const glm::vec3 &point_1, const glm::vec3 &point_2)
		{
			glm::vec3 p1Forward = glm::normalize(point_1);
			glm::vec3 p2Forward = glm::normalize(point_2);
			glm::quat rotation = glm::rotation(p1Forward, p2Forward);
			return glm::degrees(glm::eulerAngles(rotation));
		}

		inline glm::vec3 InterpolateTwoV3(glm::vec3 start, glm::vec3 end, float t)
		{
			return glm::vec3
			{
		       start.x + (end.x - start.x) * t,
		       start.y + (end.y - start.y) * t,
		       start.z + (end.z - start.z) * t
			};
		}
	}

}