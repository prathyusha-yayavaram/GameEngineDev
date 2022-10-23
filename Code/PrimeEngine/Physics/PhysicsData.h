#ifndef __PYENGINE_2_0_PHYSICSDATA_H__
#define __PYENGINE_2_0_PHYSICSDATA_H__

#include "PrimeEngine/Math/Vector3.h"

namespace PE {
	namespace Components {
		struct PhysicsData {
			bool intersects;
			float distance;
			Vector3 unitSlide;
		};
	}
}
#endif