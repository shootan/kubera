#include "DXUT.h"
#include "SDKmisc.h"
#include "GFBXMeshLoader.h"

GFBXMeshLoader* GFBXMeshLoader::inst = 0;
GFBXMeshLoader::GFBXMeshLoader()
{}
GFBXMeshLoader::~GFBXMeshLoader()
{}
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



HRESULT GFBXMeshLoader::OnCreateDevice(LPDIRECT3DDEVICE9 pd3dDevice)
{
	HRESULT hr;
	WCHAR s[MAX_PATH];
	V_RETURN(DXUTFindDXSDKMediaFileCch( s, MAX_PATH, 
		//L"tyrant_zombie/tyrant _zombie.fbx"));
		//L"BaseMale/3D/baseMale.fbx"));
		//L"Modern Weapons Pack/M4A1/FBX/M4A1 Sopmod.fbx"));
		//L"Free_Rocks/_models/cliff.fbx"));
		L"Meshes/SciFi_Fighter_AK5.FBX"));
	std::wstring wstr(s);
	std::string str("");
	str.assign( wstr.begin(), wstr.end());
	if (LoadFBX(str.c_str(), &m_verts) != S_OK)
		return S_FALSE;

	return S_OK;
}


HRESULT GFBXMeshLoader::OnDestroyDevice()
{

	return S_OK;
}

HRESULT GFBXMeshLoader::OnResetDevice(IDirect3DDevice9* pd3dDevice)
{
	return S_OK;
}

HRESULT GFBXMeshLoader::OnLostDevice()
{
	return S_OK;
}

void GFBXMeshLoader::OutputDebugVertices(FBXVERTS* pVerts)
{
	if (!pVerts)
		return;
	int nIdx = 0;
	FBXVERTS::iterator it = pVerts->begin();
	while ( it != pVerts->end()) {
		FBXVertex vertex = *it;
		// debug string
		char szTmp[256];
		sprintf(szTmp, "%d => pos : %f %f %f    norm : %f %f %f   uv : %f %f  \n", 
			nIdx, vertex.pos.x, vertex.pos.y, vertex.pos.z,
			vertex.norm.x, vertex.norm.y, vertex.norm.z, vertex.uv.x, vertex.uv.y);
		OutputDebugStringA(szTmp);
		it++;
	}
}

HRESULT GFBXMeshLoader::LoadPolygon(FbxMesh* pMesh, FbxVector4* pVertices, FbxGeometryElementUV* leUV,
	int PolyIdx, std::vector<FBXVertex>* pOutVertexVector)
{

	int iNumVertices = pMesh->GetPolygonSize(PolyIdx);
	if (iNumVertices == 3)
		OutputDebugStringA("This Mesh made in 3 Polygons");
	else if (iNumVertices == 4)
		OutputDebugStringA("This Mesh made in 4 Polygons");
	else
		assert("It is Only allowed mesh made in 3 or 4 polygons");



	pMesh->GenerateTangentsData(0);
	int iNumTangent = pMesh->GetElementTangentCount();
	int iNumBinormal = pMesh->GetElementBinormalCount();


	int l;
	int ElementPolygonGroupCount = pMesh->GetElementPolygonGroupCount();
	for (l = 0; l < pMesh->GetElementPolygonGroupCount(); l++)
	{
		FbxGeometryElementPolygonGroup* lePolgrp = pMesh->GetElementPolygonGroup(l);
		switch (lePolgrp->GetMappingMode())
		{
		case FbxGeometryElement::eByPolygon:
			if (lePolgrp->GetReferenceMode() == FbxGeometryElement::eIndex)
			{
				int polyGroupId = lePolgrp->GetIndexArray().GetAt(PolyIdx);
				break;
			}
		default:
			// any other mapping modes don't make sense
			OutputDebugStringA("        \"unsupported group assignment\"");
			break;
		}
	}

	//if (leNormals->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	//{
	//	char header[100];
	//	FBXSDK_sprintf(header, 100, "            Normal Vector: ");
	//	if (leNormals->GetReferenceMode() == FbxGeometryElement::eDirect)
	//		OutputDebugStringA("leNormals->GetDirectArray().GetAt(i)\");
	//}

	for (int k = 0; k < iNumVertices; k++)
	{
		FBXVertex vertex;
		// pos
		int iControlPointIndex = pMesh->GetPolygonVertex(PolyIdx, k);
		vertex.pos.x = (float)pVertices[iControlPointIndex].mData[0];
		vertex.pos.y = (float)pVertices[iControlPointIndex].mData[1];
		vertex.pos.z = (float)pVertices[iControlPointIndex].mData[2];
		// norm
		FbxVector4 fbxVecNorm;
		pMesh->GetPolygonVertexNormal(PolyIdx, k, fbxVecNorm);
		vertex.norm.x = fbxVecNorm.mData[0];
		vertex.norm.y = fbxVecNorm.mData[1];
		vertex.norm.z = fbxVecNorm.mData[2];
		// uv again
		int lTextureUVIndex = pMesh->GetTextureUVIndex(PolyIdx, k);
		FbxVector2 fbxVecUV = leUV->GetDirectArray().GetAt(lTextureUVIndex);
		vertex.uv.x = (float)fbxVecUV.mData[0];
		vertex.uv.y = 1 - (float)fbxVecUV.mData[1];
		// tangent

		pOutVertexVector->push_back(vertex);
	}
	//if (iNumVertices == 4){
	//	int vtCnt = pOutVertexVector->size();
	//	pOutVertexVector->push_back((*pOutVertexVector)[vtCnt - 1]);
	//	pOutVertexVector->push_back((*pOutVertexVector)[vtCnt - 3]);
	//}

	return S_OK;
}

#define MAT_HEADER_LENGTH 200

void DisplayMaterialTextureConnections(FbxSurfaceMaterial* pMaterial, char * header, int pMatId, int l);
void DisplayTextureNames(FbxProperty &pProperty, FbxString& pConnectionString);

void DisplayMaterialConnections(FbxMesh* pMesh)
{
	int i, l, lPolygonCount = pMesh->GetPolygonCount();

	char header[MAT_HEADER_LENGTH];

	OutputDebugStringA("    Polygons Material Connections");

	//check whether the material maps with only one mesh
	bool lIsAllSame = true;
	for (l = 0; l < pMesh->GetElementMaterialCount(); l++)
	{

		FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial(l);
		if (lMaterialElement->GetMappingMode() == FbxGeometryElement::eByPolygon)
		{
			lIsAllSame = false;
			break;
		}
	}

	//For eAllSame mapping type, just out the material and texture mapping info once
	if (lIsAllSame)
	{
		for (l = 0; l < pMesh->GetElementMaterialCount(); l++)
		{

			FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial(l);
			if (lMaterialElement->GetMappingMode() == FbxGeometryElement::eAllSame)
			{
				FbxSurfaceMaterial* lMaterial = pMesh->GetNode()->GetMaterial(lMaterialElement->GetIndexArray().GetAt(0));
				int lMatId = lMaterialElement->GetIndexArray().GetAt(0);
				if (lMatId >= 0)
				{
					OutputDebugStringA("        All polygons share the same material in mesh ");
					DisplayMaterialTextureConnections(lMaterial, header, lMatId, l);
				}
			}
		}

		//no material
		if (l == 0)
			OutputDebugStringA("        no material applied");
	}

	//For eByPolygon mapping type, just out the material and texture mapping info once
	else
	{
		for (i = 0; i < lPolygonCount; i++)
		{
			OutputDebugStringA("        Polygon ");

			for (l = 0; l < pMesh->GetElementMaterialCount(); l++)
			{

				FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial(l);
				FbxSurfaceMaterial* lMaterial = NULL;
				int lMatId = -1;
				lMaterial = pMesh->GetNode()->GetMaterial(lMaterialElement->GetIndexArray().GetAt(i));
				lMatId = lMaterialElement->GetIndexArray().GetAt(i);

				if (lMatId >= 0)
				{
					DisplayMaterialTextureConnections(lMaterial, header, lMatId, l);
				}
			}
		}
	}
}


void DisplayMaterialTextureConnections(FbxSurfaceMaterial* pMaterial, char * header, int pMatId, int l)
{
	if (!pMaterial)
		return;

	FbxString lConnectionString = "            Material %d -- ";
	//Show all the textures

	FbxProperty lProperty;
	//Diffuse Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
	DisplayTextureNames(lProperty, lConnectionString);

	//DiffuseFactor Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
	DisplayTextureNames(lProperty, lConnectionString);

	//Emissive Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
	DisplayTextureNames(lProperty, lConnectionString);

	//EmissiveFactor Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sEmissiveFactor);
	DisplayTextureNames(lProperty, lConnectionString);


	//Ambient Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
	DisplayTextureNames(lProperty, lConnectionString);

	//AmbientFactor Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sAmbientFactor);
	DisplayTextureNames(lProperty, lConnectionString);

	//Specular Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
	DisplayTextureNames(lProperty, lConnectionString);

	//SpecularFactor Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sSpecularFactor);
	DisplayTextureNames(lProperty, lConnectionString);

	//Shininess Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
	DisplayTextureNames(lProperty, lConnectionString);

	//Bump Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sBump);
	DisplayTextureNames(lProperty, lConnectionString);

	//Normal Map Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
	DisplayTextureNames(lProperty, lConnectionString);

	//Transparent Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sTransparentColor);
	DisplayTextureNames(lProperty, lConnectionString);

	//TransparencyFactor Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
	DisplayTextureNames(lProperty, lConnectionString);

	//Reflection Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sReflection);
	DisplayTextureNames(lProperty, lConnectionString);

	//ReflectionFactor Textures
	lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sReflectionFactor);
	DisplayTextureNames(lProperty, lConnectionString);

	//Update header with material info
	bool lStringOverflow = (lConnectionString.GetLen() + 10 >= MAT_HEADER_LENGTH); // allow for string length and some padding for "%d"
	if (lStringOverflow)
	{
		// Truncate string!
		lConnectionString = lConnectionString.Left(MAT_HEADER_LENGTH - 10);
		lConnectionString = lConnectionString + "...";
	}
	FBXSDK_sprintf(header, MAT_HEADER_LENGTH, lConnectionString.Buffer(), pMatId, l);
	OutputDebugStringA(header);
}

void DisplayTextureNames(FbxProperty &pProperty, FbxString& pConnectionString)
{
	int lLayeredTextureCount = pProperty.GetSrcObjectCount<FbxLayeredTexture>();
	if (lLayeredTextureCount > 0)
	{
		for (int j = 0; j<lLayeredTextureCount; ++j)
		{
			FbxLayeredTexture *lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
			int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
			pConnectionString += " Texture ";

			for (int k = 0; k<lNbTextures; ++k)
			{
				//lConnectionString += k;
				pConnectionString += "\"";
				pConnectionString += (char*)lLayeredTexture->GetName();
				pConnectionString += "\"";
				pConnectionString += " ";
			}
			pConnectionString += "of ";
			pConnectionString += pProperty.GetName();
			pConnectionString += " on layer ";
			pConnectionString += j;
		}
		pConnectionString += " |";
	}
	else
	{
		//no layered texture simply get on the property
		int lNbTextures = pProperty.GetSrcObjectCount<FbxTexture>();

		if (lNbTextures > 0)
		{
			pConnectionString += " Texture ";
			pConnectionString += " ";

			for (int j = 0; j<lNbTextures; ++j)
			{
				FbxTexture* lTexture = pProperty.GetSrcObject<FbxTexture>(j);
				if (lTexture)
				{
					pConnectionString += "\"";
					pConnectionString += (char*)lTexture->GetName();
					pConnectionString += "\"";
					pConnectionString += " ";
				}
			}
			pConnectionString += "of ";
			pConnectionString += pProperty.GetName();
			pConnectionString += " |";
		}
	}
}



HRESULT GFBXMeshLoader::LoadMesh(FbxMesh* pMesh, std::vector<FBXVertex>* pOutVertexVector)
{
	//get all UV set names
	FbxStringList lUVSetNameList;
	pMesh->GetUVSetNames(lUVSetNameList);
	int uvcount = lUVSetNameList.GetCount();
	if (uvcount < 1)
		return S_FALSE;
	const char* lUVSetName = lUVSetNameList.GetStringAt(uvcount - 1); //lUVSetIndex);
	const FbxGeometryElementUV* lUVElement = pMesh->GetElementUV(lUVSetName);
	if (!lUVElement)
		return S_FALSE;
	// only support mapping mode eByPolygonVertex and eByControlPoint
	if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
		lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
		return S_FALSE;
	//index array, where holds the index referenced to the uv data
	const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
	const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;
	// check 
	if (lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
		OutputDebugStringA("lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint \n");
	}
	else {
		OutputDebugStringA("lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex \n");
	}
	if (lUVElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
		OutputDebugStringA("lUVElement->GetReferenceMode() == FbxGeometryElement::eDirect");
	}
	else {
		OutputDebugStringA("lUVElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect");
	}

	int leUVcount = pMesh->GetElementUVCount();
	FbxGeometryElementUV* leUV = pMesh->GetElementUV(0);
	if (leUV->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
		OutputDebugStringA("leUV->GetMappingMode() == FbxGeometryElement::eByControlPoint \n");
	}
	else {
		OutputDebugStringA("leUV->GetMappingMode() == FbxGeometryElement::eByPolygonVertex \n");
	}
	if (leUV->GetReferenceMode() == FbxGeometryElement::eDirect) {
		OutputDebugStringA("leUV->GetReferenceMode() == FbxGeometryElement::eDirect");
	}
	else {
		OutputDebugStringA("leUV->GetReferenceMode() == FbxGeometryElement::eIndexToDirect");
	}

	FbxVector4* pVertices = pMesh->GetControlPoints();
	int ployCount = pMesh->GetPolygonCount();
	for (int j = 0; j < ployCount; j++)
	{
		LoadPolygon(pMesh, pVertices, leUV, j, pOutVertexVector);
	}

	OutputDebugVertices(pOutVertexVector);
}

HRESULT GFBXMeshLoader::LoadFBX(const char* filename, std::vector<FBXVertex>* pOutVertexVector)
{
	FbxManager* pFbxSdkManager = nullptr;
	if (pFbxSdkManager == nullptr)
	{
		pFbxSdkManager = FbxManager::Create();
		FbxIOSettings* pIOsettings = FbxIOSettings::Create(pFbxSdkManager, IOSROOT);
		pFbxSdkManager->SetIOSettings(pIOsettings);
	}

	FbxImporter* pImporter = FbxImporter::Create(pFbxSdkManager, "");
	FbxScene* pFbxScene = FbxScene::Create(pFbxSdkManager, "");

	bool bSuccess = pImporter->Initialize(filename, -1, pFbxSdkManager->GetIOSettings());
	if (!bSuccess) 
		return E_FAIL;
	bSuccess = pImporter->Import(pFbxScene);
	if (!bSuccess) 
		return E_FAIL;
	pImporter->Destroy();

	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();
	if (pFbxRootNode)
	{
		int childCount = pFbxRootNode->GetChildCount();
		for (int i = 0; i < childCount; i++)
		{
			FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);
			if (pFbxChildNode->GetNodeAttribute() == NULL)
				continue;
			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();
			if (AttributeType != FbxNodeAttribute::eMesh)
				continue;
			FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();
			this->LoadMesh(pMesh, pOutVertexVector);
			this->LoadTexture(pMesh);

			DisplayMaterialConnections(pMesh);
		}
	}

	if (pFbxSdkManager)
		pFbxSdkManager->Destroy();
	return S_OK;
}

HRESULT GFBXMeshLoader::LoadTextureInfo(FbxProperty pProperty, int pMaterialIndex)
{
	char szTmp[256];
	if(pProperty.IsValid())
	{
		int lTextureCount = pProperty.GetSrcObjectCount<FbxTexture>();
		sprintf(szTmp, "texture count : %d \n", lTextureCount);
		OutputDebugStringA(szTmp);
		for (int j = 0; j < lTextureCount; ++j)
		{
			//Here we have to check if it's layeredtextures, or just textures:
			FbxLayeredTexture *lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
			if (lLayeredTexture)
			{
				FbxLayeredTexture *lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
				int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
				for (int k = 0; k<lNbTextures; ++k)
				{
					FbxFileTexture* pTexture = lLayeredTexture->GetSrcObject<FbxFileTexture>(k);
					if (pTexture)
					{
						m_texDiffuseName = std::string(pTexture->GetFileName());
						
						sprintf(szTmp, "We do not handle multi layer texture. %s\n",
							m_texDiffuseName.c_str());
						OutputDebugStringA(szTmp);
					}

				}
			}
			else
			{
				//no layered texture simply get on the property
				FbxFileTexture * pTexture = pProperty.GetSrcObject<FbxFileTexture>(j);
				if (pTexture)
				{
					m_texDiffuseName = std::string(pTexture->GetFileName());
					sprintf(szTmp, "%s => %s \n", 
						pTexture->GetName(), pTexture->GetFileName());
					OutputDebugStringA(szTmp);
				}
			}
		}
	}//end if pProperty

	return S_OK;
}

HRESULT GFBXMeshLoader::LoadTexture(FbxGeometry* pGeometry)
{
	char szTmp[2048];
	int lMaterialCount = 0;
	FbxNode* lNode = NULL;
	if (pGeometry){
		lNode = pGeometry->GetNode();
		if (lNode)
			lMaterialCount = lNode->GetMaterialCount();
	}

	if (lMaterialCount > 0)
	{
		FbxPropertyT<FbxDouble3> lKFbxDouble3;
		FbxPropertyT<FbxDouble> lKFbxDouble1;
		FbxColor theColor;
		for (int lCount = 0; lCount < lMaterialCount; lCount++)
		{
			FbxSurfaceMaterial *lMaterial = lNode->GetMaterial(lCount);

			if (lMaterial){

				int lTextureIndex;
				FbxProperty lProperty;
				FBXSDK_FOR_EACH_TEXTURE(lTextureIndex)
				{
					lProperty = lMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[lTextureIndex]);
					int nDiffuseTexture = FbxLayerElement::eTextureDiffuse - FbxLayerElement::sTypeTextureStartIndex;
					
					sprintf(szTmp, "sTextureChannelNames[%d] = %s \n",
						lTextureIndex, FbxLayerElement::sTextureChannelNames[lTextureIndex]);
					OutputDebugStringA(szTmp);

					//lProperty = lMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[nDiffuseTexture]);
					LoadTextureInfo(lProperty, lCount);
					//int nNormalTexture = FbxLayerElement::eTextureNormalMap - FbxLayerElement::sTypeTextureStartIndex;
					//lProperty = lMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[nNormalTexture]);
					//LoadTextureInfo(lProperty, lCount);

				}

				//const char* diffuseName = FbxLayerElement::sTextureNames[FbxLayerElement::eTextureDiffuse];

				//FbxProperty lProperty = lMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[FbxLayerElement::eTextureDiffuse]);
				//FindAndDisplayTextureInfoByProperty(lProperty, lDisplayHeader, lMaterialIndex);

				
				//sprintf(szTmp, "%d => diffuse : %s \n",
				//	lCount, diffuseName);
				//OutputDebugStringA(szTmp);

			}//end if(lMaterial)

		}
	}


	return S_OK;
}


bool GFBXMeshLoader::ComputeBoundingSphereFromList(D3DXVECTOR3& center, float& radius)
{
	return ComputeBoundingSphereFromList(center, radius, m_verts);
}

bool GFBXMeshLoader::ComputeBoundingSphereFromList(D3DXVECTOR3& center, float& radius, FBXVERTS& verts)
{
	if (verts.size() <= 0)
	{
		center = D3DXVECTOR3(0, 0, 0);
		radius = 0;
		return false;
	}

	double dx, dy, dz;
	double rad_sq, xspan, yspan, zspan, maxspan;
	double old_to_p, old_to_p_sq, old_to_new;

	D3DXVECTOR3 xmin, xmax, ymin, ymax, zmin, zmax, dia1, dia2;


	// FIRST PASS: find 6 minima/maxima points
	xmin.x = ymin.y = zmin.z = FLT_MAX; // initialize for min/max compare 
	xmax.x = ymax.y = zmax.z = -FLT_MAX;

	for (size_t i = 0; i<verts.size(); i++)
	{
		const D3DXVECTOR3 & caller_p = verts[i].pos;

		// his ith point.
		if (caller_p.x<xmin.x)
			xmin = caller_p;
		if (caller_p.x>xmax.x)
			xmax = caller_p;
		if (caller_p.y<ymin.y)
			ymin = caller_p;
		if (caller_p.y>ymax.y)
			ymax = caller_p;
		if (caller_p.z<zmin.z)
			zmin = caller_p;
		if (caller_p.z>zmax.z)
			zmax = caller_p;
	}
	// Set xspan = distance between the 2 points xmin & xmax (squared)
	dx = xmax.x - xmin.x;
	dy = xmax.y - xmin.y;
	dz = xmax.z - xmin.z;
	xspan = dx*dx + dy*dy + dz*dz;
	// Same for y & z spans 
	dx = ymax.x - ymin.x;
	dy = ymax.y - ymin.y;
	dz = ymax.z - ymin.z;
	yspan = dx*dx + dy*dy + dz*dz;
	dx = zmax.x - zmin.x;
	dy = zmax.y - zmin.y;
	dz = zmax.z - zmin.z;
	zspan = dx*dx + dy*dy + dz*dz;

	// Set points dia1 & dia2 to the maximally seperated pair
	dia1 = xmin; dia2 = xmax; // assume xspan biggest
	maxspan = xspan;
	if (yspan>maxspan)
	{
		maxspan = yspan;
		dia1 = ymin; dia2 = ymax;
	}
	if (zspan>maxspan)
	{
		dia1 = zmin; dia2 = zmax;
	}

	// dia1,dia2 is a diameter of initial sphere
	// calc initial center
	center.x = (dia1.x + dia2.x) / 2.0f;
	center.y = (dia1.y + dia2.y) / 2.0f;
	center.z = (dia1.z + dia2.z) / 2.0f;
	// calculate initial radius**2 and radius
	dx = dia2.x - center.x;
	dy = dia2.y - center.y;
	dz = dia2.z - center.z;
	rad_sq = dx*dx + dy*dy + dz*dz;
	radius = sqrt(rad_sq);

	// SECOND PASS: increment current sphere
	for (size_t i = 0; i<verts.size(); i++)
	{
		const D3DXVECTOR3 & caller_p = verts[i].pos;
		// with his ith point.
		dx = caller_p.x - center.x;
		dy = caller_p.y - center.y;
		dz = caller_p.z - center.z;
		old_to_p_sq = dx*dx + dy*dy + dz*dz;
		if (old_to_p_sq > rad_sq) 	// do r**2 test first
		{
			// this point is outside of current sphere
			old_to_p = sqrt(old_to_p_sq);
			// calc radius of new sphere
			radius = (radius + old_to_p) / 2.0;
			rad_sq = radius * radius; 	// for next r**2 compare
			old_to_new = old_to_p - radius;
			// calc center of new sphere
			center.x = (float)((radius*center.x + old_to_new*caller_p.x) / old_to_p);
			center.y = (float)((radius*center.y + old_to_new*caller_p.y) / old_to_p);
			center.z = (float)((radius*center.z + old_to_new*caller_p.z) / old_to_p);
			//printf("\n New sphere: cen,rad = %f %f %f   %f", cen.x,cen.y,cen.z, rad);
		}
	}
}