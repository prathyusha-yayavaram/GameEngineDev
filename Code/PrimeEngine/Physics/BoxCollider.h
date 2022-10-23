#ifndef __PYENGINE_2_0_BOXCOLLIDER_H__
#define __PYENGINE_2_0_BOXCOLLIDER_H__

#include "../Events/Component.h"

#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "SphereCollider.h"
#include "PhysicsManager.h"
#include "PhysicsData.h"

namespace PE {
	namespace Components {
		struct BoxCollider : Component {
			PE_DECLARE_CLASS(BoxCollider);
			BoxCollider(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself);
			
			Array<Vector3> m_aabb;
			
			Matrix4x4 m_world;

			PhysicsData BoxCollider::checkWithSphereCollider(Component *sphereCollider);
			PhysicsData BoxCollider::checkWithBoxCollider(Component *boxCollider);
		};
	}
}
#endif

