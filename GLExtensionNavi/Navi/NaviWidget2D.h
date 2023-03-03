#pragma once
#ifndef NaviWidget2D_H
#define NaviWidget2D_H

#include "ui_NaviWidget2D.h"
#include "qSlicerNaviModuleExport.h"
#include "vtkMRMLScalarVolumeNode.h"
#include <QColorDialog>
#include "vtkMRMLDisplayNode.h"
#include "vtkMRMLScalarVolumeDisplayNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLVolumeRenderingDisplayNode.h"
#include "vtkMRMLVolumePropertyNode.h"
#include "vtkMRMLAnnotationROINode.h"
#include "NaviBase2D.h"
#include "vtkSlicerVolumeRenderingLogic.h"
#include "SwitchControl.h"
#include "qSlicerVolumeRenderingPresetComboBox.h"
#include <PythonQt.h>
class Q_SLICER_QTMODULES_NAVI_EXPORT NaviWidget2D : public NaviBase2D
{
    Q_OBJECT


	
public:
	static const std::string visible_2d;
	static const std::string visible_3d;
	static const std::string visible_roi;
	const static std::string color_node_id;
	const static std::string preset_index;
	const static std::string preset_shift;
	const static std::string node_roi;

	NaviWidget2D(QWidget *parent = Q_NULLPTR);
	QPixmap* GetPirctureFromScalarVolumeNode(vtkMRMLScalarVolumeNode* node);
	void SetNode(vtkMRMLScene* scene, vtkMRMLScene* m_PresetScene, vtkMRMLScalarVolumeNode*, vtkMRMLVolumeRenderingDisplayNode*);
	WidgetData* m_Data = nullptr;
	void freshColor();
	vtkSlicerVolumeRenderingLogic* vrlLogic = nullptr;
	vtkMRMLScalarVolumeDisplayNode* GetDisplayNode();
	vtkMRMLVolumeRenderingDisplayNode* m_VolumeRenderingDisplayNode;
	vtkMRMLScene* m_Scene;
	void SetWidget2DVisible(bool checked);
	bool hiddenPanelVisible = false;
	void SetDropdownBtnStatus(bool state, bool isEmitEvent = true);
	SwitchControl* pSwVolume;
	vtkMRMLNode* m_PresetNode = nullptr;
	bool is_fusioned;
	void DoSetColorTableNode(vtkMRMLNode* node);
	void SetAttribute(const char* key, const char* val);
	void SetName(QString name1);
	void SetExtraImage(QString path);
	void showMask(NaviWidget2D* Item2D , bool isShow);
	void SetPresetNode(vtkMRMLNode* node);
	void ChangeVisible2DState(bool b);
	void ChangeVisible3DState(bool b);
	void ChangeVisibleROIState(bool b);
	
	//QString m_Label = "Basic";
signals:
	void onRongjiChanged();
	void onOpacityChanged(int);
	void displayHiddenPanel(NaviWidget2D*);
	void visible_changed(NaviWidget2D*);
	void modelFusionStateChanged(NaviWidget2D*); //模型是否参与融合的状态改变的信号
	void showBlendPro(NaviWidget2D*);
	void fresh_2d();
	void fresh_3d();
	void show_node(const char*);
protected slots:
	//void OncbxChanged(bool b);
	void onOpenColorPad();
	void onCreateModel();
	void SetPresettNodeID(const QString);
	void OnOpacityValueChanged(int);
	//void onVisibleChanged(int);
	void onVisibleChanged2D(bool);
	void onVisibleChanged3D(bool);
	void onRongjiChanged(int);
	void OnStripSkull();
	void setColorNode(vtkMRMLNode* colorNode);
	void onGetColor(QColor color); //获取调色板rgb值
	void onVisibleChangedROI(bool b);
	//void ControlPanelVisable(bool bChecked);
	void onToggled(bool bChecked);
public:
	SwitchControl* pSwitchControl;
public:
    Ui::NaviWidget2DClass ui;
};
#endif