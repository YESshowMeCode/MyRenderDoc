#include "StatisticsTrianglesDialog.h"
#include "ui_StatisticsTrianglesDialog.h"

StatisticsTrianglesDialog::StatisticsTrianglesDialog(MainWindow *parent)
  :m_Parent(parent), ui(new Ui::StatisticsTrianglesDialog)
{
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  
  // setWindowFlags( windowFlags() & ~Qt::WindowStaysOnTopHint); //取消置顶
  this->setWindowFlag(Qt::WindowStaysOnTopHint,false);
  show();
  ui->setupUi(this);
  // setWindowFlags(windowFlags());
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  m_MinEventId = 0;
  m_MaxEventId = 0;
  m_MaterialId = 0;
  ui->trianglenumLable->setText(QString());
}

StatisticsTrianglesDialog::~StatisticsTrianglesDialog()
{
}

void StatisticsTrianglesDialog::on_blackPoint_textEdited(const QString &arg1)
{
  m_MinEventId = arg1.toUInt();
}

void StatisticsTrianglesDialog::on_whitePoint_textEdited(const QString &arg1)
{
  m_MaxEventId = arg1.toUInt();
}

void StatisticsTrianglesDialog::on_TextMaterial_textEdited(const QString &arg1)
{
  m_MaterialId = arg1.toUInt();
}

void StatisticsTrianglesDialog::on_trianglenumLable_textEdited(const QString &arg1)
{
  
}

void StatisticsTrianglesDialog::on_exeBtn_clicked()
{
  
  if(m_MinEventId >= m_MaxEventId || m_MaxEventId == 0)
  {
    QMessageBox::warning(this, tr("warning"), tr("EventId Error: The minimum is greater than the value "));
    return;
  }
  uint32_t trianglesNum = m_Parent->StatTrianlgesNum(m_MinEventId, m_MaxEventId);
  ui->trianglenumLable->setText(lit("Triangles Number is: %1").arg(trianglesNum));
}

void StatisticsTrianglesDialog::on_StatGameBtn_clicked()
{
  uint32_t AllTrianglesNum, ColorPassTrianglesNum, DepthPassTrianglesNum, TransparentTrianglesNum, DrawCallsNum;
  QList<uint32_t> Events;
  m_Parent->StatGameTrianglesData(AllTrianglesNum,ColorPassTrianglesNum, DepthPassTrianglesNum, TransparentTrianglesNum, DrawCallsNum, Events);

  QString EventsList;
  int index = 0;
  for(uint32_t eventId : Events)
  {
    index++;
    EventsList.append(QString::fromStdString(std::to_string(eventId))).append(QLatin1Char(','));
    if(index >= 6)
    {
      index = 0;
      EventsList.append(QLatin1Char('\n'));
    }
  }
  
  ui->trianglenumLable->setText(lit("All Triangles Number is %1 \n DrawCalls Number is %2 \nColorPass Triangles Number is %3 \nDepthPass Triangles Number is %4 \nTransparent Triangles Number is %5 \nTransparent events: %6")
    .arg(AllTrianglesNum).arg(DrawCallsNum).arg(ColorPassTrianglesNum).arg(DepthPassTrianglesNum).arg(TransparentTrianglesNum).arg(EventsList));
}

void StatisticsTrianglesDialog::on_StatMaterialBtn_clicked()
{
  if(m_MaterialId != 0)
  {
    QList<uint32_t> Events;
    uint32_t trianglesNum = m_Parent->StatTrianglesInfoByMaterialId(m_MaterialId, Events);

    QString EventsList;
    int index = 0;
    for(uint32_t eventId : Events)
    {
      EventsList.append(QString::fromStdString(std::to_string(eventId))).append(QLatin1Char(','));
      if(index >= 6)
      {
        index = 0;
        EventsList.append(QLatin1Char('\n'));
      }
    }
    ui->trianglenumLable->setText(lit("this Material Triangles Number is %1 \n events：%2").arg(trianglesNum).arg(EventsList));
  }
}
