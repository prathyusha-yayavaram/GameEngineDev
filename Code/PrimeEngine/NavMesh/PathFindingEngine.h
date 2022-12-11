#ifndef __PYENGINE_2_0_PATHFINDINGENGINE_H__
#define __PYENGINE_2_0_PATHFINDINGENGINE_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "../Utils/Array/Array.h"
#include "PrimeEngine/Scene/SceneNode.h"

//#define USE_DRAW_COMPONENT

namespace PE {

namespace Events {
	struct PathFindingEngine_Event_PATH_POSITIONS_FOUND : public Event {
		PE_DECLARE_CLASS(PathFindingEngine_Event_PATH_POSITIONS_FOUND);

		PathFindingEngine_Event_PATH_POSITIONS_FOUND()
		{}
		virtual ~PathFindingEngine_Event_PATH_POSITIONS_FOUND() {}

		Array<Vector3> m_pathPositions;
	};
}

namespace Components {
struct PathFindingEngine : public Component
{
	PE_DECLARE_CLASS(PathFindingEngine);

	// Constructor -------------------------------------------------------------
	PathFindingEngine(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);

	virtual ~PathFindingEngine() {}

	// Component ------------------------------------------------------------

	virtual void addDefaultComponents();
	
	// Individual events -------------------------------------------------------

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PATH_FINDING_CALCULATION);
	virtual void do_PATH_FINDING_CALCULATION(Events::Event *pEvt);


	Handle m_hComponentParent;
	

	Matrix4x4 m_base; // local transform
	Matrix4x4 m_worldTransform; // is calculated bnefore every draw via Events::CALULCATE_TRANSFORMATIONS

	bool m_inheritPositionOnly;

}; // class SceneNode


}; // namespace Components
}; // namespace PE
#endif
