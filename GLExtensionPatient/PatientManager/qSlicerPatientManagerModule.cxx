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

// PatientManager Logic includes
#include <vtkSlicerPatientManagerLogic.h>

// PatientManager includes
#include "qSlicerPatientManagerModule.h"
#include "qSlicerPatientManagerModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerPatientManagerModulePrivate
{
public:
  qSlicerPatientManagerModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerPatientManagerModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerPatientManagerModulePrivate::qSlicerPatientManagerModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerPatientManagerModule methods

//-----------------------------------------------------------------------------
qSlicerPatientManagerModule::qSlicerPatientManagerModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerPatientManagerModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerPatientManagerModule::~qSlicerPatientManagerModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerPatientManagerModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerPatientManagerModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerPatientManagerModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerPatientManagerModule::icon() const
{
  return QIcon(":/Icons/PatientManager.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerPatientManagerModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerPatientManagerModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerPatientManagerModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerPatientManagerModule
::createWidgetRepresentation()
{
  return new qSlicerPatientManagerModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerPatientManagerModule::createLogic()
{
  return vtkSlicerPatientManagerLogic::New();
}
