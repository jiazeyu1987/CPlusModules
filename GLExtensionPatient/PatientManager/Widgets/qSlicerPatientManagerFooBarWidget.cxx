/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerPatientManagerFooBarWidget.h"
#include "ui_qSlicerPatientManagerFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_PatientManager
class qSlicerPatientManagerFooBarWidgetPrivate
  : public Ui_qSlicerPatientManagerFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerPatientManagerFooBarWidget);
protected:
  qSlicerPatientManagerFooBarWidget* const q_ptr;

public:
  qSlicerPatientManagerFooBarWidgetPrivate(
    qSlicerPatientManagerFooBarWidget& object);
  virtual void setupUi(qSlicerPatientManagerFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerPatientManagerFooBarWidgetPrivate
::qSlicerPatientManagerFooBarWidgetPrivate(
  qSlicerPatientManagerFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerPatientManagerFooBarWidgetPrivate
::setupUi(qSlicerPatientManagerFooBarWidget* widget)
{
  this->Ui_qSlicerPatientManagerFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerPatientManagerFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerPatientManagerFooBarWidget
::qSlicerPatientManagerFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerPatientManagerFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerPatientManagerFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerPatientManagerFooBarWidget
::~qSlicerPatientManagerFooBarWidget()
{
}
