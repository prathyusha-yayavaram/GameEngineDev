#ifndef __PYENGINE_2_0_SPHERECOLLIDER_H__
#define __PYENGINE_2_0_SPHERECOLLIDER_H__

#include "../Events/Component.h"

#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "BoxCollider.h"
#include "PhysicsManager.h"
#include "PhysicsData.h"

namespace PE {
	namespace Components {
		struct SphereCollider : Component {
			PE_DECLARE_CLASS(SphereCollider);
			SphereCollider(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself);
			float m_radius;
			Vector3 m_center;
			Vector3 m_centerLS;
			Matrix4x4 m_world;

			PhysicsData SphereCollider::checkWithSphereCollider(Component *sphereCollider);
			PhysicsData SphereCollider::checkWithBoxCollider(Component *boxCollider);
		};
	}
}
#endif