#pragma once

#include <QDialog>
#include <QTimer>
#include "Code/Interface/QRDInterface.h"
#include "Windows/MainWindow.h"
#include "Windows/Fbx/FBXManager.h"

namespace Ui
{
class GlobalHook;
}


class GlobalHook  : public QDialog
{
  Q_OBJECT

public:
  explicit GlobalHook();
  ~GlobalHook();

  void RefreshHistoryProgress(const QStringList& HistoryItems);
  
private slots:
  void on_shimText_textEdited(const QString &arg1);
  void on_browse_clicked();
  void on_PreUsedCbx_currentIndexChanged(int index);
  void on_progressText_textEdited(const QString &arg1);
  void on_buttonBox_accepted();
  void on_buttonBox_rejected();


  void BackupAndChangeRegistry();
  void RestoreRegistry();
  std::wstring UTF82Wide(const std::string& strUTF8);
  
  void LoadHistoryHookProgresses();
  void ApplyProgressHook();
  void SaveHookProgress();
  void FindHookDllFIle(const std::string& Path, const std::string& DllName, std::string& outDllPath);
  
private:
  Ui::GlobalHook *ui;

  QString m_ShimSavePath;

  QString m_OldAppinitDLLs;

  QString m_OldX64AppinitDLLs;

  QString m_ProgressName;
  
  QStringList m_HistoryProgresses;
  
};
