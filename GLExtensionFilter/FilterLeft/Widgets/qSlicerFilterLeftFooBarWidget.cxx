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
#include "qSlicerFilterLeftFooBarWidget.h"
#include "ui_qSlicerFilterLeftFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_FilterLeft
class qSlicerFilterLeftFooBarWidgetPrivate
  : public Ui_qSlicerFilterLeftFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerFilterLeftFooBarWidget);
protected:
  qSlicerFilterLeftFooBarWidget* const q_ptr;

public:
  qSlicerFilterLeftFooBarWidgetPrivate(
    qSlicerFilterLeftFooBarWidget& object);
  virtual void setupUi(qSlicerFilterLeftFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerFilterLeftFooBarWidgetPrivate
::qSlicerFilterLeftFooBarWidgetPrivate(
  qSlicerFilterLeftFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerFilterLeftFooBarWidgetPrivate
::setupUi(qSlicerFilterLeftFooBarWidget* widget)
{
  this->Ui_qSlicerFilterLeftFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerFilterLeftFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerFilterLeftFooBarWidget
::qSlicerFilterLeftFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerFilterLeftFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerFilterLeftFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerFilterLeftFooBarWidget
::~qSlicerFilterLeftFooBarWidget()
{
}
