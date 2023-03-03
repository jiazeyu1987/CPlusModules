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

#ifndef __qSlicerOutputBottomModuleWidget_h
#define __qSlicerOutputBottomModuleWidget_h
#include <iostream>
using namespace std;
// Slicer includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerOutputBottomModuleExport.h"
#include "ResultFiberItem.h"
#include "qSlicerGLModuleWidget.h"
#include "vtkMRMLLayoutNode.h"
#include "QTimer.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLMarkupsFiducialNode.h"
#include "vtkMRMLMarkupsNode.h"
class qSlicerOutputBottomModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_OUTPUTBOTTOM_EXPORT qSlicerOutputBottomModuleWidget :
  public qSlicerGLModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerGLModuleWidget Superclass;
  qSlicerOutputBottomModuleWidget(QWidget *parent=0);
  virtual ~qSlicerOutputBottomModuleWidget();
  void DoFreshFiberList();
public slots:
	void onGotoPreviewPage();
	void onGotoNextPage();


protected:
  QScopedPointer<qSlicerOutputBottomModuleWidgetPrivate> d_ptr;
  Q_INVOKABLE virtual void enter() override;
  Q_INVOKABLE virtual void exit() override;
  void setup() override;

private:
	void AddFiberItemToList(FiberOutPutInfo1* fiber);
	//Initialize the fiber information
	void InitFiberInfo();

private:
  Q_DECLARE_PRIVATE(qSlicerOutputBottomModuleWidget);
  Q_DISABLE_COPY(qSlicerOutputBottomModuleWidget);
};

#endif
