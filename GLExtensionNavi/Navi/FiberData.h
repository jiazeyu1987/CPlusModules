#pragma once
#include <qobject.h>
#include "itkObject.h"

#include "vtkMRMLScene.h"
#include "vtkSlicerMeasureLogicAbstract.h"
#include "vtkCallbackCommand.h"
#include "vtkMRMLMarkupsFiducialNode.h"
#include "vtkMRMLMarkupsNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLTransformNode.h"
#include "vtkMatrix4x4.h"
#include "vtkOutlineFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkMRMLSegmentationNode.h"
#include "vtkEllipsoidSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkCallbackCommand.h"
#include "vtkObject.h"
#include <QColorDialog>
#include <vtkMRMLMarkupsLineNode.h>
#include <vtkMRMLMarkupsDisplayNode.h>
#include <vtkCylinderSource.h>
class  FiberData
{
public:
	FiberData();
	virtual ~FiberData() ;
public:
	static int fiber_type;
	bool m_LoadingModelFlag = false;
	QColorDialog* m_pColorDialog = nullptr; //调色板显示类
	QPalette m_myPalette;         //paltte对象
	vtkMRMLScene* m_Scene = nullptr;
	vtkSlicerMeasureLogicAbstract* m_Logic = nullptr;
	vtkMRMLMarkupsFiducialNode* m_FiducialNode = nullptr;
	vtkMRMLMarkupsFiducialNode* m_InputFiducialNode = nullptr;
	vtkMRMLMarkupsFiducialNode* m_TargetFiducialNode = nullptr;
	vtkMRMLModelNode* m_ModelNode = nullptr;
	vtkMRMLModelNode* m_ModelNode2 = nullptr;
	vtkMRMLTransformNode* m_TransformNode = nullptr;
	vtkMRMLMarkupsLineNode* m_RedProjectionNode = nullptr;
	vtkMRMLMarkupsLineNode* m_GreenProjectionNode = nullptr;
	vtkMRMLMarkupsLineNode* m_YellowProjectionNode = nullptr;
	double unSelectedColor[3] = { 255,0,0 };
	int m_State;
	int m_red = 229;   //颜色的三色，红绿蓝
	int m_green = 229;
	int m_blue = 76;
	int m_AblationNum = 0;
	float m_RelativePositionDistance = 0;
	int m_RelativePositionDirection = 0;
	double start_pos[5] = { 0,0,0,0,0 };
	bool m_Active = true;
	vtkMRMLSegmentationNode* m_SegmentationNode = nullptr;
	vtkMRMLSegmentationNode* m_ProtectionNode = nullptr;
	vtkMRMLScalarVolumeNode* m_FiberVesselNode = nullptr;
	bool m_ProtectionNodeVisibility = false;
	unsigned long m_NodeObserverTag2 = 0;
	bool is_enabled;
	bool btn1checked = false;
	bool btn2checked = false;
	bool pointTargetSetted = false;
	bool pointInputSetted = false;
	bool m_IsShuqian = false;
	vtkCylinderSource* cyl = nullptr;
	QString m_FiberType;
	//double m_PointInput[3] = { 0 };
	//double m_PointTarget[3] = { 0 };
	int tick_length = 25;
	int fiber_length = 150;
	/*std::vector<mitk::DataNode::Pointer> ablation_vector;
	double modelLength = 202.0;
	mitk::Point3D point1;
	mitk::Point3D point2;*/
	int old_slider_data = -1;
	vtkObserverManager* m_ObserverManager = nullptr;
};
