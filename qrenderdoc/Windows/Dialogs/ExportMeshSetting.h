#pragma once

#include <QDialog>
#include <QTimer>
#include "Code/Interface/QRDInterface.h"
#include "Windows/MainWindow.h"
#include "Windows/Fbx/FBXManager.h"

namespace Ui
{
class ExportMeshSetting;
}


class ExportMeshSetting  : public QDialog
{
  Q_OBJECT

public:
  explicit ExportMeshSetting(const QStringList& VSInMeshDataItems, const QStringList& VSOutMeshDataItems, const QStringList& DSMeshDataItems, MainWindow *parent);
  ~ExportMeshSetting();

  void RefreshMeshSetting(const QStringList& MeshDataItems);
  QString GetMeshSettingItem(int index);

private slots:
  void on_filename_textEdited(const QString &arg1);
  void on_browse_clicked();
  void on_ExportTexCheckBox_toggled();
  void on_reverseTriangle_toggled();
  void on_MergeMeshCheckBox_toggled();
  void on_separateShaderIdCheckBox_toggled();
  void on_VSInBtn_clicked();
  void on_VSOutBtn_clicked();
  void on_DSBtn_clicked();
  void on_InverseMat_textEdited(const QString &arg1);
  void on_blackPoint_textEdited(const QString &arg1);
  void on_whitePoint_textEdited(const QString &arg1);
  void on_verticesCbx_currentIndexChanged(int index);
  void on_normalCbx_currentIndexChanged(int index);
  void on_ColorCbx_currentIndexChanged(int index);
  void on_texCoordCbx_currentIndexChanged(int index);
  void on_texCoord1Cbx_currentIndexChanged(int index);
  void on_texCoord2Cbx_currentIndexChanged(int index);
  void on_texCoord3Cbx_currentIndexChanged(int index);
  void on_skinWeightCbx_currentIndexChanged(int index);
  void on_TangentsCbx_currentIndexChanged(int index);
  void on_saveCancelButtons_accepted();
  void on_saveCancelButtons_rejected();
private:
  Ui::ExportMeshSetting *ui;
  MainWindow* m_Parent;

  QString m_SavePath;

  uint32_t m_MinActionId;
  uint32_t m_MaxActionId;

  QStringList m_VSInMeshDataItems;
  
  QStringList m_VSOutMeshDataItems;

  QStringList m_DSMeshDataItems;

  QMap<int, QString> m_MeshSettingMap;

  bool m_bIsNeedExportTexture;

  bool m_bIsReverseTriangle;

  ExportMeshType m_ExportType;

  FbxMatrix m_InverseMat;

  bool m_bMergeMesh;

  bool m_bSeparateShaderIdFromTexture;

};
