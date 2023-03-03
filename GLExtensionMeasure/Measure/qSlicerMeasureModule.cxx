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

// Measure Logic includes
#include <vtkSlicerMeasureLogic.h>

// Measure includes
#include "qSlicerMeasureModule.h"
#include "qSlicerMeasureModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerMeasureModulePrivate
{
public:
  qSlicerMeasureModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerMeasureModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerMeasureModulePrivate::qSlicerMeasureModulePrivate()
{

}

//-----------------------------------------------------------------------------
// qSlicerMeasureModule methods

//-----------------------------------------------------------------------------
qSlicerMeasureModule::qSlicerMeasureModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerMeasureModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerMeasureModule::~qSlicerMeasureModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerMeasureModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerMeasureModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerMeasureModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerMeasureModule::icon() const
{
  return QIcon(":/Icons/Measure.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerMeasureModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerMeasureModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerMeasureModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerMeasureModule
::createWidgetRepresentation()
{
  return new qSlicerMeasureModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerMeasureModule::createLogic()
{
  return vtkSlicerMeasureLogic::New();
}
