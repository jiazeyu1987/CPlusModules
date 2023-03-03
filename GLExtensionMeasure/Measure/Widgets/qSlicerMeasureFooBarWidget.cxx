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
#include "qSlicerMeasureFooBarWidget.h"
#include "ui_qSlicerMeasureFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Measure
class qSlicerMeasureFooBarWidgetPrivate
  : public Ui_qSlicerMeasureFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerMeasureFooBarWidget);
protected:
  qSlicerMeasureFooBarWidget* const q_ptr;

public:
  qSlicerMeasureFooBarWidgetPrivate(
    qSlicerMeasureFooBarWidget& object);
  virtual void setupUi(qSlicerMeasureFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerMeasureFooBarWidgetPrivate
::qSlicerMeasureFooBarWidgetPrivate(
  qSlicerMeasureFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerMeasureFooBarWidgetPrivate
::setupUi(qSlicerMeasureFooBarWidget* widget)
{
  this->Ui_qSlicerMeasureFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerMeasureFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerMeasureFooBarWidget
::qSlicerMeasureFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerMeasureFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerMeasureFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerMeasureFooBarWidget
::~qSlicerMeasureFooBarWidget()
{
}
