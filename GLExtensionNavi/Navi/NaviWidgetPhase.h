#pragma once

#include <QtWidgets/QWidget>
#include <QColorDialog>
#include "ui_NaviWidgetPhase.h"
#include "vtkMRMLScalarVolumeNode.h"
class NaviWidgetPhase : public QWidget
{
    Q_OBJECT

private:
	QColorDialog *m_pColorDialog;//调色板显示类
	QPalette m_myPalette;//paltte对象
	int m_red;   //颜色的三色，红绿蓝
	int m_green;
	int m_blue;
	vtkMRMLScalarVolumeNode* m_Node;
public:
    NaviWidgetPhase(QWidget *parent = Q_NULLPTR);
	void SetMyNode(vtkMRMLScalarVolumeNode*);
	vtkMRMLScalarVolumeNode* GetMyNode();
	void SetImage(const char* imagePath,int width, int height,const char* extra_image_path = nullptr);
	void SelectedStyle(bool selected);
signals:
	void volumnSetting();
	void modelFusionStateChanged(bool bChanged); //模型是否参与融合的状态改变的信号
	void changeBackColor(); //改变背景色信号函数
Q_SIGNALS:
	void clicked();
	
	void need_update();
private:
    Ui::NaviWidgetPhase ui;
	
};
