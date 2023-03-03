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
#include "qSlicerOutputFooBarWidget.h"
#include "ui_qSlicerOutputFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Output
class qSlicerOutputFooBarWidgetPrivate
  : public Ui_qSlicerOutputFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerOutputFooBarWidget);
protected:
  qSlicerOutputFooBarWidget* const q_ptr;

public:
  qSlicerOutputFooBarWidgetPrivate(
    qSlicerOutputFooBarWidget& object);
  virtual void setupUi(qSlicerOutputFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerOutputFooBarWidgetPrivate
::qSlicerOutputFooBarWidgetPrivate(
  qSlicerOutputFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerOutputFooBarWidgetPrivate
::setupUi(qSlicerOutputFooBarWidget* widget)
{
  this->Ui_qSlicerOutputFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerOutputFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerOutputFooBarWidget
::qSlicerOutputFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerOutputFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerOutputFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerOutputFooBarWidget
::~qSlicerOutputFooBarWidget()
{
}
