#include "PhysicsManager.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"

#include "../Events/Component.h"
#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/Math/Matrix4x4.h"

#include "PrimeEngine/Physics/SphereCollider.h"
#include "PrimeEngine/Physics/BoxCollider.h"
#include "PhysicsData.h"

namespace PE {
	namespace Components {

		PE_IMPLEMENT_CLASS1(PhysicsManager, Component);

		PhysicsManager::PhysicsManager(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself)
			: Component(context, arena, hMyself),
			m_collRefTable()
		{
		}



		int PhysicsManager::addPhysicsComponent(const char *name, Vector3 position, Handle colliders, bool isGravityEffected, Matrix4x4 m_base) {
			//PhysicsComponent physicsComponent = { name, position, position, position, colliders, isGravityEffected, 0, m_base, m_base };
			m_collRefTable.size();
			//m_PhysicsComponents.push_back(physicsComponent);
			UpdateColliders(colliders, m_base);
			return m_PhysicsComponents.size() - 1;
		}

		void PhysicsManager::UpdateColliders() {
			for (size_t i = 0; i < m_PhysicsComponents.size(); i++) {
				if (m_PhysicsComponents[i].m_collider.isValid()) {
					m_PhysicsComponents[i].m_oldBase = m_PhysicsComponents[i].m_base;

					Component* pComponent = m_PhysicsComponents[i].m_collider.getObject<Component>();

					if (pComponent->isInstanceOf<SphereCollider>()) {
						SphereCollider* pSC = (SphereCollider*)pComponent;
						pSC->m_world = m_PhysicsComponents[i].m_base;
						pSC->m_center = m_PhysicsComponents[i].m_base * pSC->m_centerLS;
					}
					else if (pComponent->isInstanceOf<BoxCollider>()) {
						BoxCollider* pBC = (BoxCollider*)pComponent;

						pBC->m_world = m_PhysicsComponents[i].m_base;
						for (int j = 0; j < 8; j++) {
							pBC->m_aabb[j] = m_PhysicsComponents[i].m_base * pBC->m_aabb[j];
						}
					}
				}
			}
		}

		void PhysicsManager::UpdateColliders(Handle colliders, Matrix4x4 m_base) {
				if (colliders.isValid()) {

					Component* pComponent = colliders.getObject<Component>();

					if (pComponent->isInstanceOf<SphereCollider>()) {
						SphereCollider* pSC = (SphereCollider*)pComponent;
						pSC->m_world = m_base;
						pSC->m_center = m_base * pSC->m_centerLS;
					}
					else if (pComponent->isInstanceOf<BoxCollider>()) {
						BoxCollider* pBC = (BoxCollider*)pComponent;

						pBC->m_world = m_base;
						for (int j = 0; j < 8; j++) {
							pBC->m_aabb[j] =  m_base * pBC->m_aabb[j];
						}
					}
				}
		}

		void PhysicsManager::collisionCheck() {
			for (size_t i = 0; i < m_PhysicsComponents.size(); i++) {
				Component* pComponent = m_PhysicsComponents[i].m_collider.getObject<Component>();

				SphereCollider* pSC = NULL;
				BoxCollider* pBC = NULL;
				if (pComponent->isInstanceOf<SphereCollider>()) {
					pSC = (SphereCollider*)pComponent;
				}
				else {
					pBC = (BoxCollider*)pComponent;
				}

				PE::Components::PhysicsData data;
				for (size_t j = 0; j < m_PhysicsComponents.size(); j++) {
					if (m_PhysicsComponents[j].m_collider.isValid()) {
						Component* pmComponent = m_PhysicsComponents[j].m_collider.getObject<Component>();
						if (pSC) {
							if (pmComponent->isInstanceOf<SphereCollider>()) {
								data = pSC->checkWithSphereCollider(pmComponent);
							}
							else {
								data = pSC->checkWithBoxCollider(pmComponent);
							}
						}
						else {
							if (pmComponent->isInstanceOf<SphereCollider>()) {
								data = pBC->checkWithSphereCollider(pmComponent);
							}
							else {
								data = pBC->checkWithBoxCollider(pmComponent);
							}
						}
						if (data.intersects) {
							m_collRefTable.push_back(float(i));
							m_collRefTable.push_back(data.distance);
							m_collRefTable.push_back(data.unitSlide.m_x);
							m_collRefTable.push_back(data.unitSlide.m_y);
							m_collRefTable.push_back(data.unitSlide.m_z);
							m_collRefTable.push_back(float(j));
						}
					}
				}
			}
		}

		void PhysicsManager::resolveCollision() {
			for (size_t i = 0; i < m_collRefTable.size(); i += 6) {
				m_PhysicsComponents[int(m_collRefTable[i + 0])].m_downVelocity = 0;

				Vector3 normal = Vector3(m_collRefTable[i + 2], m_collRefTable[i + 3], m_collRefTable[i + 4]);

				float penetrationDistance = fabs(m_collRefTable[i + 1]);
				Vector3 finalPos = m_PhysicsComponents[int(m_collRefTable[i + 0])].m_endPos + (normal * penetrationDistance);
				m_PhysicsComponents[int(m_collRefTable[i + 0])].m_base.setPos(finalPos);

				//m_PhysicsComponents[int(m_collRefTable[i + 0])].m_startPos = 
				m_PhysicsComponents[int(m_collRefTable[i + 0])].m_endPos = finalPos;
			}
		}

		void PhysicsManager::simulate(float timestep) {



		}
	}
}
