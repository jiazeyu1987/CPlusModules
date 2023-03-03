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

#ifndef __qSlicerMeasureModuleWidget_h
#define __qSlicerMeasureModuleWidget_h

// Slicer includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerMeasureModuleExport.h"
#include "MeasureToolPanel.h"
#include "qSlicerGLModuleWidget.h"
class qSlicerMeasureModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_MEASURE_EXPORT qSlicerMeasureModuleWidget :
  public qSlicerGLModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerGLModuleWidget Superclass;
  qSlicerMeasureModuleWidget(QWidget *parent=0);
  virtual ~qSlicerMeasureModuleWidget();
public slots:
	void onActiveMarkupMRMLNodeAdded(vtkMRMLNode* markupsNode);
	
protected:
  QScopedPointer<qSlicerMeasureModuleWidgetPrivate> d_ptr;

  virtual void setup();

private:
  Q_DECLARE_PRIVATE(qSlicerMeasureModuleWidget);
  Q_DISABLE_COPY(qSlicerMeasureModuleWidget);

protected:
	std::vector<std::string> m_NodeVector;
	int m_current_front_view_direction = 0;
	int m_current_view = 0;
	int m_SelectedTumorIndex = 0;
	std::string m_Type = "AuxType1";
	MeasureToolPanel* measurePanel = nullptr;
	Q_INVOKABLE virtual void enter() override;
private Q_SLOTS:
	void OpenMeasureToolPanel(bool checked);
	void OnRotate(bool checked);
	void OnBasicPoint(bool checked);
	void OnZoomOut(bool checked);
	void OnTranslate(bool checked);
	void OnSceenShot(bool checked);
	void OnScreenShot();
	void OnViewType1();
	void OnFresh();
	void OnViewType2();
	void OnViewType0();
	void onSliceCompositeNodeModified();
	void OnSignalChangeLayoutTo3();
	void OnSignalChangeLayoutTo4();
	void OnSignalChangeLayoutTo5();
	void OnSignalChangeLayoutTo6();

	void OnResetButton(QString);
	void OnPreOperation(QString,bool);
	void OnPostOperation(QString,bool);
	void OnReset();
	void OnGrayScale(bool checked);
	void OnDrawLineTriggered();
	void OnDrawPolygonTriggered();
	void OnDrawAngleTriggered();
	void OnDrawPointTriggered();
	void OnDrawROITriggered();
	void OnDrawPlaneTriggered();
	void Ontimeout();
	void OnOpenACPCPanel();
	void OnMeasureShowOff(bool);
private:
	QTimer* m_timer;
};

#endif
