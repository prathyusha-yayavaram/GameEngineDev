#ifndef __PYENGINE_2_0_PHYSICSMANAGER_H__
#define __PYENGINE_2_0_PHYSICSMANAGER_H__

#include "../Events/Component.h"
#include "PhysicsComponent.h"
#include <vector>

namespace PE {
	namespace Components {
		struct PhysicsManager : public Component {

			PE_DECLARE_CLASS(PhysicsManager);
			PhysicsManager(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself);


			int PhysicsManager::addPhysicsComponent(const char* name, Vector3 position, Handle colliders, bool isGravityEffected, Matrix4x4 m_base);

			void PhysicsManager::UpdateColliders(Handle colliders, Matrix4x4 m_base);
			void PhysicsManager::UpdateColliders();
			void PhysicsManager::resolveCollision();
			void PhysicsManager::collisionCheck();
			void PhysicsManager::simulate(float timestep);

			std::vector<PhysicsComponent> m_PhysicsComponents;
			std::vector<float> m_collRefTable;
		};
	};
};
#endif