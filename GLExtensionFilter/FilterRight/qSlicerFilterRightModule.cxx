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

// FilterRight Logic includes
#include <vtkSlicerFilterRightLogic.h>

// FilterRight includes
#include "qSlicerFilterRightModule.h"
#include "qSlicerFilterRightModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerFilterRightModulePrivate
{
public:
  qSlicerFilterRightModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerFilterRightModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerFilterRightModulePrivate::qSlicerFilterRightModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerFilterRightModule methods

//-----------------------------------------------------------------------------
qSlicerFilterRightModule::qSlicerFilterRightModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerFilterRightModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerFilterRightModule::~qSlicerFilterRightModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerFilterRightModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerFilterRightModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFilterRightModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerFilterRightModule::icon() const
{
  return QIcon(":/Icons/FilterRight.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerFilterRightModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFilterRightModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerFilterRightModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerFilterRightModule
::createWidgetRepresentation()
{
  return new qSlicerFilterRightModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerFilterRightModule::createLogic()
{
  return vtkSlicerFilterRightLogic::New();
}
