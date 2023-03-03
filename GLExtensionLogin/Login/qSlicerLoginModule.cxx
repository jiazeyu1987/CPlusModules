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

// Login Logic includes
#include <vtkSlicerLoginLogic.h>

// Login includes
#include "qSlicerLoginModule.h"
#include "qSlicerLoginModuleWidget.h"


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerLoginModulePrivate
{
public:
  qSlicerLoginModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerLoginModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerLoginModulePrivate::qSlicerLoginModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerLoginModule methods

//-----------------------------------------------------------------------------
qSlicerLoginModule::qSlicerLoginModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerLoginModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerLoginModule::~qSlicerLoginModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerLoginModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerLoginModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerLoginModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerLoginModule::icon() const
{
  return QIcon(":/Icons/Login.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerLoginModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerLoginModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerLoginModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerLoginModule
::createWidgetRepresentation()
{
  return new qSlicerLoginModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerLoginModule::createLogic()
{
  return vtkSlicerLoginLogic::New();
}
