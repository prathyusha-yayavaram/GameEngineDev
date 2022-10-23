#pragma once
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/Math/Matrix4x4.h"

namespace PE {
	namespace Components {
		struct PhysicsComponent {
			const char* m_name;
			Vector3 m_startPos;
			Vector3 m_endPos;
			Vector3 m_finalPos;
			Handle m_collider;
			bool m_isGravityEffects;
			float m_downVelocity;
			Matrix4x4 m_base;
			Matrix4x4 m_oldBase;
		};
	}
}