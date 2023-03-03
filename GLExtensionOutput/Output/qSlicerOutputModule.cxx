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

// Output Logic includes
#include <vtkSlicerOutputLogic.h>

// Output includes
#include "qSlicerOutputModule.h"
#include "qSlicerOutputModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerOutputModulePrivate
{
public:
  qSlicerOutputModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerOutputModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerOutputModulePrivate::qSlicerOutputModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerOutputModule methods

//-----------------------------------------------------------------------------
qSlicerOutputModule::qSlicerOutputModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerOutputModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerOutputModule::~qSlicerOutputModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerOutputModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerOutputModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerOutputModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerOutputModule::icon() const
{
  return QIcon(":/Icons/Output.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerOutputModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerOutputModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerOutputModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerOutputModule
::createWidgetRepresentation()
{
  return new qSlicerOutputModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerOutputModule::createLogic()
{
  return vtkSlicerOutputLogic::New();
}
