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

// FilterTop Logic includes
#include <vtkSlicerFilterTopLogic.h>

// FilterTop includes
#include "qSlicerFilterTopModule.h"
#include "qSlicerFilterTopModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerFilterTopModulePrivate
{
public:
  qSlicerFilterTopModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerFilterTopModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerFilterTopModulePrivate::qSlicerFilterTopModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerFilterTopModule methods

//-----------------------------------------------------------------------------
qSlicerFilterTopModule::qSlicerFilterTopModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerFilterTopModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerFilterTopModule::~qSlicerFilterTopModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerFilterTopModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerFilterTopModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFilterTopModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerFilterTopModule::icon() const
{
  return QIcon(":/Icons/FilterTop.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerFilterTopModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFilterTopModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerFilterTopModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerFilterTopModule
::createWidgetRepresentation()
{
  return new qSlicerFilterTopModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerFilterTopModule::createLogic()
{
  return vtkSlicerFilterTopLogic::New();
}
