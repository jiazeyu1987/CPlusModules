#pragma once
#ifndef MODEL_ADJUST_VIEW
#define MODEL_ADJUST_VIEW
#include "qSlicerNaviModuleExport.h"
#include "qwidget.h"

#include "ui_ModelAdjustView.h"

class Q_SLICER_QTMODULES_NAVI_EXPORT ModelAdjustView:public QWidget
{
	Q_OBJECT
public:
	ModelAdjustView(QWidget* parent = nullptr);

private:
	ModelAdjustView* m_AdjustView;
public:
	Ui::ModelAdjustView ui;
};

#endif // !MODEL_ADJUST_VIEW