#pragma once

#include <QtWidgets/QWidget>
#include <QColorDialog>
#include "ui_NaviWidgetPhase.h"
#include "vtkMRMLScalarVolumeNode.h"
class NaviWidgetPhase : public QWidget
{
    Q_OBJECT

private:
	QColorDialog *m_pColorDialog;//��ɫ����ʾ��
	QPalette m_myPalette;//paltte����
	int m_red;   //��ɫ����ɫ��������
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
	void modelFusionStateChanged(bool bChanged); //ģ���Ƿ�����ںϵ�״̬�ı���ź�
	void changeBackColor(); //�ı䱳��ɫ�źź���
Q_SIGNALS:
	void clicked();
	
	void need_update();
private:
    Ui::NaviWidgetPhase ui;
	
};
