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

// FilterLeft Logic includes
#include <vtkSlicerFilterLeftLogic.h>

// FilterLeft includes
#include "qSlicerFilterLeftModule.h"
#include "qSlicerFilterLeftModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerFilterLeftModulePrivate
{
public:
  qSlicerFilterLeftModulePrivate();
  qSlicerFilterLeftModuleWidget* Widget;
};

//-----------------------------------------------------------------------------
// qSlicerFilterLeftModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerFilterLeftModulePrivate::qSlicerFilterLeftModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerFilterLeftModule methods

//-----------------------------------------------------------------------------
qSlicerFilterLeftModule::qSlicerFilterLeftModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerFilterLeftModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerFilterLeftModule::~qSlicerFilterLeftModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerFilterLeftModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerFilterLeftModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFilterLeftModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerFilterLeftModule::icon() const
{
  return QIcon(":/Icons/FilterLeft.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerFilterLeftModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFilterLeftModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerFilterLeftModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerFilterLeftModule
::createWidgetRepresentation()
{
  Q_D(qSlicerFilterLeftModule);
  d->Widget = new qSlicerFilterLeftModuleWidget;
  //connect(d->Widget, SIGNAL(loadfile()), this, SLOT(onLoadFile()));
  return d->Widget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerFilterLeftModule::createLogic()
{
  return vtkSlicerFilterLeftLogic::New();
}

