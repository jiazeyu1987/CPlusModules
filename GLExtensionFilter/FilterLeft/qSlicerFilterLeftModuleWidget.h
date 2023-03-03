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

#ifndef __qSlicerFilterLeftModuleWidget_h
#define __qSlicerFilterLeftModuleWidget_h

// Slicer includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerFilterLeftModuleExport.h"
#include "qSlicerGLModuleWidget.h"
class qSlicerFilterLeftModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_FILTERLEFT_EXPORT qSlicerFilterLeftModuleWidget :
	public qSlicerGLModuleWidget
{
	Q_OBJECT

public:

	typedef qSlicerGLModuleWidget Superclass;
	qSlicerFilterLeftModuleWidget(QWidget* parent = 0);
	virtual ~qSlicerFilterLeftModuleWidget();

public slots:
	void OnNodeSelected(vtkObject* appLogic, void* data, unsigned long event);
	void onLogicModified();
	void OnRemoveClick();
	void onLoadPatientFiles();
	void onLoadDTI();

protected:
	QScopedPointer<qSlicerFilterLeftModuleWidgetPrivate> d_ptr;

	virtual void setup();
signals:
	void loadfile();
	
private:
	Q_DECLARE_PRIVATE(qSlicerFilterLeftModuleWidget);
	Q_DISABLE_COPY(qSlicerFilterLeftModuleWidget);
};

#endif
