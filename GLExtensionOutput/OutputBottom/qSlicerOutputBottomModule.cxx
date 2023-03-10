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

// OutputBottom Logic includes
#include <vtkSlicerOutputBottomLogic.h>

// OutputBottom includes
#include "qSlicerOutputBottomModule.h"
#include "qSlicerOutputBottomModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerOutputBottomModulePrivate
{
public:
  qSlicerOutputBottomModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerOutputBottomModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerOutputBottomModulePrivate::qSlicerOutputBottomModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerOutputBottomModule methods

//-----------------------------------------------------------------------------
qSlicerOutputBottomModule::qSlicerOutputBottomModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerOutputBottomModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerOutputBottomModule::~qSlicerOutputBottomModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerOutputBottomModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerOutputBottomModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerOutputBottomModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerOutputBottomModule::icon() const
{
  return QIcon(":/Icons/OutputBottom.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerOutputBottomModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerOutputBottomModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerOutputBottomModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerOutputBottomModule
::createWidgetRepresentation()
{
  return new qSlicerOutputBottomModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerOutputBottomModule::createLogic()
{
  return vtkSlicerOutputBottomLogic::New();
}
