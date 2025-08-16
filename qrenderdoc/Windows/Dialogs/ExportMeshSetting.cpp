#include "ExportMeshSetting.h"
#include <QColorDialog>
#include <QFileInfo>
#include "Code/QRDUtils.h"

#include "ui_ExportMeshSetting.h"

ExportMeshSetting::ExportMeshSetting(const QStringList& VSInMeshDataItems, const QStringList& VSOutMeshDataItems, const QStringList& DSMeshDataItems,MainWindow *parent)
  :m_VSInMeshDataItems(VSInMeshDataItems),m_VSOutMeshDataItems(VSOutMeshDataItems),m_DSMeshDataItems(DSMeshDataItems), m_Parent(parent),ui(new Ui::ExportMeshSetting)
{
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  
  // setWindowFlags( windowFlags() & ~Qt::WindowStaysOnTopHint); //取消置顶
  this->setWindowFlag(Qt::WindowStaysOnTopHint,false);
  show();
  ui->setupUi(this);
  // setWindowFlags(windowFlags());
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  
  ui->filename->setFont(Formatter::PreferredFont());
  ui->blackLabel->setFont(Formatter::PreferredFont());
  ui->whiteLabel->setFont(Formatter::PreferredFont());

  RefreshMeshSetting(VSInMeshDataItems);

  m_bIsNeedExportTexture = true;
  m_bIsReverseTriangle = false;
  m_ExportType = ExportMeshType::VS_In;
  m_bMergeMesh = true;
  

  ui->reverseTriangle->setChecked(false);
  ui->ExportTexCheckBox->setChecked(true);
  ui->VSInBtn->setChecked(true);
  ui->VSOutBtn->setChecked(false);
  ui->MergeMeshCheckBox->setChecked(true);
  m_MinActionId = 0;
  m_MaxActionId = 0;

  ui->separateShaderIdCheckBox->setHidden(true);
  ui->skinWeightlabel->setHidden(true);
  ui->skinWeightCbx->setHidden(true);
  ui->label->setHidden(true);
  ui->label_2->setHidden(true);
  ui->InverseMat->setHidden(true);
}

void ExportMeshSetting::RefreshMeshSetting(const QStringList &MeshDataItems)
{
  ui->verticesCbx->clear();
  ui->normalCbx->clear();
  ui->texCoordCbx->clear();
  ui->texCoord1Cbx->clear();
  ui->texCoord2Cbx->clear();
  ui->texCoord3Cbx->clear();
  ui->TangentsCbx->clear();
  ui->skinWeightCbx->clear();
  ui->ColorCbx->clear();
  
  ui->verticesCbx->addItems(MeshDataItems);
  ui->normalCbx->addItems(MeshDataItems);
  ui->texCoordCbx->addItems(MeshDataItems);
  ui->texCoord1Cbx->addItems(MeshDataItems);
  ui->texCoord2Cbx->addItems(MeshDataItems);
  ui->texCoord3Cbx->addItems(MeshDataItems);
  ui->TangentsCbx->addItems(MeshDataItems);
  ui->skinWeightCbx->addItems(MeshDataItems);
  ui->ColorCbx->addItems(MeshDataItems);

  m_MeshSettingMap.clear();
  m_MeshSettingMap.insert(MeshDataLayer::VERTICE, MeshDataItems[0]);
  m_MeshSettingMap.insert(MeshDataLayer::NORMAL, MeshDataItems[0]);
  m_MeshSettingMap.insert(MeshDataLayer::COLOR, MeshDataItems[0]);
  m_MeshSettingMap.insert(MeshDataLayer::UV, MeshDataItems[0]);
  m_MeshSettingMap.insert(MeshDataLayer::UV1, MeshDataItems[0]);
  m_MeshSettingMap.insert(MeshDataLayer::UV2, MeshDataItems[0]);
  m_MeshSettingMap.insert(MeshDataLayer::UV3, MeshDataItems[0]);
  m_MeshSettingMap.insert(MeshDataLayer::SKINWEIGHT, MeshDataItems[0]);
  m_MeshSettingMap.insert(MeshDataLayer::TANGENT, MeshDataItems[0]);
}

QString ExportMeshSetting::GetMeshSettingItem(int index)
{
  QString str;
  if(m_ExportType == ExportMeshType::VS_In)
  {
    if(index < m_VSInMeshDataItems.count())
    {
      str = m_VSInMeshDataItems[index];
    }
  }
  else if(m_ExportType == ExportMeshType::VS_Out)
  {
    if(index < m_VSOutMeshDataItems.count())
    {
      str = m_VSOutMeshDataItems[index];
    }
  }
  else
  {
    if(index < m_DSMeshDataItems.count())
    {
      str = m_DSMeshDataItems[index];
    }
  }
  return str;
}

ExportMeshSetting::~ExportMeshSetting()
{
  delete ui;
}

void ExportMeshSetting::on_filename_textEdited(const QString &arg1)
{
  
}

void ExportMeshSetting::on_browse_clicked()
{
  m_SavePath = QFileDialog::getExistingDirectory(this, tr("选择保存位置"));
  ui->filename->setText(m_SavePath);
}

void ExportMeshSetting::on_ExportTexCheckBox_toggled()
{
  m_bIsNeedExportTexture = ui->ExportTexCheckBox->isChecked();
}

void ExportMeshSetting::on_reverseTriangle_toggled()
{
  m_bIsReverseTriangle = ui->reverseTriangle->isChecked();
}

void ExportMeshSetting::on_MergeMeshCheckBox_toggled()
{
  m_bMergeMesh = ui->MergeMeshCheckBox->isChecked();
}

void ExportMeshSetting::on_separateShaderIdCheckBox_toggled()
{
  m_bSeparateShaderIdFromTexture = ui->separateShaderIdCheckBox->isChecked();
}

void ExportMeshSetting::on_VSInBtn_clicked()
{
  ui->label->setHidden(true);
  ui->label_2->setHidden(true);
  ui->InverseMat->setHidden(true);
  m_ExportType = ExportMeshType::VS_In;
  RefreshMeshSetting(m_VSInMeshDataItems);
}

void ExportMeshSetting::on_VSOutBtn_clicked()
{
  ui->label->setHidden(false);
  ui->label_2->setHidden(false);
  ui->InverseMat->setHidden(false);
  m_ExportType = ExportMeshType::VS_Out;
  RefreshMeshSetting(m_VSOutMeshDataItems);
}

void ExportMeshSetting::on_DSBtn_clicked()
{
  ui->label->setHidden(false);
  ui->label_2->setHidden(false);
  ui->InverseMat->setHidden(false);
  m_ExportType = ExportMeshType::DS;
  RefreshMeshSetting(m_DSMeshDataItems);
}

void ExportMeshSetting::on_InverseMat_textEdited(const QString &arg1)
{
  if(arg1.length() <= 0) return;
  vector<double> matArr;
  std::string stdstr = arg1.toStdString();
  QStringList splitStr = arg1.split(QLatin1Char(';'));
  for(int i = 0; i < splitStr.length(); i++)
  {
    QStringList subStr = splitStr[i].split(QLatin1Char(','));
    for(int j = 0; j < subStr.length(); j++)
    {
      matArr.push_back(subStr[j].toDouble());
    }
  }
  if(matArr.size() != 16) return;
  FbxMatrix Mat = FbxMatrix(matArr[0], matArr[1], matArr[2], matArr[3],
                    matArr[4], matArr[5], matArr[6], matArr[7],
                    matArr[8], matArr[9], matArr[10], matArr[11],
                    matArr[12], matArr[13], matArr[14], matArr[15]);
  // m_InverseMat = Mat.Inverse();
  m_InverseMat = Mat;
}

void ExportMeshSetting::on_blackPoint_textEdited(const QString &arg1)
{
  m_MinActionId = arg1.toUInt();
}

void ExportMeshSetting::on_whitePoint_textEdited(const QString &arg1)
{
  m_MaxActionId = arg1.toUInt();
}

void ExportMeshSetting::on_verticesCbx_currentIndexChanged(int index)
{
  if(index < 0) return;
  m_MeshSettingMap[MeshDataLayer::VERTICE] = GetMeshSettingItem(index);
}

void ExportMeshSetting::on_normalCbx_currentIndexChanged(int index)
{
  if(index < 0) return;
  m_MeshSettingMap[MeshDataLayer::NORMAL] = GetMeshSettingItem(index);
}

void ExportMeshSetting::on_ColorCbx_currentIndexChanged(int index)
{
  if(index < 0) return;
  m_MeshSettingMap[MeshDataLayer::COLOR] = GetMeshSettingItem(index);
}

void ExportMeshSetting::on_texCoordCbx_currentIndexChanged(int index)
{
  if(index < 0) return;
  m_MeshSettingMap[MeshDataLayer::UV] = GetMeshSettingItem(index);
}

void ExportMeshSetting::on_texCoord1Cbx_currentIndexChanged(int index)
{
  if(index < 0) return;
  m_MeshSettingMap[MeshDataLayer::UV1] = GetMeshSettingItem(index);
}

void ExportMeshSetting::on_texCoord2Cbx_currentIndexChanged(int index)
{
  if(index < 0) return;
  m_MeshSettingMap[MeshDataLayer::UV2] = GetMeshSettingItem(index);
}

void ExportMeshSetting::on_texCoord3Cbx_currentIndexChanged(int index)
{
  if(index < 0) return;
  m_MeshSettingMap[MeshDataLayer::UV3] = GetMeshSettingItem(index);
}

void ExportMeshSetting::on_skinWeightCbx_currentIndexChanged(int index)
{
  if(index < 0) return;
  m_MeshSettingMap[MeshDataLayer::SKINWEIGHT] = GetMeshSettingItem(index);
}

void ExportMeshSetting::on_TangentsCbx_currentIndexChanged(int index)
{
  if(index < 0) return;
  m_MeshSettingMap[MeshDataLayer::TANGENT] = GetMeshSettingItem(index);
}

void ExportMeshSetting::on_saveCancelButtons_accepted()
{
  if(m_MaxActionId == 0 || m_MinActionId > m_MaxActionId)
  {
    QMessageBox::warning(this, tr("warning"), tr("ActionId Error"));
    return;
  }

  if(m_SavePath.size() <= 0)
  {
    QMessageBox::warning(this, tr("warning"), tr("Path is Null"));
    return;
  }
  ExportMeshData exportData;
  exportData.savePath = m_SavePath;
  exportData.startActionId = m_MinActionId;
  exportData.endActionId = m_MaxActionId;
  exportData.meshSetting = m_MeshSettingMap;
  exportData.inverseMat = m_InverseMat;
  exportData.bExportTexture = m_bIsNeedExportTexture;
  exportData.bReverseTraiangle = m_bIsReverseTriangle;
  exportData.exportType = m_ExportType;
  exportData.bMergeMesh = m_bMergeMesh;
  exportData.bSeparateShaderId = m_bSeparateShaderIdFromTexture;
  m_Parent->ExportAllMesh(exportData);

  // path is valid and either doesn't exist or user confirmed replacement
  setResult(1);
  accept();
}

void ExportMeshSetting::on_saveCancelButtons_rejected()
{
  reject();
}
