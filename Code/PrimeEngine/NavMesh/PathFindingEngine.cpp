
#include "PathFindingEngine.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Scene/MeshInstance.h"
#include "PrimeEngine/Scene/DefaultAnimationSM.h"
#include "PrimeEngine/Scene/SkeletonInstance.h"
#include "PrimeEngine/Scene/SceneNode.h"

namespace PE {
namespace Events {
	PE_IMPLEMENT_CLASS1(PathFindingEngine_Event_PATH_POSITIONS_FOUND, Event);
}

namespace Components {

PE_IMPLEMENT_CLASS1(PathFindingEngine, Component);


// Constructor -------------------------------------------------------------
PathFindingEngine::PathFindingEngine(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) :  Component(context, arena, hMyself), m_inheritPositionOnly(false)
{
}

void PathFindingEngine::addDefaultComponents()
{
	Component::addDefaultComponents();
	
	
	// Data components

	// event handlers
	PE_REGISTER_EVENT_HANDLER(Events::Event_FIND_PATH, PathFindingEngine::do_PATH_FINDING_CALCULATION);

}

void PathFindingEngine::do_PATH_FINDING_CALCULATION(Events::Event *pEvt)
{
	Events::Event_FIND_PATH*pRealEvent = (Events::Event_FIND_PATH*)(pEvt);
	Matrix4x4 &m = m_base;
	m.setPos(m.getPos() + pRealEvent->m_targetPos);
}

}; // namespace Components
}; // namespace PE
