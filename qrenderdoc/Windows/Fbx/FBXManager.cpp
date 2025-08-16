#include "FBXManager.h"
#include <iostream>
#include <fbxsdk.h>
#include <cassert>
#include <set>
#include <string>
#include "FBXCommon.h"

// CSVGeometry& FBXManager::GenGeometry(const BufferViewer* model)
// {
//   
// }

FBXManager::FBXManager()
{
}

bool FBXManager::CreateScene(FbxScene *pScene, char *pSampleFileName,
                             const CSVGeometry &cSVGeometry)
{
  FbxNode* lMesh = CreateMesh(pScene, "Mesh", cSVGeometry);

  SetMeshDefaultPosition(lMesh);

  // Build the node tree.
  FbxNode* lRootNode = pScene->GetRootNode();

  lRootNode->AddChild(lMesh);

  return true;
}

FbxNode * FBXManager::CreateMesh(FbxScene *pScene, const char *pName,
    const CSVGeometry &cSVGeometry)
{
  FbxMesh* lMesh = FbxMesh::Create(pScene, pName);

	assert(cSVGeometry.vertices.size() > 0, "vertices is null");

  // Set material mapping.
  FbxGeometryElementMaterial* lMaterialElement = lMesh->CreateElementMaterial();
  lMaterialElement->SetMappingMode(FbxLayerElement::eByPolygon);
  lMaterialElement->SetReferenceMode(FbxLayerElement::eIndexToDirect);
  CreateMaterialsMap(cSVGeometry);

  // // Prepare Vertex
  // QList<FbxVector4> Vertiecs;
  // vector<uint> Indexs = cSVGeometry.indexs;
  // QMap<uint, FbxVector4> VectorIndexMap;
  // uint MinIndex = INT32_MAX;
  // for (int i = 0; i < Indexs.size(); i++)
  // {
  //   MinIndex = MinIndex > Indexs[i]? Indexs[i]: MinIndex;
  // }
  // for (int i = 0; i < Indexs.size(); i++)
  // {
  //   Indexs[i] = Indexs[i] - MinIndex;
  //   if(!VectorIndexMap.contains(Indexs[i]))
  //   {
  //     VectorIndexMap.insert(Indexs[i], cSVGeometry.vertices[i]);
  //   }
  // }
  //
  // Vertiecs = VectorIndexMap.values();
  
	// Create control points.
	lMesh->InitControlPoints(cSVGeometry.vertices.size());
	FbxVector4* lControlPoints = lMesh->GetControlPoints();

	for (int i = 0; i < cSVGeometry.vertices.size(); i++)
	{
		lControlPoints[i] = cSVGeometry.vertices[i];
	}
  int MaterialIndex = 0;
  uint64_t shaderId = 0;
  for (int i = 0; i < cSVGeometry.vertices.size() / 3; i++)
	{
    if(i >= cSVGeometry.shaderResIds.size())
      break;
    MaterialIndex = m_ShaderIndexMap[cSVGeometry.shaderResIds[i]];
		lMesh->BeginPolygon(MaterialIndex);
    if(m_bReverseTriangle)
    {
      lMesh->AddPolygon(3 * i + 2);
      lMesh->AddPolygon(3 * i + 1);
      lMesh->AddPolygon(3 * i + 0);
    }
    else
    {
      lMesh->AddPolygon(3 * i + 0);
      lMesh->AddPolygon(3 * i + 1);
      lMesh->AddPolygon(3 * i + 2);
    }
		lMesh->EndPolygon();
	}
  
	// 法向量
	FbxLayer* lNormalLayer = lMesh->GetLayer((int)MeshDataLayer::NORMAL);
	if (lNormalLayer == NULL)
	{
		int index = lMesh->CreateLayer();
		printf_s("normal:" + index);
		lNormalLayer = lMesh->GetLayer((int)MeshDataLayer::NORMAL);
	}

	FbxLayerElementNormal* lLayerElementNormal = FbxLayerElementNormal::Create(lMesh, "");
	lLayerElementNormal->SetMappingMode(FbxLayerElement::eByControlPoint);
  
	lLayerElementNormal->SetReferenceMode(FbxLayerElement::eDirect);

	for (auto iter = cSVGeometry.normals.begin(); iter != cSVGeometry.normals.end(); iter++)
	{
		lLayerElementNormal->GetDirectArray().Add(*iter);
	}

	lNormalLayer->SetNormals(lLayerElementNormal);

	// 添加切线
	FbxLayer* lTangentLayer = lMesh->GetLayer((int)MeshDataLayer::TANGENT);
	if (lTangentLayer == NULL)
	{
		int index = lMesh->CreateLayer();
		printf_s("tangent:" + index);
		lTangentLayer = lMesh->GetLayer((int)MeshDataLayer::TANGENT);
	}

	FbxLayerElementTangent* lLayerElementTangent = FbxLayerElementTangent::Create(lMesh, "");
	lLayerElementTangent->SetMappingMode(FbxLayerElement::eByControlPoint);
	lLayerElementTangent->SetReferenceMode(FbxLayerElement::eDirect);

	for (auto iter = cSVGeometry.tangents.begin(); iter != cSVGeometry.tangents.end(); iter++)
	{
		lLayerElementTangent->GetDirectArray().Add(*iter);
	}

	lTangentLayer->SetTangents(lLayerElementTangent);

	// 添加uv
	FbxLayer* lUVLayer = lMesh->GetLayer((int)MeshDataLayer::UV);
	if (lUVLayer == NULL)
	{
		int index = lMesh->CreateLayer();
		printf_s("uv:"+index);
		lUVLayer = lMesh->GetLayer((int)MeshDataLayer::UV);
	}

	FbxLayerElementUV* lLayerElementUV = FbxLayerElementUV::Create(lMesh, "TEXCOORD0");
	lLayerElementUV->SetMappingMode(FbxLayerElement::eByControlPoint);
	lLayerElementUV->SetReferenceMode(FbxLayerElement::eDirect);

	for (auto iter = cSVGeometry.texCoords.begin(); iter != cSVGeometry.texCoords.end(); iter++)
	{
		FbxVector2 data(iter->mData[0], iter->mData[1]);
		lLayerElementUV->GetDirectArray().Add(data);
	}

	lUVLayer->SetUVs(lLayerElementUV);

	// 添加uv1
	FbxLayer* lUV1Layer = lMesh->GetLayer((int)MeshDataLayer::UV1);
	if (lUV1Layer == NULL)
	{
		int index = lMesh->CreateLayer();
		printf_s("uv1:" + index);
		lUV1Layer = lMesh->GetLayer((int)MeshDataLayer::UV1);
	}

	FbxLayerElementUV* lLayerElementUV1 = FbxLayerElementUV::Create(lMesh, "TEXCOORD1");
	lLayerElementUV1->SetMappingMode(FbxLayerElement::eByControlPoint);
	lLayerElementUV1->SetReferenceMode(FbxLayerElement::eDirect);

	for (auto iter = cSVGeometry.texCoords1.begin(); iter != cSVGeometry.texCoords1.end(); iter++)
	{
		FbxVector2 data(iter->mData[0], iter->mData[1]);
		lLayerElementUV1->GetDirectArray().Add(data);
	}

	lUV1Layer->SetUVs(lLayerElementUV1);

  // 添加uv2
  FbxLayer* lUV2Layer = lMesh->GetLayer((int)MeshDataLayer::UV2);
  if (lUV2Layer == NULL)
  {
    int index = lMesh->CreateLayer();
    printf_s("uv1:" + index);
    lUV2Layer = lMesh->GetLayer((int)MeshDataLayer::UV2);
  }

  FbxLayerElementUV* lLayerElementUV2 = FbxLayerElementUV::Create(lMesh, "TEXCOORD2");
  lLayerElementUV2->SetMappingMode(FbxLayerElement::eByControlPoint);
  lLayerElementUV2->SetReferenceMode(FbxLayerElement::eDirect);

  for (auto iter = cSVGeometry.texCoords2.begin(); iter != cSVGeometry.texCoords2.end(); iter++)
  {
    FbxVector2 data(iter->mData[0], iter->mData[1]);
    lLayerElementUV2->GetDirectArray().Add(data);
  }

  lUV2Layer->SetUVs(lLayerElementUV2);

  // 添加uv3
  FbxLayer* lUV3Layer = lMesh->GetLayer((int)MeshDataLayer::UV3);
  if (lUV3Layer == NULL)
  {
    int index = lMesh->CreateLayer();
    printf_s("uv1:" + index);
    lUV3Layer = lMesh->GetLayer((int)MeshDataLayer::UV3);
  }

  FbxLayerElementUV* lLayerElementUV3 = FbxLayerElementUV::Create(lMesh, "TEXCOORD3");
  lLayerElementUV3->SetMappingMode(FbxLayerElement::eByControlPoint);
  lLayerElementUV3->SetReferenceMode(FbxLayerElement::eDirect);

  for (auto iter = cSVGeometry.texCoords3.begin(); iter != cSVGeometry.texCoords3.end(); iter++)
  {
    FbxVector2 data(iter->mData[0], iter->mData[1]);
    lLayerElementUV3->GetDirectArray().Add(data);
  }

  lUV3Layer->SetUVs(lLayerElementUV3);


  
	FbxLayer* lColorLayer = lMesh->GetLayer((int)MeshDataLayer::COLOR);
	if (lColorLayer == NULL)
	{
		int index = lMesh->CreateLayer();
		printf_s("color:" + index);
		lColorLayer = lMesh->GetLayer((int)MeshDataLayer::COLOR);
	}
	
	FbxLayerElementVertexColor* lLayerElementVertexColor = FbxLayerElementVertexColor::Create(lMesh, "COLOR");
	lColorLayer->SetVertexColors(lLayerElementVertexColor);
	
	for (auto iter = cSVGeometry.color.begin(); iter != cSVGeometry.color.end(); iter++)
	{
		lLayerElementVertexColor->GetDirectArray().Add(*iter);
	}
	lColorLayer->SetVertexColors(lLayerElementVertexColor);
  

	FbxNode* lNode = FbxNode::Create(pScene, pName);
	lNode->SetNodeAttribute(lMesh);

  CreateMaterials(pScene, lMesh, cSVGeometry);

  // 添加蒙皮权重
  if(cSVGeometry.BoneIndexs.size() > 0)
  {
    set<uint> fbxBones;
    int maxBoneIndex = 0;
    uint BoneIndex;
    for (auto iter = cSVGeometry.BoneIndexs.begin(); iter != cSVGeometry.BoneIndexs.end(); iter++)
    {
      for (int j = 0; j < 4; j++)
      {
        BoneIndex = (uint)iter->mData[0];
        maxBoneIndex = maxBoneIndex > BoneIndex? maxBoneIndex: BoneIndex;
        fbxBones.insert(BoneIndex);
      }
    }
    if(maxBoneIndex > 1)
    {
      vector<FbxCluster*> clusters(maxBoneIndex, NULL);
      for (std::set<uint>::iterator it = fbxBones.begin(); it != fbxBones.end(); ++it)
      {
        FbxCluster* pCluster = FbxCluster::Create(pScene, "");
        pCluster->SetLink(lNode);
        pCluster->SetLinkMode(FbxCluster::eTotalOne);
        clusters[*it] = pCluster;
      }

      for (int i = 0; i < cSVGeometry.indexs.size(); i++)
      {
        for (int j = 0; j < 4; j++)
        {
          int bone = (uint)cSVGeometry.BoneIndexs[i].mData[j];
          int vecticeIndex = (uint)cSVGeometry.indexs[i];
          double weight = cSVGeometry.skinWeights[i].mData[j];
          if(clusters[bone])
          {
            // clusters[bone]->AddControlPointIndex(vecticeIndex, weight);
          }
        }
      }

      FbxSkin* lSkin = FbxSkin::Create(pScene, "");

      FbxScene* p = lNode->GetScene();
      // FbxAMatrix modelMatrix = lNode->EvaluateGlobalTransform();
      for (uint32_t i = 0; i < clusters.size(); i++) {
        // clusters[i]->SetTransformMatrix(modelMatrix);
        // FbxAMatrix boneMatrix = fbxBones[i]->EvaluateGlobalTransform();
        // clusters[i]->SetTransformLinkMatrix(boneMatrix);
        lSkin->AddCluster(clusters[i]);
      }
    }
  }

  
	return lNode;
}

void FBXManager::CreateMaterials(FbxScene* pScene, FbxMesh* Mesh, const CSVGeometry &cSVGeometry)
{
  set<uint64_t> MaterialIds;
  for (int i = 0; i < cSVGeometry.shaderResIds.size(); i++)
  {
      MaterialIds.insert(cSVGeometry.shaderResIds[i]);
  }

  int index = 0;
  for(auto iter = MaterialIds.begin(); iter != MaterialIds.end(); ++iter)
  {
    m_ShaderIndexMap.insert(pair<uint64_t, int>(*iter, index));
    
    FbxString lMaterialName = "material";
    FbxString lShadingName = "Phong";
    lMaterialName += (int)*iter;
    FbxDouble3 lBlack(0.0, 0.0, 0.0);
    FbxDouble3 lRed(1.0, 0.0, 0.0);
    FbxDouble3 lColor;
    FbxSurfacePhong *lMaterial = FbxSurfacePhong::Create(pScene, lMaterialName.Buffer());
    
    
    // Generate primary and secondary colors.
    lMaterial->Emissive.Set(lBlack);
    lMaterial->Ambient.Set(lRed);
    lColor = FbxDouble3(index > 2   ? 1.0 : 0.0, 
        index > 0 && index < 4 ? 1.0 : 0.0, 
        index % 2   ? 0.0 : 1.0);
    lMaterial->Diffuse.Set(lColor);
    lMaterial->TransparencyFactor.Set(0.0);
    lMaterial->ShadingModel.Set(lShadingName);
    lMaterial->Shininess.Set(0.5);

    index++;
    //get the node of mesh, add material for it.
    FbxNode* lNode = Mesh->GetNode();
    if(lNode)             
      lNode->AddMaterial(lMaterial);
  }
}

void FBXManager::CreateMaterialsMap(const CSVGeometry &cSVGeometry)
{
  set<uint64_t> MaterialIds;
  for (int i = 0; i < cSVGeometry.shaderResIds.size(); i++)
  {
      MaterialIds.insert(cSVGeometry.shaderResIds[i]);
  }
  int index = 0;
  for(auto iter = MaterialIds.begin(); iter != MaterialIds.end(); ++iter)
  {
    m_ShaderIndexMap.insert(pair<uint64_t, int>(*iter, index));
    index++;
  }
}

void FBXManager::SetMeshDefaultPosition(FbxNode *pTriangle)
{
  pTriangle->LclTranslation.Set(FbxVector4(0.0, 0.0, 0.0));
  pTriangle->LclRotation.Set(FbxVector4(0.0, 0.0, 0.0));
  pTriangle->LclScaling.Set(FbxVector4(1.0, 1.0, 1.0));
}

int FBXManager::OutputFBX(const char *myOutPut, const CSVGeometry &cSVGeometry, bool bReverseTriangle)
{
  string lmyOutput = myOutPut;
  m_bReverseTriangle = bReverseTriangle;
  // size_t myNameSize = strlen(lmyOutput.c_str());
  //
  // if (strcmp(lmyOutput.c_str() + myNameSize - 4, ".fbx"))
  // {
  //   return 0;
  // }
  char* llmyOutput = const_cast<char *>(lmyOutput.c_str());

  FbxManager* lSdkManager = NULL;
  FbxScene* lScene = NULL;
  char* lSampleFileName = llmyOutput;
  bool lResult;

  // Prepare the FBX SDK.
  InitializeSdkObjects(lSdkManager, lScene);

  // Create the scene.
  lResult = CreateScene(lScene, lSampleFileName, cSVGeometry);

  if (lResult == false)
  {
    printf_s("\n\nAn error occurred while creating the scene...\n");
    DestroySdkObjects(lSdkManager, lResult);
    return 0;
  }

  // Save the scene.
  lResult = SaveScene(lSdkManager, lScene, lSampleFileName);

  if (lResult == false)
  {
    printf_s("\n\nAn error occurred while saving the scene...\n");
    DestroySdkObjects(lSdkManager, lResult);
    return 0;
  }

  // Destroy all objects created by the FBX SDK.
  DestroySdkObjects(lSdkManager, lResult);

  return 0;
}
