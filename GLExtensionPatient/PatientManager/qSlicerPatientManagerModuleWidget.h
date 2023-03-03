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

#ifndef __qSlicerPatientManagerModuleWidget_h
#define __qSlicerPatientManagerModuleWidget_h

// Slicer includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerPatientManagerModuleExport.h"
#include "qGLPatientMangeView.h"

class qSlicerPatientManagerModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_PATIENTMANAGER_EXPORT qSlicerPatientManagerModuleWidget :
  public qSlicerGLModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerGLModuleWidget Superclass;
  qSlicerPatientManagerModuleWidget(QWidget *parent=0);
  virtual ~qSlicerPatientManagerModuleWidget();
  Q_INVOKABLE virtual void enter() override;
  Q_INVOKABLE virtual void exit() override;
public slots:
	void onGotoNextPage();
	void onGotoPreviewPage();
protected:
  QScopedPointer<qSlicerPatientManagerModuleWidgetPrivate> d_ptr;
  bool AutoTestBreak(int);
  virtual void setup();
  void DoReinitScene();
private:
  patientMangeView* pmv;
  Q_DECLARE_PRIVATE(qSlicerPatientManagerModuleWidget);
  Q_DISABLE_COPY(qSlicerPatientManagerModuleWidget);

  friend class patientMangeView;
};

#endif
