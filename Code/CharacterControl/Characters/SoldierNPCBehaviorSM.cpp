#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Scene/DebugRenderer.h"
#include "PrimeEngine/NavMesh/PathFindingEngine.h"

#include "../ClientGameObjectManagerAddon.h"
#include "../CharacterControlContext.h"
#include "SoldierNPCMovementSM.h"
#include "SoldierNPCAnimationSM.h"
#include "SoldierNPCBehaviorSM.h"
#include "SoldierNPC.h"
#include "PrimeEngine/Scene/SceneNode.h"
#include "PrimeEngine/Render/IRenderer.h"
using namespace PE::Components;
using namespace PE::Events;
using namespace CharacterControl::Events;

namespace CharacterControl{

namespace Components{

PE_IMPLEMENT_CLASS1(SoldierNPCBehaviorSM, Component);

SoldierNPCBehaviorSM::SoldierNPCBehaviorSM(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself, PE::Handle hMovementSM, PE::Handle hPathFindingEngine)
: Component(context, arena, hMyself)
, m_hMovementSM(hMovementSM),
m_pathFindingEngine(hPathFindingEngine)
{

}

void SoldierNPCBehaviorSM::start()
{
	if (m_havePatrolWayPoint)
	{
		m_state = WAITING_FOR_WAYPOINT; // will update on next do_UPDATE()
	}
	else if (m_isAiAgent) {
		m_state = WAITING_FOR_TARGET_POS;
	}
	else
	{
		m_state = IDLE; // stand in place

		PE::Handle h("SoldierNPCMovementSM_Event_STOP", sizeof(SoldierNPCMovementSM_Event_STOP));
		SoldierNPCMovementSM_Event_STOP *pEvt = new(h) SoldierNPCMovementSM_Event_STOP();

		m_hMovementSM.getObject<Component>()->handleEvent(pEvt);
		// release memory now that event is processed
		h.release();
		
	}	
}

void SoldierNPCBehaviorSM::addDefaultComponents()
{
	Component::addDefaultComponents();

	PE_REGISTER_EVENT_HANDLER(SoldierNPCMovementSM_Event_TARGET_REACHED, SoldierNPCBehaviorSM::do_SoldierNPCMovementSM_Event_TARGET_REACHED);
	PE_REGISTER_EVENT_HANDLER(Event_NAVMESH_PROP2, SoldierNPCBehaviorSM::do_SoldierNPCMovementSM_Event_TARGET_REACHED);

	PE_REGISTER_EVENT_HANDLER(Event_UPDATE, SoldierNPCBehaviorSM::do_UPDATE);
	PE_REGISTER_EVENT_HANDLER(Event_MOUSE_CLICK, SoldierNPCBehaviorSM::do_UPDATE);

	PE_REGISTER_EVENT_HANDLER(Event_PRE_RENDER_needsRC, SoldierNPCBehaviorSM::do_PRE_RENDER_needsRC);
}

void SoldierNPCBehaviorSM::do_PathFindingEngine_Event_PATH_POSITIONS_FOUND(PE::Events::Event* pEvt)
{
	PEINFO("SoldierNPCBehaviorSM::do_PathFindingEngine_Event_PATH_POSITIONS_FOUND\n");

	PathFindingEngine_Event_PATH_POSITIONS_FOUND* pathPosFoundEvt = (PathFindingEngine_Event_PATH_POSITIONS_FOUND*)(pEvt);

	m_pathPositions = pathPosFoundEvt->m_pathPositions;
	m_curPathPositionIndex = 0;

	m_state = PATROLLING_NAVMESH_POS;
	PE::Handle h("SoldierNPCMovementSM_Event_MOVE_TO", sizeof(SoldierNPCMovementSM_Event_MOVE_TO));
	Events::SoldierNPCMovementSM_Event_MOVE_TO* pMovementEvt = new(h) SoldierNPCMovementSM_Event_MOVE_TO(m_pathPositions[m_curPathPositionIndex]);
	m_hMovementSM.getObject<Component>()->handleEvent(pMovementEvt);
	// release memory now that event is processed
	h.release();

	m_curPathPositionIndex++;

}

void SoldierNPCBehaviorSM::do_SoldierNPCMovementSM_Event_TARGET_REACHED(PE::Events::Event *pEvt)
{
	PEINFO("SoldierNPCBehaviorSM::do_SoldierNPCMovementSM_Event_TARGET_REACHED\n");

	if (m_state == PATROLLING_WAYPOINTS)
	{
		ClientGameObjectManagerAddon *pGameObjectManagerAddon = (ClientGameObjectManagerAddon *)(m_pContext->get<CharacterControlContext>()->getGameObjectManagerAddon());
		if (pGameObjectManagerAddon)
		{
			// search for waypoint object
			WayPoint *pWP = pGameObjectManagerAddon->getWayPoint(m_curPatrolWayPoint);
			if (pWP && StringOps::length(pWP->m_nextWayPointName) > 0)
			{
				// have next waypoint to go to
				pWP = pGameObjectManagerAddon->getWayPoint(pWP->m_nextWayPointName);
				if (pWP)
				{
					if (*pWP->m_nextWayPointName == '9' || *pWP->m_nextWayPointName == 'a' || *pWP->m_nextWayPointName == 'd') {
						if (!(Event_NAVMESH_PROP2::GetClassId() == pEvt->getClassId())) {
							return;
						}
					}

					StringOps::writeToString(pWP->m_name, m_curPatrolWayPoint, 32);

					m_state = PATROLLING_WAYPOINTS;
					PE::Handle h("SoldierNPCMovementSM_Event_MOVE_TO", sizeof(SoldierNPCMovementSM_Event_MOVE_TO));
					Events::SoldierNPCMovementSM_Event_MOVE_TO *pEvt = new(h) SoldierNPCMovementSM_Event_MOVE_TO(pWP->m_base.getPos());

					m_hMovementSM.getObject<Component>()->handleEvent(pEvt);
					// release memory now that event is processed
					h.release();
				}
			}
			else
			{
				m_state = IDLE;
				// no need to send the event. movement state machine will automatically send event to animation state machine to play idle animation
			}
		}
	}
	else if (m_state == PATROLLING_NAVMESH_POS) {

		if (m_curPathPositionIndex < m_pathPositions.m_size) {
			m_state = PATROLLING_NAVMESH_POS;
			PE::Handle h("SoldierNPCMovementSM_Event_MOVE_TO", sizeof(SoldierNPCMovementSM_Event_MOVE_TO));
			Events::SoldierNPCMovementSM_Event_MOVE_TO* pEvt = new(h) SoldierNPCMovementSM_Event_MOVE_TO(m_pathPositions[m_curPathPositionIndex]);
			m_hMovementSM.getObject<Component>()->handleEvent(pEvt);
			// release memory now that event is processed
			h.release();

			m_curPathPositionIndex++;
		}
		else {
			m_state = WAITING_FOR_TARGET_POS;

		}
	}
}


// this event is executed when thread has RC
void SoldierNPCBehaviorSM::do_PRE_RENDER_needsRC(PE::Events::Event *pEvt)
{
	Event_PRE_RENDER_needsRC *pRealEvent = (Event_PRE_RENDER_needsRC *)(pEvt);
	if (m_havePatrolWayPoint)
	{
		char buf[80];
		if ((m_state == WAITING_FOR_WAYPOINT && *m_curPatrolWayPoint == '1') 
			|| (m_state == PATROLLING_WAYPOINTS && *m_curPatrolWayPoint == '7') 
			|| (m_state == PATROLLING_WAYPOINTS && *m_curPatrolWayPoint == 'b')
			|| (m_state == PATROLLING_WAYPOINTS && *m_curPatrolWayPoint == 'c')
			|| m_state == IDLE) {
			sprintf(buf, "Waiting for next target position");
		}
		else {
			sprintf(buf, "Patrolling NavMesh path points");
		}
		SoldierNPC *pSol = getFirstParentByTypePtr<SoldierNPC>();
		PE::Handle hSoldierSceneNode = pSol->getFirstComponentHandle<PE::Components::SceneNode>();
		Matrix4x4 base = hSoldierSceneNode.getObject<PE::Components::SceneNode>()->m_worldTransform;
		
		DebugRenderer::Instance()->createTextMesh(
			buf, false, false, true, false, 0,
			base.getPos(), 0.01f, pRealEvent->m_threadOwnershipMask);
		
		{
			//we can also construct points ourself
			bool sent = false;
			ClientGameObjectManagerAddon *pGameObjectManagerAddon = (ClientGameObjectManagerAddon *)(m_pContext->get<CharacterControlContext>()->getGameObjectManagerAddon());
			if (pGameObjectManagerAddon)
			{
				WayPoint *pWP = pGameObjectManagerAddon->getWayPoint(m_curPatrolWayPoint);
				if (pWP)
				{
					Vector3 target = pWP->m_base.getPos();
					Vector3 pos = base.getPos();
					Vector3 color(1.0f, 1.0f, 0);
					Vector3 linepts[] = {pos, color, target, color};
					
					//DebugRenderer::Instance()->createLineMesh(true, base,  &linepts[0].m_x, 2, 0);// send event while the array is on the stack
					sent = true;
				}
			}
			//if (!sent) // if for whatever reason we didnt retrieve waypoint info, send the event with transform only
				//DebugRenderer::Instance()->createLineMesh(true, base, NULL, 0, 0);// send event while the array is on the stack
		}
	}
}

void SoldierNPCBehaviorSM::do_UPDATE(PE::Events::Event *pEvt)
{
	if (m_state == WAITING_FOR_WAYPOINT)
	{
		if (m_havePatrolWayPoint)
		{
			if (*m_curPatrolWayPoint == '1') {
				if (Event_MOUSE_CLICK::GetClassId() == pEvt->getClassId()) {
					ClientGameObjectManagerAddon* pGameObjectManagerAddon = (ClientGameObjectManagerAddon*)(m_pContext->get<CharacterControlContext>()->getGameObjectManagerAddon());
					if (pGameObjectManagerAddon)
					{
						// search for waypoint object
						WayPoint* pWP = pGameObjectManagerAddon->getWayPoint(m_curPatrolWayPoint);
						if (pWP)
						{
							m_state = PATROLLING_WAYPOINTS;
							PE::Handle h("SoldierNPCMovementSM_Event_MOVE_TO", sizeof(SoldierNPCMovementSM_Event_MOVE_TO));
							Events::SoldierNPCMovementSM_Event_MOVE_TO* pEvt = new(h) SoldierNPCMovementSM_Event_MOVE_TO(pWP->m_base.getPos());

							m_hMovementSM.getObject<Component>()->handleEvent(pEvt);
							// release memory now that event is processed
							h.release();
						}
					}
				}
			}
			
		}
		else
		{
			// should not happen, but in any case, set state to idle
			m_state = IDLE;

			PE::Handle h("SoldierNPCMovementSM_Event_STOP", sizeof(SoldierNPCMovementSM_Event_STOP));
			SoldierNPCMovementSM_Event_STOP *pEvt = new(h) SoldierNPCMovementSM_Event_STOP();

			m_hMovementSM.getObject<Component>()->handleEvent(pEvt);
			// release memory now that event is processed
			h.release();
		}
	}
	else if (m_state == WAITING_FOR_TARGET_POS) {
		if (Event_MOUSE_CLICK::GetClassId() == pEvt->getClassId())
		{
			Event_MOUSE_CLICK* mouseClickEvt = (Event_MOUSE_CLICK*)(pEvt);
			
			PE::Handle h("Event_FIND_PATH", sizeof(Event_FIND_PATH));
			PE::Events::Event_FIND_PATH* pEvt = new(h) Event_FIND_PATH(mouseClickEvt->m_targetPos);

			m_pathFindingEngine.getObject<Component>()->handleEvent(pEvt);

			h.release();
		}
	}

}


}}




