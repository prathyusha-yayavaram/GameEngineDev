#include "BoxCollider.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PhysicsManager.h"
#include "SphereCollider.h"

namespace PE {
	namespace Components {
		PE_IMPLEMENT_CLASS1(BoxCollider, Component);
		BoxCollider::BoxCollider(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself)
			: Component(context, arena, hMyself)
		{
			m_aabb.reset(10);
		}

		PhysicsData BoxCollider::checkWithSphereCollider(Component *sphereCollider) {
			return PhysicsData();
		}

		PhysicsData BoxCollider::checkWithBoxCollider(Component *boxCollider) {
			return PhysicsData();
		}
	}
}