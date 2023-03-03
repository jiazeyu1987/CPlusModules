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

// TopMenu Logic includes
#include <vtkSlicerTopMenuLogic.h>

// TopMenu includes
#include "qSlicerTopMenuModule.h"
#include "qSlicerTopMenuModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerTopMenuModulePrivate
{
public:
  qSlicerTopMenuModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerTopMenuModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerTopMenuModulePrivate::qSlicerTopMenuModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerTopMenuModule methods

//-----------------------------------------------------------------------------
qSlicerTopMenuModule::qSlicerTopMenuModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerTopMenuModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerTopMenuModule::~qSlicerTopMenuModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerTopMenuModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerTopMenuModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerTopMenuModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerTopMenuModule::icon() const
{
  return QIcon(":/Icons/TopMenu.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerTopMenuModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerTopMenuModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerTopMenuModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerTopMenuModule
::createWidgetRepresentation()
{
  return new qSlicerTopMenuModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerTopMenuModule::createLogic()
{
  return vtkSlicerTopMenuLogic::New();
}
