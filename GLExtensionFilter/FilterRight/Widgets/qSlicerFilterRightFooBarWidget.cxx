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
#include "qSlicerFilterRightFooBarWidget.h"
#include "ui_qSlicerFilterRightFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_FilterRight
class qSlicerFilterRightFooBarWidgetPrivate
  : public Ui_qSlicerFilterRightFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerFilterRightFooBarWidget);
protected:
  qSlicerFilterRightFooBarWidget* const q_ptr;

public:
  qSlicerFilterRightFooBarWidgetPrivate(
    qSlicerFilterRightFooBarWidget& object);
  virtual void setupUi(qSlicerFilterRightFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerFilterRightFooBarWidgetPrivate
::qSlicerFilterRightFooBarWidgetPrivate(
  qSlicerFilterRightFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerFilterRightFooBarWidgetPrivate
::setupUi(qSlicerFilterRightFooBarWidget* widget)
{
  this->Ui_qSlicerFilterRightFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerFilterRightFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerFilterRightFooBarWidget
::qSlicerFilterRightFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerFilterRightFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerFilterRightFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerFilterRightFooBarWidget
::~qSlicerFilterRightFooBarWidget()
{
}
