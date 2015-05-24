#pragma once

#include "GFBXMesh.h"
#include "Utilities.h"
#include "Material.h"
#include <vector>
#include <unordered_map>
#include <fbxsdk.h>

using namespace GFBX;

class GFBXMeshLoader
{
private:
	GFBXMeshLoader();
	virtual ~GFBXMeshLoader();
	static GFBXMeshLoader* inst;
public:

	static GFBXMeshLoader* getInstance();
	static void destroyInstance();

	HRESULT LoadFBXMesh(GFBX::Mesh* targetMesh, WCHAR* file, ID3D11Device* pd3dDevice);
	HRESULT OnCreateDevice(ID3D11Device* pd3dDevice);
	HRESULT OnDestroyDevice();
	HRESULT OnResetDevice(ID3D11Device* pd3dDevice);
	HRESULT OnLostDevice();

	void OutputDebugVertices(GFBX::VERTS* pVerts);
	//bool ComputeBoundingSphere(GFBX::Mesh* pMesh, D3DXVECTOR3& outCenter, float& outRadius);
	//bool ComputeBoundingSphereFromList(D3DXVECTOR3& outCenter, float& outRadius, GFBX::VERTS& verts);

protected:
	HRESULT LoadFBX(GFBX::Mesh* targetMesh, const char* filename, ID3D11Device* pd3dDevice);

	//void DebugOutputSkeletonHierarchy();
	void Clean();
	void ClearData();


///////////////////////////////////////////////////////////////////////
// http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/how-to-work-with-fbx-sdk-r3582

	bool Initialize();

private:
	FbxManager* mFBXManager;
	FbxScene* mFBXScene;
	std::string mInputFilePath;
	std::string mOutputFilePath;
	bool mHasAnimation;
	std::unordered_map<unsigned int, FbxCtrlPoint*> mControlPoints;
	unsigned int mTriangleCount;
	std::vector<FbxTriangle> mTriangles;
	std::vector<PNTIWVertex> mVertices;
	GFBX::FbxSkeleton mSkeleton;
	std::unordered_map<unsigned int, Material*> mMaterialLookUp;
	FbxLongLong mAnimationLength;
	std::string mAnimationName;
	LARGE_INTEGER mCPUFreq;


private:
	HRESULT LoadFBXFromRootNode(GFBX::Mesh* targetMesh, FbxNode* rootNode, ID3D11Device* pd3dDevice);
	void ProcessGeometry(GFBX::Mesh* targetMesh, FbxNode* inNode, ID3D11Device* pd3dDevice);
	void ProcessGeometrySubset(GFBX::MeshSubset* targetMeshSubset, FbxNode* inNode);
	void ProcessSkeletonHierarchy(FbxNode* inRootNode);
	void ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex);
	void ProcessControlPoints(FbxNode* inNode);      //모든 버텍스의 위치를 얻어옴
	void ProcessJointsAndAnimations(FbxNode* inNode, const char * aniName);
	void ProcessAnimationFrames(FbxNode* inNode, FbxAMatrix geometryTransform, FbxCluster* currCluster, const char * aniName);
	FbxAnimStack* GetAnimStack(const char * szName);
	unsigned int FindJointIndexUsingName(const std::string& inJointName);
	bool ProcessMesh(FbxNode* inNode);
	void ReadUV(FbxMesh* inMesh, int inCtrlPointIndex, int inTextureUVIndex, int inUVLayer, D3DXVECTOR2& outUV);
	void ReadNormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, D3DXVECTOR3& outNormal);
	void ReadBinormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, D3DXVECTOR3& outBinormal);
	void ReadTangent(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, D3DXVECTOR3& outTangent);
	bool ReadVertex(FbxMesh* currMesh, int polyId, int vtxId, int vertexCounter, PNTIWVertex &outVtx);
	void Optimize();
	int FindVertex(const PNTIWVertex& inTargetVertex, const std::vector<PNTIWVertex>& uniqueVertices);
	void GenerateTangentsData(FbxNode* inNode);
	bool CopyToTargetMesh(GFBX::MeshSubset* targetMesh);
	bool CopyToTargetAnimation(GFBX::Mesh* targetMesh);

	void AssociateMaterialToMesh(FbxNode* inNode);
	void ProcessMaterials(FbxNode* inNode);
	void ProcessMaterialAttribute(FbxSurfaceMaterial* inMaterial, unsigned int inMaterialIndex);
	void ProcessMaterialTexture(FbxSurfaceMaterial* inMaterial, Material* ioMaterial);
	void PrintMaterial();
	void PrintTriangles();
	void CleanupFbxManager();
	void ClearMeshData();

	//std::vector<GFBX::FbxJoint>	mJoints;
	GFBX::Mesh*					mTargetMesh;
	bool bOptimize;
	bool bAnimationProcessed;
};

