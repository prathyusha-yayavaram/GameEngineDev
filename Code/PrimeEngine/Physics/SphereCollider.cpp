#include "SphereCollider.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PhysicsManager.h"
#include "BoxCollider.h"

namespace PE {
	namespace Components {
		PE_IMPLEMENT_CLASS1(SphereCollider, Component);
		SphereCollider::SphereCollider(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself)
			: Component(context, arena, hMyself)
		{
		}

		PhysicsData SphereCollider::checkWithSphereCollider(Component *component) {

			return PhysicsData();
		}

		PhysicsData SphereCollider::checkWithBoxCollider(Component *component) {
			BoxCollider* boxCollider = (BoxCollider*)component;
			Array<Vector2> minMax;
			minMax.add({ boxCollider->m_aabb[0].m_x,  boxCollider->m_aabb[6].m_x });
			minMax.add({ boxCollider->m_aabb[0].m_x,  boxCollider->m_aabb[6].m_x });
			minMax.add({ boxCollider->m_aabb[0].m_x,  boxCollider->m_aabb[6].m_x });
			float sqDist = 0.0f;
			for (int i = 0; i < 3; i++) {
				// for each axis count any excess distance outside box extents
				float v = m_center.m_x;
				if (v < minMax[i].m_x) sqDist += (minMax[i].m_x - v) * (minMax[i].m_x - v);
				if (v > minMax[i].m_y) sqDist += (v - minMax[i].m_y) * (v - minMax[i].m_y);
			}
			Vector3 q;
			// For each coordinate axis, if the point coordinate value is
			// outside box, clamp it to the box, else keep it as is
			for (int i = 0; i < 3; i++) {
				float v = m_center.m_x;
				if (v < minMax[i].m_x) v = minMax[i].m_x; // v = max( v, b.min[i] )
				if (v > minMax[i].m_y) v = minMax[i].m_y; // v = min( v, b.max[i] )
				if (i == 0) {
					q.m_x = v;
				}
				else if (i == 1) {
					q.m_y = v;
				}
				else {
					q.m_z = v;
				}
			}
			q.normalize();
			PhysicsData data;
			if (sqDist <= m_radius * m_radius) {
				data.intersects = true;
				data.distance = m_radius - sqrt(sqDist);
				data.unitSlide = q;
			}
			else {
				//No collision
				data.intersects = false;
			}
			return PhysicsData();
		}
	}
}
