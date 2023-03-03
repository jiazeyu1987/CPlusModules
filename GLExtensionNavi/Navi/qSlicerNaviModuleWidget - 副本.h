/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#ifndef __qSlicerNaviModuleWidget_h
#define __qSlicerNaviModuleWidget_h

// Slicer includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerNaviModuleExport.h"
#include "qlistwidget.h"
#include "qMRMLNodeComboBox.h"
#include "qpushbutton.h"
#include "qSlicerGLModuleWidget.h"
#include "QGroupBox.h"
#include "QButtonGroup.h"
#include "ctkFlowLayout.h"
#include "NaviBase2D.h"
#include "FiberData.h"
#include "NaviWidget3D.h"
#include "NaviWidget2D.h"
#include "NaviWidgetFiber.h"
#include "vtkMRMLVolumeRenderingDisplayNode.h"
#include "vtkMRMLAnnotationROINode.h"
#include "qMRMLSliceView.h"
#include "vtkRenderWidget.h"
#include "qmainwindow.h"
#include "qSlicerMainWindow.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"
class qSlicerNaviModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_NAVI_EXPORT qSlicerNaviModuleWidget :
  public qSlicerGLModuleWidget
{
  Q_OBJECT

public: 

  typedef qSlicerGLModuleWidget Superclass;
  qSlicerNaviModuleWidget(QWidget *parent=0);
  virtual ~qSlicerNaviModuleWidget();

  /**
	初始化的数据包括 
	1.vtkMRMLScalarVolumeNode 用来初始化2D列表
	2.Segment								  用来初始化3D列表
	3.XXXX									  用来初始化光纤列表
  */
  Q_INVOKABLE virtual void enter() override;
  Q_INVOKABLE virtual void exit() override;
protected:
	QScopedPointer<qSlicerNaviModuleWidgetPrivate> d_ptr;
	virtual void setup();
	bool AutoTestBreak(int i);
	void DoGetFiberWidgets();
private:
	Q_DECLARE_PRIVATE(qSlicerNaviModuleWidget);
	Q_DISABLE_COPY(qSlicerNaviModuleWidget);
public slots:
	void onGotoPreviewPage();
	void onGotoNextPage();
	void onChooseItem(QListWidgetItem*);
	void onItemRongjiChanged();
	void onEnterInit();
	void onHDBrainExtract();
	void onLogicModified(vtkObject* caller, vtkObject* obj);
	void onTest2();
	void onTest3();
	void onTest4();
	void onTest5();
	void onTest6();
	void onTest7();
	void onTest8();
	void onTest9();
	void SetCurrent2DDisplay();
	void update_ablation_number();
	void onAblationEstimate();
	void SetCurrent3DDisplay();
	void onInCreaseAblationDistance();
	void onDeCreaseAblationDistance();
	void onUpdateAblationAreaInfo(int num);
	void OnBrainParcellation();
	void onCreateSegmentPipeline(int);
	void onSegmentAdded(vtkObject*, void*);
	void onSegmentRemoved();
	void onSegmentModified();
	void onSegment3DCancel();
	void OnWaveChangeClick();
	void onSegment3DConfirm();
	void onRemoveSegment(NaviWidget3D* widget3d);
	void onStateChanged(int state);
	void onStateChanged2D(int state);
	void onFusionRatioChanged(int);
	void onItemOpacityChanged(int);
	void onNewOpticalFiber();
	void onFiberDirection();
	void onAddAblationArea();
	void onRemoveAblationArea();
	void onAdjustFiberPoint();
	void DoCenterTargetPoint();
	void DoFreshFiberListView();
	void onNormalViewDirection();
	void onChangeViewDirection(int);
	void OnDeleteFiberWidget(NaviWidgetFiber* fiber);
	void on_rotate_to_vector(double*  m_PointInput,double* m_Point);
	void on_fiber_depth(NaviWidgetFiber*, int);
	void on_estimate(NaviWidgetFiber*);
	void DoVolumeMapImageData(vtkMRMLScalarVolumeNode* volume, vtkImageData* imageData, vtkMatrix4x4* matrix);
	void DoVolumeMapImageData(vtkMRMLScalarVolumeNode* volume, vtkOrientedImageData* imageData);
	void DoVolumeMapModel(vtkMRMLScalarVolumeNode* volume, vtkMRMLModelNode* m_ModelNode);
	void DoClearVolume(vtkMRMLScalarVolumeNode* volume);
	void DoStatisticVolume(vtkMRMLScalarVolumeNode* volume, double bounds[6]);
	void FitROIToVolume(vtkMRMLVolumeRenderingDisplayNode* vspNode);
	void onReturnLastPage();
	void onAblationArea();
	void onPathAdjustment();
	void onAllUnvisible();
	void onoperation();
	void onbacktab0();
	void UpdateListWidget(NaviWidget2D*);
	void OnVisibleChagned(NaviWidget2D*);
	void on_click_fiber_widget(NaviWidgetFiber* widget);
	void DoCenterInputPoint();
	void onLeksellRecognize();
	void UpdateFiberList(NaviWidgetFiber* fiber);
protected:
	QString GetSegmentStastics(vtkMRMLSegmentationNode* snode);
  void DoInitTemp3DWidget();
  NaviWidgetFiber* GetSelectedFiberWidget();
  void onActiveMarkupMRMLNodeAdded(vtkMRMLNode* markupsNode);
  vtkSmartPointer<vtkImageData> polyDataToImageData(vtkSmartPointer<vtkPolyData> polydata,double spacing[3]);
  
  QVector<FiberData*> FiberDataVector;
  QVector< NaviWidgetFiber*> TestFiberWidgetVector;
  NaviBase2D* m_Rongji1 = nullptr;
  NaviBase2D* m_Rongji0 = nullptr;
  vtkMRMLScene* m_PresetScene = nullptr;
  enum State{
	  None = 0x01,
	  WaitForAddSegment = 0x02,
	  HoldSegment = 0x04
  };
  int m_State = qSlicerNaviModuleWidget::None;
  std::vector<NaviBase2D*> Vector2D;
  QVector<Widget3DData*> ViewWidgetVector3D;
  QMap< QListWidgetItem*, NaviWidgetFiber*> m_WidgetItem2Fiber;
public:
	vtkNew<vtkCallbackCommand> m_CallbackCommand;
	static void processEvents(vtkObject* caller, unsigned long eid, void* clientData, void* callData);
private:
	vtkMRMLVolumeRenderingDisplayNode* GetVolumeRenderingDisplayNode(vtkMRMLVolumeNode* node);
	void doTestAddData();
	void DoUpdateToServer();
	void DoFresh2DListView();
	void DoFresh3DListView();
	void DoFreshStructureListView();
	void DoFreshPhaseListView();
	bool isHiddenVolume = false;
	QListWidgetItem* m_LastVolListWidget=nullptr;
	QListWidgetItem* m_LastFiberItem = nullptr;
	//QMap<std::string, bool> mapIsVolumeBtnPressed;
	
};

#endif
