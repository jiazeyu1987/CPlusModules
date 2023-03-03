#pragma once
#ifndef NaviDWI2D_H
#define NaviDWI2D_H

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
#include "vtkMRMLDiffusionWeightedVolumeNode.h"
#include "qMRMLDisplayNodeViewComboBox.h"
class Q_SLICER_QTMODULES_NAVI_EXPORT NaviDWI2D : public NaviBase2D
{
    Q_OBJECT


	
public:

	NaviDWI2D(QWidget *parent = Q_NULLPTR);
	void SetNode(vtkMRMLScene* scene, vtkMRMLScene* m_PresetScene, vtkMRMLScalarVolumeNode*, vtkMRMLVolumeRenderingDisplayNode*);
	WidgetData* m_Data = nullptr;
	void freshColor();
	vtkMRMLScalarVolumeDisplayNode* GetDisplayNode();
	vtkMRMLVolumeRenderingDisplayNode* m_VolumeRenderingDisplayNode;
	vtkMRMLScene* m_Scene;
	vtkMRMLDiffusionWeightedVolumeNode* node_dti_output;
	vtkMRMLScalarVolumeNode* node_dti_baseline;
	QString GetFiberBundleFilePath();
	QString GetFiberBundleNodeName();
	QString GetDTIFilePath();
	QString GetBaseLineFilePath();
	void DoCreateDTINrrd();
	void DoLoadDTIFiberBundle();
signals:
	void onRongjiChanged();
	void onOpacityChanged(int);
protected slots:
	void OncbxChanged();
	void onOpenColorPad();
	void OnOpacityValueChanged(int);
	void onVisibleChanged(int);
	void onRongjiChanged(int);
	void onDTIBuild();
	
	void setColorNode(vtkMRMLNode* colorNode);
	void onGetColor(QColor color); //获取调色板rgb值
	void onCropToggled(bool crop);
	void SetTubeVisible(bool);
	void DoBuidDTI();
public:
    Ui::NaviDWI2D ui;
};
#endif