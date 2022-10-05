// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "MeshManager.h"
// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/APIAbstraction/GPUMaterial/GPUMaterialSet.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/Texture/Texture.h"
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPUManager.h"
#include "PrimeEngine/../../GlobalConfig/GlobalConfig.h"

#include "PrimeEngine/Geometry/SkeletonCPU/SkeletonCPU.h"

#include "PrimeEngine/Scene/RootSceneNode.h"

#include "Light.h"

// Sibling/Children includes

#include "MeshInstance.h"
#include "Skeleton.h"
#include "SceneNode.h"
#include "DrawList.h"
#include "SH_DRAW.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"

namespace PE {
namespace Components{

PE_IMPLEMENT_CLASS1(MeshManager, Component);
MeshManager::MeshManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
	: Component(context, arena, hMyself)
	, m_assets(context, arena, 256)
{
}

PE::Handle MeshManager::getAsset(const char *asset, const char *package, int &threadOwnershipMask)
{
	char key[StrTPair<Handle>::StrSize];
	sprintf(key, "%s/%s", package, asset);
	
	int index = m_assets.findIndex(key);
	if (index != -1)
	{
		return m_assets.m_pairs[index].m_value;
	}
	Handle h;

	if (StringOps::endswith(asset, "skela"))
	{
		PE::Handle hSkeleton("Skeleton", sizeof(Skeleton));
		Skeleton *pSkeleton = new(hSkeleton) Skeleton(*m_pContext, m_arena, hSkeleton);
		pSkeleton->addDefaultComponents();

		pSkeleton->initFromFiles(asset, package, threadOwnershipMask);
		h = hSkeleton;
	}
	else if (StringOps::endswith(asset, "mesha"))
	{
		MeshCPU mcpu(*m_pContext, m_arena);
		mcpu.ReadMesh(asset, package, "");
		
		PE::Handle hMesh("Mesh", sizeof(Mesh));
		Mesh *pMesh = new(hMesh) Mesh(*m_pContext, m_arena, hMesh);
		//sprintf(pMesh->m_hPositionBufferCPU);
		pMesh->addDefaultComponents();
		
		pMesh->loadFromMeshCPU_needsRC(mcpu, threadOwnershipMask);
		Array<PrimitiveTypes::Float32> m_positionValues = pMesh->m_hPositionBufferCPU.getObject<PositionBufferCPU>()->m_values;
		float min_x = NULL, min_y = NULL, min_z = NULL, max_x = NULL, max_y = NULL, max_z = NULL;
		for (int i = 0; i < m_positionValues.m_size; i++) {
			PrimitiveTypes::Float32 curValue = m_positionValues.getByIndexUnchecked(i);
			if (i % 3 == 0) {
				if (min_x == NULL) {
					min_x = curValue;
				}
				if (max_x == NULL) {
					max_x = curValue;
				}
				min_x = min(min_x, curValue);
				max_x = max(max_x, curValue);
			}
			else if (i % 3 == 1) {
				if (min_y == NULL) {
					min_y = curValue;
				}
				if (max_y == NULL) {
					max_y = curValue;
				}
				min_y = min(min_y, curValue);
				max_y = max(max_y, curValue);
			}
			else {
				if (min_z == NULL) {
					min_z = curValue;
				}
				if (max_z == NULL) {
					max_z = curValue;
				}
				min_z = min(min_z, curValue);
				max_z = max(max_z, curValue);
			}
			
			pMesh->m_aabb.reset(8);
			pMesh->m_aabb.add(Vector3(min_x, min_y, min_z));
			pMesh->m_aabb.add(Vector3(min_x, max_y, min_z));
			pMesh->m_aabb.add(Vector3(max_x, max_y, min_z));
			pMesh->m_aabb.add(Vector3(max_x, min_y, min_z));
			pMesh->m_aabb.add(Vector3(min_x, min_y, max_z));
			pMesh->m_aabb.add(Vector3(min_x, max_y, max_z));
			pMesh->m_aabb.add(Vector3(max_x, max_y, max_z));
			pMesh->m_aabb.add(Vector3(max_x, min_y, max_z));
		}
#if PE_API_IS_D3D11
		// todo: work out how lods will work
		//scpu.buildLod();
#endif
        // generate collision volume here. or you could generate it in MeshCPU::ReadMesh()
        pMesh->m_performBoundingVolumeCulling = true; // will now perform tests for this mesh

		h = hMesh;
	}


	PEASSERT(h.isValid(), "Something must need to be loaded here");

	RootSceneNode::Instance()->addComponent(h);
	m_assets.add(key, h);
	return h;
}

void MeshManager::registerAsset(const PE::Handle &h)
{
	static int uniqueId = 0;
	++uniqueId;
	char key[StrTPair<Handle>::StrSize];
	sprintf(key, "__generated_%d", uniqueId);
	
	int index = m_assets.findIndex(key);
	PEASSERT(index == -1, "Generated meshes have to be unique");
	
	RootSceneNode::Instance()->addComponent(h);
	m_assets.add(key, h);
}

}; // namespace Components
}; // namespace PE
