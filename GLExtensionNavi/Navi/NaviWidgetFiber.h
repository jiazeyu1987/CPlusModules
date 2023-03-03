#pragma once

#include <QWidget>
#include <QColorDialog>
#include "ui_NaviWidgetFiber.h"
#include "FiberData.h"
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
#include "ctkVTKObject.h"
#include "SwitchControl.h"
class FiberBaseInfo {
public:
	QStringList FiberTypeList = { "FD04C01A-4mm-1.55mm", "FD04C01B-4mm-2.3mm", "FD10C01A-10mm-1.55mm", "FD10C01B-10mm-2.3mm", "FD15C01A-15mm-1.55mm", "FD15C01B-15mm-2.3mm" };
	int id;
	float ablZRadius[2] = { 0 }; //z radius
	float ablRadius[2] = { 0 };//x or y radius
	float length;//the length of fiber
	float spacing;//The default spacing between two ablation block
	float startPos[2] = { 0 };//The distance from the first ablation block to the target
	QString name;//the name of fiber
	QStringList list;//the fiber model list
	FiberBaseInfo() {}
	~FiberBaseInfo() {}
	FiberBaseInfo(int _id, float _len, float _spacing, QString _name, QString _fileName, float zradius1, float zradius2, float radius1, float radius2, float pos, float pos2) {
		id = _id;
		length = _len;
		spacing = _spacing;
		name = FiberTypeList[id - 1];
		ablZRadius[0] = zradius1;
		ablZRadius[1] = zradius2;
		ablRadius[0] = radius1;
		ablRadius[1] = radius2;
		startPos[0] = pos;
		startPos[1] = pos2;
		for (int i = 1; i <= 2; i++) {
			list.append(QString("%1%2").arg(_fileName).arg(i));
		}
	}
};

class NaviWidgetFiber : public QWidget
{
	Q_OBJECT
		QVTK_OBJECT
public:
	NaviWidgetFiber(FiberData* firberData, bool is_shuqian, QWidget* parent, vtkMRMLScene* scene, vtkSlicerMeasureLogicAbstract* logic);
	~NaviWidgetFiber();
protected:
	
	static void processEvents(vtkObject* caller, unsigned long eid, void* clientData, void* callData);
	void mouseReleaseEvent(QMouseEvent* event);
	//mitk::DataStorage::Pointer m_Storage;
public:
	enum State
	{
		Normal,
		WaitForTargetPoint,
		WaitForInputPoint
	};
	enum Layout
	{
		InnerHeight = 145,
		SpanHeight = 405,
		
	};
	static QVector<FiberBaseInfo> m_BaseInfoVec;
	const static std::string input_node_id;
	const static std::string output_node_id;
	const static std::string fiber_name;
	const static std::string fiber_type;
	const static std::string color_red;
	const static std::string color_green;
	const static std::string color_blue;
	const static std::string fiber_visible;
	const static std::string fiber_ablation_visible;
	const static std::string fiber_save_depth;
	const static std::string ablation_node_id;
	const static std::string stl_head;
	const static std::string stl_head_node;
	const static std::string red_projection;
	const static std::string green_projection;
	const static std::string yellow_projection;
	const static std::string r_d_direction;
	const static std::string r_d_distance;
	const static std::string leksell_x;
	const static std::string leksell_y;
	const static std::string leksell_z;
	const static std::string leksell_arc;
	const static std::string leksell_ran;
	const static std::string protect_area_id;
	const static std::string protect_area_visible;
	const static std::string protect_area_diameter;
	const static std::string is_on_edit;
	FiberBaseInfo m_BaseInfo;
	int tick_length = 25;
	int lastSelectType = 0;
	int fiber_length = 150;

	FiberData* m_FiberData = nullptr;
	void CreateSegmentationNode();
	void SetVisibleEnabled(bool b);
	void SetFrameInfo(QString x, QString y, QString z, QString arc, QString ran);
	static void ProcessMRMLEvents(vtkObject* caller, unsigned long eid, void* clientData, void* callData);
	void SetFixedFiberType(QString);
	void DoUpdateAblationNumber();
	void DoUpdateDistance();
	void SetFiberEnabled(bool b);
	void SetDeleteable(bool b);
	void SetToggleActive(bool b);
	void DeleteFromScene();
	bool disable_use();
	void DoUpdateFiberPosition();
	void CheckFiberAddActive();
	
	void ChangeWave();
	void CreateCylinderProtectionArea();
	void update_ablation_position(int i);
	void SetSliderValue(int value);
	void SetFiberFrameInfo(QString x,QString y,QString z,QString arc,QString ring);
	void DoAddAblationDetail(int startpos,std::string segment_id);
	void SetSwitchVisible(SwitchControl* sc, bool b);
	void SetAttribute(const char* key, const char* val,int i = 0);
	QString GetName();
	void SetFiberName(QString str);
	QString GetType();
	void CreatPicTextButton(QPushButton *btn,QString picPath,QString text);
	SwitchControl* CreateSwitch(QHBoxLayout* layout);
	SwitchControl* ctrl_ablation = nullptr;
	SwitchControl* ctrl_protection = nullptr;
	SwitchControl* ctrl_vessel = nullptr;
	void setPictureText(QString str);
public Q_SLOTS:
	void OnEdit(bool b);
	void OnOpenColorPad();
	void OnUpdateTargetPointInfo();
	void OnChangeVisible(bool);
	void OnChangeAblactionVisible(bool b);
	void OnCreateFiber();
	void onChangeViewDirection(int i);
	//void mouseReleaseEvent(QMouseEvent* event);
	void OnClickBtnFiberPoint1(bool);
	void OnClickBtnFiberPoint2(bool);
	void onTargetNodeAdded();
	void onInputNodeAdded();
	void OnChangeProtectVisible(bool);
	void OnNodeAdd(const itk::EventObject&);
	void OnDepthChanged(int);
	void OnFiberSTLChanged(int);
	void OnDelete();
	void OnAddAblationArea();
	void onRemoveAblationArea();
	void OnOpacityChanged(int);
	void onAdjustFiberPoint();
	void OnEstimate(bool);
	void OnProtectionDiameterChanged(double);
	void OnDeleteAblationArea();
	void OnAblationChanged(int);
	void DoFreshAblationList();
	void OnFiberNameChanged(QString);
	void onGetColor(QColor);
	void OnFiberViewDirection();
	void DoSafeRemove(vtkMRMLNode*);
	void OnClickTargetPoint();
	void OnClickInputPoint();
	void OnAlpha0();
	void OnAlpha100();
	void OnDeleteFiber();
Q_SIGNALS:
	void clicked(NaviWidgetFiber*);
	void show_fiber(NaviWidgetFiber*, QString);
	void move_fiber(NaviWidgetFiber*);
	void delete_fiber(NaviWidgetFiber*);
	void view_direction(NaviWidgetFiber*,int);
	void need_update();
	void center_target();
	void update_frames();
	void center_input();
	void on_edit_fiber(NaviWidgetFiber*, bool b);
	void rotate_to_vector(double*, double* m_PointTarget);
	void fiber_depth(NaviWidgetFiber*, int);
	void estimate(NaviWidgetFiber*);
	//void onOpacityChanged1(int);
	void ablation_number_changed();
public:
	Ui::NaviWidgetFiber ui;
	void RotateToFiber(double* m_PointTarget, double*);
	vtkNew<vtkCallbackCommand> m_CallbackCommand;
	void freshColor();
	void changeRelativePositionDirection(int);
	void ModifyRelativePositionDistance(double dis);
};
