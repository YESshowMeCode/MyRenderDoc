#pragma once

#include <QDialog>
#include <QTimer>
#include "Code/Interface/QRDInterface.h"
#include "Windows/MainWindow.h"


namespace Ui
{
class StatisticsTrianglesDialog;
}


class StatisticsTrianglesDialog : public QDialog
{
  Q_OBJECT
public:
  explicit StatisticsTrianglesDialog(MainWindow *parent);
  ~StatisticsTrianglesDialog();


private slots:
  void on_blackPoint_textEdited(const QString &arg1);
  void on_whitePoint_textEdited(const QString &arg1);
  void on_TextMaterial_textEdited(const QString &arg1);
  void on_trianglenumLable_textEdited(const QString &arg1);
  void on_exeBtn_clicked();
  void on_StatGameBtn_clicked();
  void on_StatMaterialBtn_clicked();  

private:
  Ui::StatisticsTrianglesDialog *ui;
  MainWindow* m_Parent;

  uint32_t m_MinEventId;
  uint32_t m_MaxEventId;
  
  uint32_t m_MaterialId;

};
