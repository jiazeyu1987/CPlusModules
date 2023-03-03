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

// Navi Logic includes
#include <vtkSlicerNaviLogic.h>

// Navi includes
#include "qSlicerNaviModule.h"
#include "qSlicerNaviModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerNaviModulePrivate
{
public:
  qSlicerNaviModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerNaviModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerNaviModulePrivate::qSlicerNaviModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerNaviModule methods

//-----------------------------------------------------------------------------
qSlicerNaviModule::qSlicerNaviModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerNaviModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerNaviModule::~qSlicerNaviModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerNaviModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerNaviModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerNaviModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerNaviModule::icon() const
{
  return QIcon(":/Icons/Navi.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerNaviModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerNaviModule::dependencies() const
{
	QStringList moduleDependencies;
	moduleDependencies << "Colors" << "Markups"<<"Models"<<"Segmentations"<<"VolumeRendering"<<"Volumes";
	return moduleDependencies;
}

//-----------------------------------------------------------------------------
void qSlicerNaviModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerNaviModule
::createWidgetRepresentation()
{
  return new qSlicerNaviModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerNaviModule::createLogic()
{
  return vtkSlicerNaviLogic::New();
}
