#include "GlobalHook.h"

#include <corecrt_io.h>

#include <QColorDialog>
#include <QFileInfo>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <winreg.h>
#include <fstream>
#include "Code/QRDUtils.h"
#include "ui_GlobalHook.h"


GlobalHook::GlobalHook()
  :ui(new Ui::GlobalHook)
{
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  ui->setupUi(this);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  ui->shimText->setFont(Formatter::PreferredFont());
  ui->progressText->setFont(Formatter::PreferredFont());

  m_ShimSavePath = tr("F:/X6/Tools/renderdoc-1.27/Hook/PapergamesShim.dll");

  ui->shimText->setText(m_ShimSavePath);
  LoadHistoryHookProgresses();
}

GlobalHook::~GlobalHook()
{
  delete ui;
}

void GlobalHook::RefreshHistoryProgress(const QStringList &HistoryItems)
{
  ui->PreUsedCbx->clear();
  ui->PreUsedCbx->addItems(HistoryItems);
}

void GlobalHook::BackupAndChangeRegistry()
{
  HKEY keyNative = NULL;
  HKEY keyWow32 = NULL;

  DWORD appinitEnabled = 1;
  QString nativeShimPath = m_ShimSavePath;
  nativeShimPath.replace(QLatin1Char('/'), QLatin1Char('\\'));
  std::string shimpathNative = nativeShimPath.toStdString();
  
  // AppInit_DLLs requires short paths, but short paths can be disabled globally or on a per-volume
  // level. If short paths are disabled we'll get the long path back, we *always* expect the path to
  // get shorter because the shim filename is bigger than 8.3.

  DWORD nativeShortSize = GetShortPathNameW((LPCWSTR)shimpathNative.c_str(), NULL,
                                            (DWORD)shimpathNative.length());


  // open the native key
  LSTATUS ret = RegCreateKeyExA(HKEY_LOCAL_MACHINE,
                                "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows", 0, NULL,
                                0, KEY_READ | KEY_WRITE, NULL, &keyNative, NULL);
  

  const DWORD one = 1;

  // fetch the previous data for LoadAppInit_DLLs and AppInit_DLLs
  DWORD sz = 4;
  ret = RegGetValueA(keyNative, NULL, "LoadAppInit_DLLs", RRF_RT_REG_DWORD, NULL,
                     &appinitEnabled, &sz);

  sz = 0;
  ret = RegGetValueW(keyNative, NULL, L"AppInit_DLLs", RRF_RT_ANY, NULL, NULL, &sz);
  if(ret == ERROR_MORE_DATA || ret == ERROR_SUCCESS)
  {
    rdcarray<wchar_t> oldStr;
    oldStr.resize(sz / sizeof(wchar_t));
    ret = RegGetValueW(keyNative, NULL, L"AppInit_DLLs", RRF_RT_ANY, NULL,
                       oldStr.data(), &sz);
    // m_OldAppinitDLLs.fromStdString(oldStr);
  }

  ret = RegSetValueExA(keyNative, "LoadAppInit_DLLs", 0, REG_DWORD, (const BYTE *)&one, sizeof(one));

  std::wstring shimPath = UTF82Wide(nativeShimPath.toStdString());
  ret = RegSetValueExW(keyNative, L"AppInit_DLLs", 0, REG_SZ, (const BYTE *)shimPath.data(),
                       DWORD(shimPath.length() * sizeof(wchar_t)));


  if(!keyWow32)
  {
    ret = RegCreateKeyExA(HKEY_LOCAL_MACHINE,
                          "SOFTWARE\\Wow6432Node\\Microsoft\\Windows NT\\CurrentVersion\\Windows",
                          0, NULL, 0, KEY_READ | KEY_WRITE, NULL, &keyWow32, NULL);

  }
  
  // if we're doing Wow32, repeat the process for those keys
  if(keyWow32)
  {
    sz = 4;
    ret = RegGetValueA(keyWow32, NULL, "LoadAppInit_DLLs", RRF_RT_REG_DWORD, NULL,
                       (void *)&appinitEnabled, &sz);

    sz = 0;
    ret = RegGetValueW(keyWow32, NULL, L"AppInit_DLLs", RRF_RT_ANY, NULL, NULL, &sz);
    if(ret == ERROR_MORE_DATA || ret == ERROR_SUCCESS)
    {
      // ret = RegGetValueW(keyWow32, NULL, L"AppInit_DLLs", RRF_RT_ANY, NULL,
      //                    (LPBYTE)lpOldStr, &sz);
      // m_OldAppinitDLLs.fromStdString(oldStr);
    }

    ret = RegSetValueExA(keyWow32, "LoadAppInit_DLLs", 0, REG_DWORD, (const BYTE *)&one, sizeof(one));

    std::wstring shimPath = UTF82Wide(nativeShimPath.toStdString());
    ret = RegSetValueExW(keyWow32, L"AppInit_DLLs", 0, REG_SZ, (const BYTE *)shimPath.data(),
                         DWORD(shimPath.length() * sizeof(wchar_t)));
  }
}

void GlobalHook::RestoreRegistry()
{
  HKEY keyNative = NULL;
  HKEY keyWow32 = NULL;

  std::string oldAppInitDLLs = m_OldAppinitDLLs.toStdString();
  
  LSTATUS ret = RegCreateKeyExA(HKEY_LOCAL_MACHINE,
                                "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows", 0, NULL,
                                0, KEY_READ | KEY_WRITE, NULL, &keyNative, NULL);


#if ENABLED(RDOC_X64)
  ret = RegCreateKeyExA(HKEY_LOCAL_MACHINE,
                        "SOFTWARE\\Wow6432Node\\Microsoft\\Windows NT\\CurrentVersion\\Windows", 0,
                        NULL, 0, KEY_READ | KEY_WRITE, NULL, &keyWow32, NULL);

  REG_CHECK("Could not open AppInit key");
#endif
  

  ret = RegSetValueExW(keyNative, L"AppInit_DLLs", 0, REG_SZ,
                       (const BYTE *)oldAppInitDLLs.c_str(),
                       DWORD(oldAppInitDLLs.length() * sizeof(wchar_t)));
  // REG_CHECK("Could not set AppInit_DLLs");

  // if we opened it, restore the Wow32 values as well
  if(keyWow32)
  {
    ret = RegSetValueExW(keyWow32, L"AppInit_DLLs", 0, REG_SZ,
                         (const BYTE *)oldAppInitDLLs.c_str(),
                         DWORD(oldAppInitDLLs.length() * sizeof(wchar_t)));
    // REG_CHECK("Could not set AppInit_DLLs");
  }
}

std::wstring GlobalHook::UTF82Wide(const std::string &strUTF8)
{
  int nWide = ::MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), (int)strUTF8.size(), NULL, 0);

  std::unique_ptr<wchar_t[]> buffer(new wchar_t[nWide + 1]);
  if (!buffer)
  {
    return L"";
  }

  ::MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), (int)strUTF8.size(), buffer.get(), nWide);
  buffer[nWide] = L'\0';

  return buffer.get();
}


void GlobalHook::LoadHistoryHookProgresses()
{
  QString path = QString::fromStdString(_pgmptr);
  std::string stdstr = path.toStdString();
  QStringList splitStr = path.split(QLatin1Char('\\'));
  path.clear();
  for(int i = 0; i < splitStr.length() - 1; i++)
  {
    path.append(splitStr[i]);
    path.append(tr("/"));
  }
  std::string saveHistoryPath = path.toStdString() + "/HistoryProgress.txt";

  std::ifstream  fileHandle;
  fileHandle.open(saveHistoryPath, ios::in);
  if(!fileHandle.is_open())
    return;

  char buff[1024] = {0};
  m_HistoryProgresses.clear();
  while(fileHandle >> buff)
  {
    std::string str = buff;
    QString qstr = QString::fromStdString(str);
    if(!m_HistoryProgresses.contains(qstr))
    {
      m_HistoryProgresses << qstr;
    }
  }
  if(m_HistoryProgresses.length() > 0)
  {
    m_ProgressName = m_HistoryProgresses.last();
    ui->progressText->setText(m_ProgressName);
  }
  RefreshHistoryProgress(m_HistoryProgresses);
}

void GlobalHook::ApplyProgressHook()
{
  QStringList splitStr = m_ShimSavePath.split(QLatin1Char('/'));
  QString path;
  for(int i = 0; i < splitStr.length() - 1; i++)
  {
    path.append(splitStr[i]);
    path.append(tr("/"));
  }
  std::string dllPath;
  FindHookDllFIle(path.toStdString(), "inlinePapergames.dll", dllPath);
  
  
  std::string HookConfigPath = path.toStdString() + "PaperHook.txt";
  fstream file(HookConfigPath, ios::out);
  std::ofstream out(HookConfigPath, std::ios::app);
  out << dllPath << std::endl;
  out << m_ProgressName.toStdString() << std::endl;
  out.close();
  SaveHookProgress();
}

void GlobalHook::SaveHookProgress()
{
  if(m_HistoryProgresses.contains(m_ProgressName))
  {
    m_HistoryProgresses.removeOne(m_ProgressName);
  }
  m_HistoryProgresses << m_ProgressName;
  
  QString path = QString::fromStdString(_pgmptr);
  std::string stdstr = path.toStdString();
  QStringList splitStr = path.split(QLatin1Char('\\'));
  path.clear();
  for(int i = 0; i < splitStr.length() - 1; i++)
  {
    path.append(splitStr[i]);
    path.append(tr("/"));
  }
  std::string saveHistoryPath = path.toStdString() + "HistoryProgress.txt";
  fstream file(saveHistoryPath, ios::out);
  std::ofstream out(saveHistoryPath, std::ios::app);
  for(QString str : m_HistoryProgresses)
  {
    out << str.toStdString() << std::endl;
  }
  out.close();
}

void GlobalHook::FindHookDllFIle(const std::string &Path, const std::string &DllName, std::string& outDllPath)
{
  intptr_t hFile = 0;
  struct _finddata_t fileinfo;
  string p;
  bool bFind = false;
  if ((hFile = _findfirst(p.assign(Path).append("/*").c_str(), &fileinfo)) != -1)
  {
    
    while (!bFind && _findnext(hFile, &fileinfo) == 0)
    {

      if ((fileinfo.attrib & _A_SUBDIR))
      {
        if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
          FindHookDllFIle(p.assign(Path).append("/").append(fileinfo.name), DllName, outDllPath);
      }
      else
      {
        if(fileinfo.name == DllName)
        {
          outDllPath = p.assign(Path).append("/").append(fileinfo.name);
          bFind = true;
        }
      }
    } 
    _findclose(hFile);
  }
}

void GlobalHook::on_shimText_textEdited(const QString &arg1)
{
}

void GlobalHook::on_browse_clicked()
{
  m_ShimSavePath = QFileDialog::getOpenFileName(this, tr("选择shim文件"));
  ui->shimText->setText(m_ShimSavePath);
}

void GlobalHook::on_PreUsedCbx_currentIndexChanged(int index)
{
}

void GlobalHook::on_progressText_textEdited(const QString &arg1)
{
  m_ProgressName = arg1;
}

void GlobalHook::on_buttonBox_accepted()
{
  if(m_ShimSavePath.length() <= 0 || m_ProgressName.length() <= 0) return;

  BackupAndChangeRegistry();
  ApplyProgressHook();
  setResult(1);
  accept();
}

void GlobalHook::on_buttonBox_rejected()
{
  reject();
}
