#pragma once
#include <QApplication>
#include "Code/QRDUtils.h"
#include "QRDInterface.h"

ShaderToolOutput RunTool(const ShaderProcessingTool &tool, QWidget *window, QString input_file,
                         QString output_file, QStringList &argList);