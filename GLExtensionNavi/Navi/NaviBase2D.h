#pragma once
#ifndef NaviBase2D_H
#define NaviBase2D_H

#include "qSlicerNaviModuleExport.h"
#include "vtkMRMLScalarVolumeNode.h"
#include <QColorDialog>

class WidgetData
{
public:
	bool is_enabled = true;
	QString structure_label;
	QString structure_path;
	QString structure_alias;
	float opacitiy = 1;
	QPalette m_myPalette;//paltte对象
	QColorDialog* m_pColorDialog;//调色板显示类
	int m_red;   //颜色的三色，红绿蓝
	int m_green;
	int m_blue;
	float m_opacity;
};

class Q_SLICER_QTMODULES_NAVI_EXPORT NaviBase2D : public QWidget
{
    Q_OBJECT

public:
	NaviBase2D(QWidget* parent);
	vtkMRMLScalarVolumeNode* m_Node = nullptr;
	int m_Rongji = 0;
};
#endif