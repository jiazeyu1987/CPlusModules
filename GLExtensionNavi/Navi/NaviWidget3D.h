#pragma once

#include <QtWidgets/QWidget>
#include <QColorDialog>
#include "vtkSegment.h"
#include "vtkMRMLSegmentationDisplayNode.h"
#include "ui_NaviWidget3D.h"
#include "vtkMRMLScalarVolumeNode.h"
class Widget3DData
{
public:
	bool is_enabled = true;
	QString structure_label;
	QString structure_path;
	QString structure_alias;
	vtkMRMLScalarVolumeNode* m_MasterNode = nullptr;
	float opacitiy = 1;
	std::string segmentID = "";
	vtkSegment* segment = nullptr;
	vtkMRMLSegmentationNode* m_SegmentationNode = nullptr;
	
	QColorDialog* m_pColorDialog;//调色板显示类
	QPalette m_myPalette;//paltte对象
	int m_red;   //颜色的三色，红绿蓝
	int m_green;
	int m_blue;
	float m_opacity;
	float m_opacity2D = 100;
	float m_opacity3D = 100;
};

class NaviWidget3D : public QWidget
{
    Q_OBJECT
private:
	void init();
public:
	static const std::string visible_2d;
	static const std::string visible_3d;
	static const std::string opacity_all;
	static const std::string opacity_2D;
	static const std::string opacity_3D;
	const static std::string color_red;
	const static std::string color_green;
	const static std::string color_blue;
	const static std::string structure_name;
	const static std::string master_node_id;
	void SetTitle(QString);
	void hideDeleteBtn();
	void hideModifyBtn();
	void SetPicture(QString, QString);
	void SetData(Widget3DData*);
	vtkMRMLSegmentationDisplayNode* GetDisplayNode();
	void SetAttribute(const char* key, const char* val);
	void SetSegmentInfo(vtkSegment*, std::string,  vtkMRMLSegmentationNode* snode);
	void freshColor();
	void ChangeVisible2DState(bool);
	void ChangeVisible3DState(bool b);
	void SetMasterNode(vtkMRMLScalarVolumeNode*);
	Widget3DData* m_Data = nullptr;
  NaviWidget3D(QWidget *parent = Q_NULLPTR);
	NaviWidget3D(Widget3DData*, QWidget *parent = Q_NULLPTR);
	~NaviWidget3D();
signals:
	void changeBackColor();
	void removeWidget3D(NaviWidget3D*);
	void modifyWidget3D(NaviWidget3D*);
public slots:
	void OnFresh2DFromDisplayNode();
	void OnFresh3DFromDisplayNode();
	void onVisibleChanged(bool);
	void on3DVisibleChanged(bool);
	void OnOpacityValueChanged(int);
	void OnOpacityValueChanged2D(int);
	void OnOpacityValueChanged3D(int);
	void onDelete3DItem();
	void onModify3DItem();
	void onOpenColorPad();
	
	void onGetColor(QColor color); //获取调色板rgb值
	void onSetBackColor();
public:
    Ui::NaviWidget3D ui;

	
};
