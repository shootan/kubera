#include "DXUT.h"
#include "SDKmisc.h"
#include "GFBXMeshLoader.h"

using namespace GFBX;

GFBXMeshLoader* GFBXMeshLoader::inst = 0;
GFBXMeshLoader::GFBXMeshLoader()
{
	Clean();
}
GFBXMeshLoader::~GFBXMeshLoader()
{
	
}
GFBXMeshLoader* GFBXMeshLoader::getInstance()
{
	if (inst == 0)
		inst = new GFBXMeshLoader();
	return inst;
}
void GFBXMeshLoader::destroyInstance()
{
	if (GFBXMeshLoader::inst)
		delete GFBXMeshLoader::inst;
	GFBXMeshLoader::inst = 0;
}



HRESULT GFBXMeshLoader::OnCreateDevice(ID3D11Device* pd3dDevice)
{
	return S_OK;
}


HRESULT GFBXMeshLoader::OnDestroyDevice()
{

	return S_OK;
}

HRESULT GFBXMeshLoader::OnResetDevice(ID3D11Device* pd3dDevice)
{
	return S_OK;
}

HRESULT GFBXMeshLoader::OnLostDevice()
{
	return S_OK;
}

void GFBXMeshLoader::ClearData()
{
	mTriangleCount = 0;
	mControlPoints.clear();
	mTriangles.clear();
	mVertices.clear();
	mMaterialLookUp.clear();
	mInputFilePath.clear();
	mOutputFilePath.clear();
	mAnimationLength = 0;
	mAnimationName.clear();
}


void GFBXMeshLoader::Clean()
{
	mTargetMesh = NULL;
	mFBXManager = nullptr;
	mFBXScene = nullptr;

	//mJoints.clear();
	mSkeleton.mJoints.clear();
	mHasAnimation = true;
	bAnimationProcessed = false;

	ClearData();
}


void GFBXMeshLoader::ClearMeshData()
{

}


HRESULT GFBXMeshLoader::LoadFBXMesh(GFBX::Mesh* targetMesh, WCHAR* file, ID3D11Device* pd3dDevice)
{
	Clean();
	if (!targetMesh)
		return S_FALSE;
	//targetMesh->Clear();
	HRESULT hr;
	WCHAR s[MAX_PATH];
	V_RETURN(DXUTFindDXSDKMediaFileCch(s, MAX_PATH, file));
	std::wstring wstr(s);
	std::string str("");
	str.assign(wstr.begin(), wstr.end());
	if (LoadFBX(targetMesh, str.c_str(), pd3dDevice) != S_OK)
		return S_FALSE;

	Clean();

	return S_OK;
}

void GFBXMeshLoader::OutputDebugVertices(GFBX::VERTS* pVerts)
{
	if (!pVerts)
		return;
	int nIdx = 0;
	GFBX::VERTS::iterator it = pVerts->begin();
	while ( it != pVerts->end()) {
		GFBX::Vertex vertex = *it;
		// debug string
		char szTmp[256];
		sprintf(szTmp, "%d => pos : %f %f %f    norm : %f %f %f   uv : %f %f  \n", 
			nIdx, vertex.pos.x, vertex.pos.y, vertex.pos.z,
			vertex.norm.x, vertex.norm.y, vertex.norm.z, vertex.uv.x, vertex.uv.y);
		OutputDebugStringA(szTmp);
		it++;
	}
}


HRESULT GFBXMeshLoader::LoadFBX(GFBX::Mesh* targetMesh, const char* filename, ID3D11Device* pd3dDevice)
{
	if (mFBXManager == nullptr)
	{
		mFBXManager = FbxManager::Create();
		FbxIOSettings* pIOsettings = FbxIOSettings::Create(mFBXManager, IOSROOT);
		mFBXManager->SetIOSettings(pIOsettings);
	}

	FbxImporter* pImporter = FbxImporter::Create(mFBXManager, "");
	mFBXScene = FbxScene::Create(mFBXManager, "");

	bool bSuccess = pImporter->Initialize(filename, -1, mFBXManager->GetIOSettings());
	if (!bSuccess) 
		return E_FAIL;
	bSuccess = pImporter->Import(mFBXScene);
	if (!bSuccess) 
		return E_FAIL;
	pImporter->Destroy();

	FbxNode* pFbxRootNode = mFBXScene->GetRootNode();
	if (pFbxRootNode)
	{
		LoadFBXFromRootNode( targetMesh, pFbxRootNode, pd3dDevice);
	}
	
	if (mFBXScene)
	{
		mFBXScene->Destroy();
		mFBXScene = NULL;
	}
	if (mFBXManager)
	{
		mFBXManager->Destroy();
		mFBXManager = NULL;
	}

	//ComputeBoundingSphere(targetMesh, targetMesh->m_vBoundingCenter, targetMesh->m_fBoundingRadius);

	return S_OK;
}

//
//bool GFBXMeshLoader::ComputeBoundingSphere(GFBX::Mesh* pMesh, D3DXVECTOR3& center, float& radius)
//{
//	GFBX::VERTS vrtsRet;
//	for (int i = 0; i < pMesh->GetSubsetCount(); i++)
//	{
//		GFBX::VERTS verts = pMesh->GetSubset(i)->m_verts;
//		for (int j = 0; j < verts.size(); j++)
//		{
//			vrtsRet.push_back(verts[j]);
//		}
//	}
//
//	return ComputeBoundingSphereFromList(center, radius, vrtsRet);
//}
//
//bool GFBXMeshLoader::ComputeBoundingSphereFromList(D3DXVECTOR3& center, float& radius, GFBX::VERTS& verts)
//{
//	if (verts.size() <= 0)
//	{
//		center = D3DXVECTOR3(0, 0, 0);
//		radius = 0;
//		return false;
//	}
//
//	double dx, dy, dz;
//	double rad_sq, xspan, yspan, zspan, maxspan;
//	double old_to_p, old_to_p_sq, old_to_new;
//
//	D3DXVECTOR3 xmin, xmax, ymin, ymax, zmin, zmax, dia1, dia2;
//
//
//	// FIRST PASS: find 6 minima/maxima points
//	xmin.x = ymin.y = zmin.z = FLT_MAX; // initialize for min/max compare 
//	xmax.x = ymax.y = zmax.z = -FLT_MAX;
//
//	for (size_t i = 0; i<verts.size(); i++)
//	{
//		const D3DXVECTOR3 & caller_p = verts[i].pos;
//
//		// his ith point.
//		if (caller_p.x<xmin.x)
//			xmin = caller_p;
//		if (caller_p.x>xmax.x)
//			xmax = caller_p;
//		if (caller_p.y<ymin.y)
//			ymin = caller_p;
//		if (caller_p.y>ymax.y)
//			ymax = caller_p;
//		if (caller_p.z<zmin.z)
//			zmin = caller_p;
//		if (caller_p.z>zmax.z)
//			zmax = caller_p;
//	}
//	// Set xspan = distance between the 2 points xmin & xmax (squared)
//	dx = xmax.x - xmin.x;
//	dy = xmax.y - xmin.y;
//	dz = xmax.z - xmin.z;
//	xspan = dx*dx + dy*dy + dz*dz;
//	// Same for y & z spans 
//	dx = ymax.x - ymin.x;
//	dy = ymax.y - ymin.y;
//	dz = ymax.z - ymin.z;
//	yspan = dx*dx + dy*dy + dz*dz;
//	dx = zmax.x - zmin.x;
//	dy = zmax.y - zmin.y;
//	dz = zmax.z - zmin.z;
//	zspan = dx*dx + dy*dy + dz*dz;
//
//	// Set points dia1 & dia2 to the maximally seperated pair
//	dia1 = xmin; dia2 = xmax; // assume xspan biggest
//	maxspan = xspan;
//	if (yspan>maxspan)
//	{
//		maxspan = yspan;
//		dia1 = ymin; dia2 = ymax;
//	}
//	if (zspan>maxspan)
//	{
//		dia1 = zmin; dia2 = zmax;
//	}
//
//	// dia1,dia2 is a diameter of initial sphere
//	// calc initial center
//	center.x = (dia1.x + dia2.x) / 2.0f;
//	center.y = (dia1.y + dia2.y) / 2.0f;
//	center.z = (dia1.z + dia2.z) / 2.0f;
//	// calculate initial radius**2 and radius
//	dx = dia2.x - center.x;
//	dy = dia2.y - center.y;
//	dz = dia2.z - center.z;
//	rad_sq = dx*dx + dy*dy + dz*dz;
//	radius = sqrt(rad_sq);
//
//	// SECOND PASS: increment current sphere
//	for (size_t i = 0; i<verts.size(); i++)
//	{
//		const D3DXVECTOR3 & caller_p = verts[i].pos;
//		// with his ith point.
//		dx = caller_p.x - center.x;
//		dy = caller_p.y - center.y;
//		dz = caller_p.z - center.z;
//		old_to_p_sq = dx*dx + dy*dy + dz*dz;
//		if (old_to_p_sq > rad_sq) 	// do r**2 test first
//		{
//			// this point is outside of current sphere
//			old_to_p = sqrt(old_to_p_sq);
//			// calc radius of new sphere
//			radius = (radius + old_to_p) / 2.0;
//			rad_sq = radius * radius; 	// for next r**2 compare
//			old_to_new = old_to_p - radius;
//			// calc center of new sphere
//			center.x = (float)((radius*center.x + old_to_new*caller_p.x) / old_to_p);
//			center.y = (float)((radius*center.y + old_to_new*caller_p.y) / old_to_p);
//			center.z = (float)((radius*center.z + old_to_new*caller_p.z) / old_to_p);
//			//printf("\n New sphere: cen,rad = %f %f %f   %f", cen.x,cen.y,cen.z, rad);
//		}
//	}
//
//	return true;
//}

/*
void GFBXMeshLoader::DebugOutputSkeletonHierarchy()
{
	for (int i = 0; i < mJoints.size(); i++)
	{
		char szTmp[256];
		sprintf(szTmp, "id:%d name:%s \n", mJoints[i].mParentIndex, mJoints[i].mName.c_str() );
		OutputDebugStringA(szTmp);
	}

}
*/



HRESULT GFBXMeshLoader::LoadFBXFromRootNode(GFBX::Mesh* targetMesh, FbxNode* rootNode, ID3D11Device* pd3dDevice)
{
	ProcessSkeletonHierarchy(rootNode);
	if (mSkeleton.mJoints.empty())
	{
		mHasAnimation = false;
	}
	ProcessGeometry(targetMesh, rootNode, pd3dDevice);
	CopyToTargetAnimation(targetMesh);

	return S_OK;
}

void GFBXMeshLoader::GenerateTangentsData(FbxNode* inNode)
{
	FbxMesh* currMesh = inNode->GetMesh();
	currMesh->GenerateTangentsData(0);
}

void GFBXMeshLoader::ProcessGeometrySubset(GFBX::MeshSubset* targetMeshSubset, FbxNode* inNode)
{
	ClearData();
	GenerateTangentsData(inNode);
	ProcessControlPoints(inNode);
	if (mHasAnimation)
	{
		ProcessJointsAndAnimations(inNode, NULL);
	}
	ProcessMesh(inNode);
	AssociateMaterialToMesh(inNode);
	ProcessMaterials(inNode);
	Optimize();
	CopyToTargetMesh(targetMeshSubset);

}

void GFBXMeshLoader::ProcessGeometry(GFBX::Mesh* targetMesh, FbxNode* inNode, ID3D11Device* pd3dDevice)
{
	OutputDebugString(L"ProcessGeometry...\n");

	if (inNode->GetNodeAttribute())
	{
		switch (inNode->GetNodeAttribute()->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			ProcessGeometrySubset(targetMesh->CreateSubset(pd3dDevice), inNode);
			break;
		}
	}

	for (int i = 0; i < inNode->GetChildCount(); ++i)
	{
		ProcessGeometry(targetMesh, inNode->GetChild(i), pd3dDevice);
	}
}

void GFBXMeshLoader::ProcessSkeletonHierarchy(FbxNode* inRootNode)
{
	OutputDebugString(L"ProcessSkeletonHierarchy...\n");

	for (int childIndex = 0; childIndex < inRootNode->GetChildCount(); ++childIndex)
	{
		FbxNode* currNode = inRootNode->GetChild(childIndex);
		ProcessSkeletonHierarchyRecursively(currNode, 0, 0, -1);
	}
}

void GFBXMeshLoader::ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex)
{
	if (inNode->GetNodeAttribute() && inNode->GetNodeAttribute()->GetAttributeType() && inNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		FbxJoint currJoint;
		currJoint.mParentIndex = inParentIndex;
		currJoint.mName = inNode->GetName();
		mSkeleton.mJoints.push_back(currJoint);
	}
	for (int i = 0; i < inNode->GetChildCount(); i++)
	{
		ProcessSkeletonHierarchyRecursively(inNode->GetChild(i), inDepth + 1, mSkeleton.mJoints.size(), myIndex);
	}
}

void GFBXMeshLoader::ProcessControlPoints(FbxNode* inNode)
{
	FbxMesh* currMesh = inNode->GetMesh(); 
	unsigned int ctrlPointCount = currMesh->GetControlPointsCount();
	for (unsigned int i = 0; i < ctrlPointCount; ++i)
	{
		FbxCtrlPoint* currCtrlPoint = new FbxCtrlPoint();
		D3DXVECTOR3 currPosition;
		currPosition.x = static_cast<float>(currMesh->GetControlPointAt(i).mData[0]);
		currPosition.y = static_cast<float>(currMesh->GetControlPointAt(i).mData[1]);
		currPosition.z = static_cast<float>(currMesh->GetControlPointAt(i).mData[2]);
		currCtrlPoint->mPosition = currPosition;
		mControlPoints[i] = currCtrlPoint;
	}
}

FbxAnimStack* GFBXMeshLoader::GetAnimStack(const char * szName)
{
	FbxAnimStack* currAnimStack = NULL;

	if (!mFBXScene)
		return currAnimStack;

	int animStackCount = mFBXScene->GetSrcObjectCount<FbxAnimStack>();
	for (int i = 0; i < animStackCount; i++)
	{
		currAnimStack = mFBXScene->GetSrcObject<FbxAnimStack>(i);
		FbxString animStackName = currAnimStack->GetName();
		if (!szName || strcmp(animStackName.Buffer(), szName))
		{
			break;
		}
	}
	if (!currAnimStack)
		currAnimStack = mFBXScene->GetSrcObject<FbxAnimStack>(0);

	return currAnimStack;
}


void GFBXMeshLoader::ProcessAnimationFrames(FbxNode* inNode,
	FbxAMatrix geometryTransform, FbxCluster* currCluster, const char * aniName)
{
	std::string currJointName = currCluster->GetLink()->GetName();
	unsigned int currJointIndex = FindJointIndexUsingName(currJointName);

	// Get animation information
	// Now only supports one take
	//int animStackCount = mFBXScene->GetSrcObjectCount<FbxAnimStack>();
	FbxAnimStack* currAnimStack = GetAnimStack(aniName);
	FbxString animStackName = currAnimStack->GetName();
	mAnimationName = animStackName.Buffer();
	FbxTakeInfo* takeInfo = mFBXScene->GetTakeInfo(animStackName);
	FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
	FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
	mAnimationLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
	FbxKeyframe** currAnim = &mSkeleton.mJoints[currJointIndex].mAnimation;

	for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
	{
		FbxTime currTime;
		currTime.SetFrame(i, FbxTime::eFrames24);
		*currAnim = new FbxKeyframe();
		(*currAnim)->mFrameNum = i;
		FbxAMatrix currentTransformOffset = inNode->EvaluateGlobalTransform(currTime) * geometryTransform;
		(*currAnim)->mGlobalTransform = currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform(currTime);
		currAnim = &((*currAnim)->mNext);
	}
}


void GFBXMeshLoader::ProcessJointsAndAnimations(FbxNode* inNode, const char * aniName)
{
	if (bAnimationProcessed)
		return;
	if (!mFBXScene)
		return;
	FbxMesh* currMesh = inNode->GetMesh();
	unsigned int numOfDeformers = currMesh->GetDeformerCount();
	// This geometry transform is something I cannot understand
	// I think it is from MotionBuilder
	// If you are using Maya for your models, 99% this is just an
	// identity matrix
	// But I am taking it into account anyways......
	FbxAMatrix geometryTransform = Utilities::GetGeometryTransformation(inNode);

	// A deformer is a FBX thing, which contains some clusters
	// A cluster contains a link, which is basically a joint
	// Normally, there is only one deformer in a mesh
	for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
	{
		// There are many types of deformers in Maya,
		// We are using only skins, so we see if this is a skin
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (!currSkin)
		{
			continue;
		}
		else
		{
			bAnimationProcessed = true;
		}

		unsigned int numOfClusters = currSkin->GetClusterCount();
		for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
		{
			FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);
			std::string currJointName = currCluster->GetLink()->GetName();
			unsigned int currJointIndex = FindJointIndexUsingName(currJointName);
			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix globalBindposeInverseMatrix;

			currCluster->GetTransformMatrix(transformMatrix);	// The transformation of the mesh at binding time
			currCluster->GetTransformLinkMatrix(transformLinkMatrix);	// The transformation of the cluster(joint) at binding time from joint space to world space
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

			// Update the information in mSkeleton 
			mSkeleton.mJoints[currJointIndex].mGlobalBindposeInverse = globalBindposeInverseMatrix;
			mSkeleton.mJoints[currJointIndex].mNode = currCluster->GetLink();
			mSkeleton.mJoints[currJointIndex].mGlobalBindpose = transformLinkMatrix;

			// Associate each joint with the control points it affects
			unsigned int numOfIndices = currCluster->GetControlPointIndicesCount();
			for (unsigned int i = 0; i < numOfIndices; ++i)
			{
				FbxBlendingIndexWeightPair currBlendingIndexWeightPair;
				currBlendingIndexWeightPair.mBlendingIndex = currJointIndex;
				currBlendingIndexWeightPair.mBlendingWeight = currCluster->GetControlPointWeights()[i];
				if (currCluster->GetControlPointIndices()[i] < mControlPoints.size())
					mControlPoints[currCluster->GetControlPointIndices()[i]]->mBlendingInfo.push_back(currBlendingIndexWeightPair);
			}

			ProcessAnimationFrames(inNode, geometryTransform, currCluster, aniName);
		}
	}

	// Some of the control points only have less than 4 joints
	// affecting them.
	// For a normal renderer, there are usually 4 joints
	// I am adding more dummy joints if there isn't enough
	FbxBlendingIndexWeightPair currBlendingIndexWeightPair;
	currBlendingIndexWeightPair.mBlendingIndex = 0;
	currBlendingIndexWeightPair.mBlendingWeight = 0;
	for (auto itr = mControlPoints.begin(); itr != mControlPoints.end(); ++itr)
	{
		for (unsigned int i = itr->second->mBlendingInfo.size(); i <= 4; ++i)
		{
			itr->second->mBlendingInfo.push_back(currBlendingIndexWeightPair);
		}
	}
}

unsigned int GFBXMeshLoader::FindJointIndexUsingName(const std::string& inJointName)
{
	for (unsigned int i = 0; i < mSkeleton.mJoints.size(); ++i)
	{
		if (mSkeleton.mJoints[i].mName == inJointName)
		{
			return i;
		}
	}

	throw std::exception("FbxSkeleton information in FBX file is corrupted.");
}


bool GFBXMeshLoader::ReadVertex(FbxMesh* currMesh, int polyId, int vtxId, 
	int vertexCounter, PNTIWVertex &outVtx)
{
	D3DXVECTOR3 normal;
	D3DXVECTOR3 tangent;
	D3DXVECTOR3 binormal;
	D3DXVECTOR2 UV;

	int ctrlPointIndex = currMesh->GetPolygonVertex(polyId, vtxId);
	if (ctrlPointIndex < 0)
		return false;
	FbxCtrlPoint* currCtrlPoint = mControlPoints[ctrlPointIndex];

	ReadNormal(currMesh, ctrlPointIndex, vertexCounter, normal);
	ReadBinormal(currMesh, ctrlPointIndex, vertexCounter, binormal);
	ReadTangent(currMesh, ctrlPointIndex, vertexCounter, tangent);
	// We only have diffuse texture
	for (int k = 0; k < 1; ++k)
	{
		ReadUV(currMesh, ctrlPointIndex, currMesh->GetTextureUVIndex(polyId, vtxId), k, UV);
	}

	PNTIWVertex temp;
	temp.mPosition = currCtrlPoint->mPosition;
	temp.mNormal = normal;
	temp.mUV = UV;
	temp.mTangent = tangent;
	temp.mBinormal = binormal;
	// Copy the blending info from each control point
	for (unsigned int k = 0; k < currCtrlPoint->mBlendingInfo.size(); ++k)
	{
		VertexBlendingInfo currBlendingInfo;
		currBlendingInfo.mBlendingIndex = currCtrlPoint->mBlendingInfo[k].mBlendingIndex;
		currBlendingInfo.mBlendingWeight = currCtrlPoint->mBlendingInfo[k].mBlendingWeight;
		temp.mVertexBlendingInfos.push_back(currBlendingInfo);
	}
	// Sort the blending info so that later we can remove
	// duplicated vertices
	temp.SortBlendingInfoByWeight();
	outVtx = temp;

	return true;
}

bool GFBXMeshLoader::ProcessMesh(FbxNode* inNode)
{
	if (mTriangles.size() > 0)
		return false;

	FbxMesh* currMesh = inNode->GetMesh();

	mTriangleCount = currMesh->GetPolygonCount();
	int vertexCounter = 0;
	mTriangles.reserve(mTriangleCount);

	for (unsigned int i = 0; i < mTriangleCount; ++i)
	{

		int idsV3[] = { 0, 1, 2 };			// for polygon has 3 vertices
		int idsV4[] = { 0, 1, 2, 3, 0, 2 };	// for polygon has 4 vertices
		//int idsV4[] = { 0, 1, 2 };
		int idsV5[] = { 0, 1, 2, 3, 0, 2, 3, 4, 0};
		//int idsV5[] = { 0, 1, 2 };

		int* ids = NULL;
		int idcnt = 0;
		int iNumVertices = currMesh->GetPolygonSize(i);
		if (iNumVertices == 3){
			ids = idsV3;
			idcnt = sizeof(idsV3) / sizeof(int);
			//OutputDebugStringA("This Mesh made in 3 Polygons");
		}
		else if (iNumVertices == 4) {
			ids = idsV4;
			idcnt = sizeof(idsV4) / sizeof(int);
			//OutputDebugStringA("This Mesh made in 4 Polygons");
		}
		else if (iNumVertices == 5) {
			ids = idsV5;
			idcnt = sizeof(idsV5) / sizeof(int);
			//OutputDebugStringA("This Mesh made in 4 Polygons");
		}
		else {
			assert("It is Only allowed mesh made in 3 or 4 or 5 polygons");
			continue;
		}

		FbxTriangle currTriangle;
		int PolyVtxCnt = 0;
		for (unsigned int j = 0; j < idcnt; ++j)
		{
			int id = ids[j];
			PNTIWVertex temp;
			if (ReadVertex(currMesh, i, id, vertexCounter + id, temp))
			{
				mVertices.push_back(temp);
				currTriangle.mIndices.push_back(vertexCounter + id);
				//++vertexCounter;
				if ((++PolyVtxCnt) % 3 == 0) {	// 3°³ ½×ÀÌ¸é »ï°¢Çü
					mTriangles.push_back(currTriangle);
					currTriangle.clear();
				}
			}
			else
			{
				OutputDebugString(L"Some vertex was wrong ... \n");
			}
		}

		//if (PolyVtxCnt < iNumVertices)
		{
			vertexCounter += iNumVertices;	// -PolyVtxCnt;
		}
	}

	// Now mControlPoints has served its purpose
	// We can free its memory
	for (auto itr = mControlPoints.begin(); itr != mControlPoints.end(); ++itr)
	{
		delete itr->second;
	}
	mControlPoints.clear();

	return true;
}


void GFBXMeshLoader::ReadUV(FbxMesh* inMesh, int inCtrlPointIndex, int inTextureUVIndex, int inUVLayer, 
	D3DXVECTOR2& outUV)
{
	if (inUVLayer >= 2 || inMesh->GetElementUVCount() <= inUVLayer)
	{
		throw std::exception("Invalid UV Layer Number");
	}
	FbxGeometryElementUV* vertexUV = inMesh->GetElementUV(inUVLayer);

	switch (vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
		}
			break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(inCtrlPointIndex);
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
		}
			break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inTextureUVIndex).mData[0]);
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(inTextureUVIndex).mData[1]);
		}
			break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void GFBXMeshLoader::ReadNormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, D3DXVECTOR3& outNormal)
{
	if (inMesh->GetElementNormalCount() < 1)
	{
		throw std::exception("Invalid Normal Number");
	}

	FbxGeometryElementNormal* vertexNormal = inMesh->GetElementNormal(0);
	switch (vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
		}
			break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(inCtrlPointIndex);
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
			break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[2]);
		}
			break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(inVertexCounter);
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
			break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void GFBXMeshLoader::ReadBinormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, D3DXVECTOR3& outBinormal)
{
	if (inMesh->GetElementBinormalCount() < 1)
	{
		throw std::exception("Invalid Binormal Number");
	}

	FbxGeometryElementBinormal* vertexBinormal = inMesh->GetElementBinormal(0);
	switch (vertexBinormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexBinormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outBinormal.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outBinormal.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
			outBinormal.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
		}
			break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexBinormal->GetIndexArray().GetAt(inCtrlPointIndex);
			outBinormal.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[0]);
			outBinormal.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[1]);
			outBinormal.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[2]);
		}
			break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexBinormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outBinormal.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inVertexCounter).mData[0]);
			outBinormal.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inVertexCounter).mData[1]);
			outBinormal.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inVertexCounter).mData[2]);
		}
			break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexBinormal->GetIndexArray().GetAt(inVertexCounter);
			outBinormal.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[0]);
			outBinormal.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[1]);
			outBinormal.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[2]);
		}
			break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void GFBXMeshLoader::ReadTangent(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, D3DXVECTOR3& outTangent)
{
	if (inMesh->GetElementTangentCount() < 1)
	{
		throw std::exception("Invalid Tangent Number");
	}

	FbxGeometryElementTangent* vertexTangent = inMesh->GetElementTangent(0);
	switch (vertexTangent->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
			outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
		}
			break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexTangent->GetIndexArray().GetAt(inCtrlPointIndex);
			outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
		}
			break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[0]);
			outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[1]);
			outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[2]);
		}
			break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexTangent->GetIndexArray().GetAt(inVertexCounter);
			outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
		}
			break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

// This function removes the duplicated vertices and
// adjust the index buffer properly
// This function should take a while, though........
void GFBXMeshLoader::Optimize()
{
	
	// First get a list of unique vertices
	OutputDebugString(L"Optimize... list of unique vertices\n");
	std::vector<PNTIWVertex> uniqueVertices;
	for (unsigned int i = 0; i < mTriangles.size(); ++i)
	{
		for (unsigned int j = 0; j < 3; ++j)
		{
			// If current vertex has not been added to
			// our unique vertex list, then we add it
			int vertexIdx = i * 3 + j;
			if (FindVertex(mVertices[vertexIdx], uniqueVertices) == -1)
			{
				uniqueVertices.push_back(mVertices[i * 3 + j]);
			}
		}
	}

	// Now we update the index buffer
	OutputDebugString(L"Optimize... update the index buffer\n");
	for (unsigned int i = 0; i < mTriangles.size(); ++i)
	{
		for (unsigned int j = 0; j < 3; ++j)
		{
			mTriangles[i].mIndices[j] = FindVertex(mVertices[i * 3 + j], uniqueVertices);
		}
	}

	mVertices.clear();
	mVertices = uniqueVertices;
	uniqueVertices.clear();

	// Now we sort the triangles by materials to reduce 
	// shader's workload
	OutputDebugString(L"Optimize... sort the triangles\n");
	std::sort(mTriangles.begin(), mTriangles.end());
}

int GFBXMeshLoader::FindVertex(const PNTIWVertex& inTargetVertex, const std::vector<PNTIWVertex>& uniqueVertices)
{
	for (unsigned int i = 0; i < uniqueVertices.size(); ++i)
	{
		if (inTargetVertex == uniqueVertices[i])
		{
			return i;
		}
	}

	return -1;
}


bool GFBXMeshLoader::CopyToTargetAnimation(GFBX::Mesh* targetMesh)
{
	// initial bones
	for (unsigned int i = 0; i < mSkeleton.mJoints.size(); ++i)
	{
		FbxMatrix outGlobalBindposeInverse;
		FbxMatrix outGlobalBindpose;

		Bone bone;
		bone.boneName = mSkeleton.mJoints[i].mName;

		outGlobalBindposeInverse = mSkeleton.mJoints[i].mGlobalBindposeInverse;
		outGlobalBindpose = mSkeleton.mJoints[i].mGlobalBindpose;

		BoneKeyFrame keyframe;
		Utilities::ConvertToD3DXMatrix(outGlobalBindpose, keyframe.mGlobalBindpose);
		Utilities::ConvertToD3DXMatrix(outGlobalBindposeInverse, keyframe.mGlobalBindposeInverse);
		bone.frames.push_back(keyframe);
		targetMesh->m_Initialskeleton.bones.push_back(bone);
	}

	// animation
	targetMesh->m_Animationkeleton.animationName = mAnimationName;
	targetMesh->m_Animationkeleton.animationLength = mAnimationLength;
	for (unsigned int i = 0; i < mSkeleton.mJoints.size(); ++i)
	{
		Bone bone;
		bone.boneName = mSkeleton.mJoints[i].mName;
		FbxKeyframe* walker = mSkeleton.mJoints[i].mAnimation;
		while (walker)
		{
			int frameNum = walker->mFrameNum;
			FbxMatrix out = walker->mGlobalTransform;
			BoneKeyFrame keyframe;
			Utilities::ConvertToD3DXMatrix(out, keyframe.mGlobalBindpose);
			//Utilities::ConvertToD3DXMatrix(outGlobalBindposeInverse, keyframe.mGlobalBindposeInverse);
			bone.frames.push_back(keyframe);
			walker = walker->mNext;
		}
		targetMesh->m_Animationkeleton.bones.push_back(bone);
	}

	return true;
}

bool GFBXMeshLoader::CopyToTargetMesh(GFBX::MeshSubset* targetMesh)
{
	OutputDebugString(L"CopyToTargetMesh...\n");

	if (!targetMesh)
		return false;

	//if (mMaterialLookUp.size() > 0)
	//{
	//	targetMesh->m_texDiffuseName = mMaterialLookUp[0]->mDiffuseMapName;
	//	targetMesh->m_texNormalName = mMaterialLookUp[0]->mNormalMapName;
	//}
	
	//assert(mTriangleCount == mTriangles.size());
	//for (unsigned int i = 0; i < mTriangleCount; ++i)
	int a = mTriangles.size();

	for (unsigned int i = 0; i < mTriangles.size(); ++i)
	{
		targetMesh->m_indices.push_back(mTriangles[i].mIndices[0]);	//0?
		targetMesh->m_indices.push_back(mTriangles[i].mIndices[1]);	//2?
		targetMesh->m_indices.push_back(mTriangles[i].mIndices[2]);	//1?
	}

	for (unsigned int i = 0; i < mVertices.size(); ++i)
	{
		GFBX::Vertex v;
		// x,y,z,1
		v.pos = D3DXVECTOR3( mVertices[i].mPosition.x, mVertices[i].mPosition.y, mVertices[i].mPosition.z);
		v.norm = D3DXVECTOR3(mVertices[i].mNormal.x, mVertices[i].mNormal.y, mVertices[i].mNormal.z);
		v.uv = D3DXVECTOR2( mVertices[i].mUV.x, 1 - mVertices[i].mUV.y);
		v.tan = D3DXVECTOR3(mVertices[i].mTangent.x, mVertices[i].mTangent.y, mVertices[i].mTangent.z);
		v.binorm = D3DXVECTOR3(mVertices[i].mBinormal.x, mVertices[i].mBinormal.y, mVertices[i].mBinormal.z);
		if (mHasAnimation)
		{
			if(mVertices[i].mVertexBlendingInfos.size() >= 4)
			{
				v.weights = D3DXVECTOR4( 
					static_cast<float>(mVertices[i].mVertexBlendingInfos[0].mBlendingWeight),
					static_cast<float>(mVertices[i].mVertexBlendingInfos[1].mBlendingWeight),
					static_cast<float>(mVertices[i].mVertexBlendingInfos[2].mBlendingWeight),
					static_cast<float>(mVertices[i].mVertexBlendingInfos[3].mBlendingWeight));
				v.indices[0] = (BYTE)mVertices[i].mVertexBlendingInfos[0].mBlendingIndex;
				v.indices[1] = (BYTE)mVertices[i].mVertexBlendingInfos[1].mBlendingIndex;
				v.indices[2] = (BYTE)mVertices[i].mVertexBlendingInfos[2].mBlendingIndex;
				v.indices[3] = (BYTE)mVertices[i].mVertexBlendingInfos[3].mBlendingIndex;
			}
			else
			{
				OutputDebugString(L"Some mVertexBlendingInfos was wrong ... \n");
			}
			//char szTmp[512];
			//sprintf(szTmp, "%d => index : %d %d %d %d    weight : %f %f %f %f \n", i, 
			//	mVertices[i].mVertexBlendingInfos[0].mBlendingIndex,
			//	mVertices[i].mVertexBlendingInfos[1].mBlendingIndex,
			//	mVertices[i].mVertexBlendingInfos[2].mBlendingIndex,
			//	mVertices[i].mVertexBlendingInfos[3].mBlendingIndex,
			//	static_cast<float>(mVertices[i].mVertexBlendingInfos[0].mBlendingWeight),
			//	static_cast<float>(mVertices[i].mVertexBlendingInfos[1].mBlendingWeight),
			//	static_cast<float>(mVertices[i].mVertexBlendingInfos[2].mBlendingWeight),
			//	static_cast<float>(mVertices[i].mVertexBlendingInfos[3].mBlendingWeight));
			//OutputDebugStringA(szTmp);
		}
		targetMesh->m_verts.push_back(v);
	}

	return true;
}


void GFBXMeshLoader::AssociateMaterialToMesh(FbxNode* inNode)
{
	FbxLayerElementArrayTemplate<int>* materialIndices;
	FbxGeometryElement::EMappingMode materialMappingMode = FbxGeometryElement::eNone;
	FbxMesh* currMesh = inNode->GetMesh();

	if (currMesh->GetElementMaterial())
	{
		materialIndices = &(currMesh->GetElementMaterial()->GetIndexArray());
		materialMappingMode = currMesh->GetElementMaterial()->GetMappingMode();

		if (materialIndices)
		{
			switch (materialMappingMode)
			{
			case FbxGeometryElement::eByPolygon:
			{
				if (materialIndices->GetCount() == mTriangleCount)
				{
					for (unsigned int i = 0; i < mTriangles.size() /*mTriangleCount*/; ++i)
					{
						unsigned int materialIndex = materialIndices->GetAt(i);
						mTriangles[i].mMaterialIndex = materialIndex;
					}
				}
			}
				break;

			case FbxGeometryElement::eAllSame:
			{
				unsigned int materialIndex = materialIndices->GetAt(0);
				for (unsigned int i = 0; i < mTriangles.size()/*mTriangleCount*/; ++i)
				{
					mTriangles[i].mMaterialIndex = materialIndex;
				}
			}
				break;

			default:
				throw std::exception("Invalid mapping mode for material\n");
			}
		}
	}
}

void GFBXMeshLoader::ProcessMaterials(FbxNode* inNode)
{
	unsigned int materialCount = inNode->GetMaterialCount();

	for (unsigned int i = 0; i < materialCount; ++i)
	{
		FbxSurfaceMaterial* surfaceMaterial = inNode->GetMaterial(i);
		ProcessMaterialAttribute(surfaceMaterial, i);
		ProcessMaterialTexture(surfaceMaterial, mMaterialLookUp[i]);
	}
}

void GFBXMeshLoader::ProcessMaterialAttribute(FbxSurfaceMaterial* inMaterial, unsigned int inMaterialIndex)
{
	FbxDouble3 double3;
	FbxDouble double1;
	if (inMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
	{
		PhongMaterial* currMaterial = new PhongMaterial();

		// Amibent Color
		double3 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Ambient;
		currMaterial->mAmbient.x = static_cast<float>(double3.mData[0]);
		currMaterial->mAmbient.y = static_cast<float>(double3.mData[1]);
		currMaterial->mAmbient.z = static_cast<float>(double3.mData[2]);

		// Diffuse Color
		double3 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Diffuse;
		currMaterial->mDiffuse.x = static_cast<float>(double3.mData[0]);
		currMaterial->mDiffuse.y = static_cast<float>(double3.mData[1]);
		currMaterial->mDiffuse.z = static_cast<float>(double3.mData[2]);

		// Specular Color
		double3 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Specular;
		currMaterial->mSpecular.x = static_cast<float>(double3.mData[0]);
		currMaterial->mSpecular.y = static_cast<float>(double3.mData[1]);
		currMaterial->mSpecular.z = static_cast<float>(double3.mData[2]);

		// Emissive Color
		double3 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Emissive;
		currMaterial->mEmissive.x = static_cast<float>(double3.mData[0]);
		currMaterial->mEmissive.y = static_cast<float>(double3.mData[1]);
		currMaterial->mEmissive.z = static_cast<float>(double3.mData[2]);

		// Reflection
		double3 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Reflection;
		currMaterial->mReflection.x = static_cast<float>(double3.mData[0]);
		currMaterial->mReflection.y = static_cast<float>(double3.mData[1]);
		currMaterial->mReflection.z = static_cast<float>(double3.mData[2]);

		// Transparency Factor
		double1 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->TransparencyFactor;
		currMaterial->mTransparencyFactor = double1;

		// Shininess
		double1 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Shininess;
		currMaterial->mShininess = double1;

		// Specular Factor
		double1 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->SpecularFactor;
		currMaterial->mSpecularPower = double1;


		// Reflection Factor
		double1 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->ReflectionFactor;
		currMaterial->mReflectionFactor = double1;

		mMaterialLookUp[inMaterialIndex] = currMaterial;
	}
	else if (inMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		LambertMaterial* currMaterial = new LambertMaterial();

		// Amibent Color
		double3 = reinterpret_cast<FbxSurfaceLambert *>(inMaterial)->Ambient;
		currMaterial->mAmbient.x = static_cast<float>(double3.mData[0]);
		currMaterial->mAmbient.y = static_cast<float>(double3.mData[1]);
		currMaterial->mAmbient.z = static_cast<float>(double3.mData[2]);

		// Diffuse Color
		double3 = reinterpret_cast<FbxSurfaceLambert *>(inMaterial)->Diffuse;
		currMaterial->mDiffuse.x = static_cast<float>(double3.mData[0]);
		currMaterial->mDiffuse.y = static_cast<float>(double3.mData[1]);
		currMaterial->mDiffuse.z = static_cast<float>(double3.mData[2]);

		// Emissive Color
		double3 = reinterpret_cast<FbxSurfaceLambert *>(inMaterial)->Emissive;
		currMaterial->mEmissive.x = static_cast<float>(double3.mData[0]);
		currMaterial->mEmissive.y = static_cast<float>(double3.mData[1]);
		currMaterial->mEmissive.z = static_cast<float>(double3.mData[2]);

		// Transparency Factor
		double1 = reinterpret_cast<FbxSurfaceLambert *>(inMaterial)->TransparencyFactor;
		currMaterial->mTransparencyFactor = double1;

		mMaterialLookUp[inMaterialIndex] = currMaterial;
	}
}

void GFBXMeshLoader::ProcessMaterialTexture(FbxSurfaceMaterial* inMaterial, Material* ioMaterial)
{
	unsigned int textureIndex = 0;
	FbxProperty property;

	FBXSDK_FOR_EACH_TEXTURE(textureIndex)
	{
		property = inMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[textureIndex]);
		if (property.IsValid())
		{
			unsigned int textureCount = property.GetSrcObjectCount<FbxTexture>();
			for (unsigned int i = 0; i < textureCount; ++i)
			{
				FbxLayeredTexture* layeredTexture = property.GetSrcObject<FbxLayeredTexture>(i);
				if (layeredTexture)
				{
					throw std::exception("Layered Texture is currently unsupported\n");
				}
				else
				{
					FbxTexture* texture = property.GetSrcObject<FbxTexture>(i);
					if (texture)
					{
						std::string textureType = property.GetNameAsCStr();
						FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(texture);

						if (fileTexture)
						{
							if (textureType == "DiffuseColor")
							{
								ioMaterial->mDiffuseMapName = fileTexture->GetFileName();
							}
							else if (textureType == "SpecularColor")
							{
								ioMaterial->mSpecularMapName = fileTexture->GetFileName();
							}
							else if (textureType == "Bump")
							{
								ioMaterial->mNormalMapName = fileTexture->GetFileName();
							}
							else if (textureType == "NormalMap")
							{
								ioMaterial->mNormalMapName = fileTexture->GetFileName();
							}
						}
					}
				}
			}
		}
	}
}

void GFBXMeshLoader::PrintMaterial()
{
	for (auto itr = mMaterialLookUp.begin(); itr != mMaterialLookUp.end(); ++itr)
	{
		itr->second->WriteToStream(std::cout);
		std::cout << "\n\n";
	}
}

void GFBXMeshLoader::PrintTriangles()
{
	for (unsigned int i = 0; i < mTriangles.size(); ++i)
	{
		std::cout << "FbxTriangle# " << i + 1 << " Material Index: " << mTriangles[i].mMaterialIndex << "\n";
	}
}

void GFBXMeshLoader::CleanupFbxManager()
{
	mFBXScene->Destroy();
	mFBXScene = NULL;
	mFBXManager->Destroy();
	mFBXManager = NULL;

	mTriangles.clear();

	mVertices.clear();

	mSkeleton.mJoints.clear();

	for (auto itr = mMaterialLookUp.begin(); itr != mMaterialLookUp.end(); ++itr)
	{
		delete itr->second;
	}
	mMaterialLookUp.clear();
}
