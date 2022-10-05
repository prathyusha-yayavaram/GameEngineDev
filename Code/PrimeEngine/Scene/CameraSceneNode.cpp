#include "CameraSceneNode.h"
#include "../Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"

#define Z_ONLY_CAM_BIAS 0.0f
namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(CameraSceneNode, SceneNode);

CameraSceneNode::CameraSceneNode(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) : SceneNode(context, arena, hMyself)
{
	m_near = 0.05f;
	m_far = 2000.0f;
}
void CameraSceneNode::addDefaultComponents()
{
	Component::addDefaultComponents();
	PE_REGISTER_EVENT_HANDLER(Events::Event_CALCULATE_TRANSFORMATIONS, CameraSceneNode::do_CALCULATE_TRANSFORMATIONS);
}

void CameraSceneNode::do_CALCULATE_TRANSFORMATIONS(Events::Event *pEvt)
{
	Handle hParentSN = getFirstParentByType<SceneNode>();
	if (hParentSN.isValid())
	{
		Matrix4x4 parentTransform = hParentSN.getObject<PE::Components::SceneNode>()->m_worldTransform;
		m_worldTransform = parentTransform * m_base;
	}
	
	Matrix4x4 &mref_worldTransform = m_worldTransform;

	Vector3 pos = Vector3(mref_worldTransform.m[0][3], mref_worldTransform.m[1][3], mref_worldTransform.m[2][3]);
	Vector3 n = Vector3(mref_worldTransform.m[0][2], mref_worldTransform.m[1][2], mref_worldTransform.m[2][2]);
	Vector3 target = pos + n;
	Vector3 up = Vector3(mref_worldTransform.m[0][1], mref_worldTransform.m[1][1], mref_worldTransform.m[2][1]);

	m_worldToViewTransform = CameraOps::CreateViewMatrix(pos, target, up);

	m_worldTransform2 = mref_worldTransform;

	m_worldTransform2.moveForward(Z_ONLY_CAM_BIAS);

	Vector3 pos2 = Vector3(m_worldTransform2.m[0][3], m_worldTransform2.m[1][3], m_worldTransform2.m[2][3]);
	Vector3 n2 = Vector3(m_worldTransform2.m[0][2], m_worldTransform2.m[1][2], m_worldTransform2.m[2][2]);
	Vector3 target2 = pos2 + n2;
	Vector3 up2 = Vector3(m_worldTransform2.m[0][1], m_worldTransform2.m[1][1], m_worldTransform2.m[2][1]);

	m_worldToViewTransform2 = CameraOps::CreateViewMatrix(pos2, target2, up2);
    
    PrimitiveTypes::Float32 aspect = (PrimitiveTypes::Float32)(m_pContext->getGPUScreen()->getWidth()) / (PrimitiveTypes::Float32)(m_pContext->getGPUScreen()->getHeight());
    
    PrimitiveTypes::Float32 verticalFov = 0.33f * PrimitiveTypes::Constants::c_Pi_F32;
    if (aspect < 1.0f)
    {
        //ios portrait view
        static PrimitiveTypes::Float32 factor = 0.5f;
        verticalFov *= factor;
    }

	m_viewToProjectedTransform = CameraOps::CreateProjectionMatrix(verticalFov, 
		aspect,
		m_near, m_far);
	Matrix4x4 m_planeCalc = m_viewToProjectedTransform * m_worldToViewTransform;
	//Left
	m_pLeft.a = m_planeCalc.m[3][0] + m_planeCalc.m[0][0];
	m_pLeft.b = m_planeCalc.m[3][1] + m_planeCalc.m[0][1];
	m_pLeft.c = m_planeCalc.m[3][2] + m_planeCalc.m[0][2];
	m_pLeft.d = m_planeCalc.m[3][3] + m_planeCalc.m[0][3];
	float norm = sqrt(pow(m_pLeft.a, 2) + pow(m_pLeft.b, 2) + pow(m_pLeft.c, 2));
	m_pLeft.a = m_pLeft.a / norm;
	m_pLeft.b = m_pLeft.b / norm;
	m_pLeft.c = m_pLeft.c / norm;
	m_pLeft.d = m_pLeft.d / norm;
	//Right
	m_pRight.a = m_planeCalc.m[3][0] - m_planeCalc.m[0][0];
	m_pRight.b = m_planeCalc.m[3][1] - m_planeCalc.m[0][1];
	m_pRight.c = m_planeCalc.m[3][2] - m_planeCalc.m[0][2];
	m_pRight.d = m_planeCalc.m[3][3] - m_planeCalc.m[0][3];
	norm = sqrt(pow(m_pRight.a, 2) + pow(m_pRight.b, 2) + pow(m_pRight.c, 2));
	m_pRight.a = m_pRight.a / norm;
	m_pRight.b = m_pRight.b / norm;
	m_pRight.c = m_pRight.c / norm;
	m_pRight.d = m_pRight.d / norm;
	//Top
	m_pTop.a = m_planeCalc.m[3][0] - m_planeCalc.m[1][0];
	m_pTop.b = m_planeCalc.m[3][1] - m_planeCalc.m[1][1];
	m_pTop.c = m_planeCalc.m[3][2] - m_planeCalc.m[1][2];
	m_pTop.d = m_planeCalc.m[3][3] - m_planeCalc.m[1][3];
	norm = sqrt(pow(m_pTop.a, 2) + pow(m_pTop.b, 2) + pow(m_pTop.c, 2));
	m_pTop.a = m_pTop.a / norm;
	m_pTop.b = m_pTop.b / norm;
	m_pTop.c = m_pTop.c / norm;
	m_pTop.d = m_pTop.d / norm;
	//Bottom
	m_pBottom.a = m_planeCalc.m[3][0] + m_planeCalc.m[1][0];
	m_pBottom.b = m_planeCalc.m[3][1] + m_planeCalc.m[1][1];
	m_pBottom.c = m_planeCalc.m[3][2] + m_planeCalc.m[1][2];
	m_pBottom.d = m_planeCalc.m[3][3] + m_planeCalc.m[1][3];
	norm = sqrt(pow(m_pBottom.a, 2) + pow(m_pBottom.b, 2) + pow(m_pBottom.c, 2));
	m_pBottom.a = m_pBottom.a / norm;
	m_pBottom.b = m_pBottom.b / norm;
	m_pBottom.c = m_pBottom.c / norm;
	m_pBottom.d = m_pBottom.d / norm;
	//Near
	m_pNear.a = m_planeCalc.m[3][0] + m_planeCalc.m[2][0];
	m_pNear.b = m_planeCalc.m[3][1] + m_planeCalc.m[2][1];
	m_pNear.c = m_planeCalc.m[3][2] + m_planeCalc.m[2][2];
	m_pNear.d = m_planeCalc.m[3][3] + m_planeCalc.m[2][3];
	norm = sqrt(pow(m_pNear.a, 2) + pow(m_pNear.b, 2) + pow(m_pNear.c, 2));
	m_pNear.a = m_pNear.a / norm;
	m_pNear.b = m_pNear.b / norm;
	m_pNear.c = m_pNear.c / norm;
	m_pNear.d = m_pNear.d / norm;
	//Far
	m_pFar.a = m_planeCalc.m[3][0] - m_planeCalc.m[2][0];
	m_pFar.b = m_planeCalc.m[3][1] - m_planeCalc.m[2][1];
	m_pFar.c = m_planeCalc.m[3][2] - m_planeCalc.m[2][2];
	m_pFar.d = m_planeCalc.m[3][3] - m_planeCalc.m[2][3];
	norm = sqrt(pow(m_pFar.a, 2) + pow(m_pFar.b, 2) + pow(m_pFar.c, 2));
	m_pFar.a = m_pFar.a / norm;
	m_pFar.b = m_pFar.b / norm;
	m_pFar.c = m_pFar.c / norm;
	m_pFar.d = m_pFar.d / norm;

	SceneNode::do_CALCULATE_TRANSFORMATIONS(pEvt);

}

}; // namespace Components
}; // namespace PE
