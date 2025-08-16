#pragma once
#include <vector>
#include "fbxsdk.h"
#include "Windows/BufferViewer.h"

using namespace std;

//定义保存读取到的csv数据的结构体，目前只包含vertices、normals、texCoords、UV
struct CSVGeometry
{
  vector<uint> indexs;
  vector<FbxVector4> vertices;
  vector<FbxVector4> normals;
  vector<FbxVector4> color;
  vector<FbxVector2> texCoords;
  vector<FbxVector2> texCoords1;
  vector<FbxVector2> texCoords2;
  vector<FbxVector2> texCoords3;
  vector<FbxVector4> tangents;
  vector<FbxVector4> BoneIndexs;
  vector<FbxVector4> skinWeights;
  vector<uint64_t> shaderResIds;
  // TODO 顶点色

};

enum ExportMeshType
{
  VS_In = 0,
  VS_Out = 1,
  DS = 2
};

//导出Mesh设置
struct ExportMeshData
{
  QString savePath;
  uint32_t startActionId;
  uint32_t endActionId;
  FbxMatrix inverseMat;
  QMap<int, QString> meshSetting;
  bool bExportTexture;
  bool bReverseTraiangle;
  ExportMeshType exportType;
  bool bMergeMesh;
  bool bSeparateShaderId;
};


enum MeshDataLayer
{
  NORMAL  = 0,
  TANGENT = 1,
  UV      = 2,
  UV1     = 3,
  UV2     = 4,
  UV3     = 5,
  COLOR   = 6,
  VERTICE = 7,
  SKINWEIGHT = 8,
};

class FBXManager
{
public:

  FBXManager();
  // CSVGeometry& GenGeometry(const BufferViewer* model);
  // Generate FBX
  bool CreateScene(FbxScene* pScene, char* pSampleFileName, const CSVGeometry& cSVGeometry);
  FbxNode* CreateMesh(FbxScene* pScene, const char* pName,const  CSVGeometry& cSVGeometry);
  void CreateMaterials(FbxScene* pScene, FbxMesh* Mesh, const  CSVGeometry& cSVGeometry);
  void CreateMaterialsMap(const  CSVGeometry& cSVGeometry);
  void SetMeshDefaultPosition(FbxNode* pTriangle);
  int OutputFBX(const char* myOutPut, const CSVGeometry& cSVGeometry, bool bReverseTriangle);

private:
  CSVGeometry m_Output;
  map<uint64_t, int> m_ShaderIndexMap;
  bool m_bReverseTriangle;
};
