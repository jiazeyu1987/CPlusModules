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

#ifndef __qSlicerOutputModuleWidget_h
#define __qSlicerOutputModuleWidget_h

// Slicer includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerOutputModuleExport.h"
#include "qSlicerGLModuleWidget.h"

#include "qSurgeryPlanReportWidget.h"

class qSlicerOutputModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_OUTPUT_EXPORT qSlicerOutputModuleWidget :
  public qSlicerGLModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerGLModuleWidget Superclass;
  qSlicerOutputModuleWidget(QWidget *parent=0);
  virtual ~qSlicerOutputModuleWidget();
  Q_INVOKABLE virtual void enter() override;
  Q_INVOKABLE virtual void exit() override;
private slots :
	void onGotoPreviewPage();
	//void OnIdentifyHeadFramePosition();
	void OnIdentifyHeadFramePositionNew();
	void OnDisplayModelChange(bool);
	void OnPDFExport();
	void OnResultExport();
	void SetIdentifyType(int num);
	void ShowSelectSeries(int num);
	void DoFreshFiberInfo();
	void DoFreshTumorInfo();
protected:
  QScopedPointer<qSlicerOutputModuleWidgetPrivate> d_ptr;

  void setup() override;

public:
	void InitInfo();
private:
  Q_DECLARE_PRIVATE(qSlicerOutputModuleWidget);
  Q_DISABLE_COPY(qSlicerOutputModuleWidget);
};

#endif
