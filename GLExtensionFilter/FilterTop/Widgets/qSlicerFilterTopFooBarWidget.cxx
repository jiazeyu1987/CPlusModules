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
#include "qSlicerFilterTopFooBarWidget.h"
#include "ui_qSlicerFilterTopFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_FilterTop
class qSlicerFilterTopFooBarWidgetPrivate
  : public Ui_qSlicerFilterTopFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerFilterTopFooBarWidget);
protected:
  qSlicerFilterTopFooBarWidget* const q_ptr;

public:
  qSlicerFilterTopFooBarWidgetPrivate(
    qSlicerFilterTopFooBarWidget& object);
  virtual void setupUi(qSlicerFilterTopFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerFilterTopFooBarWidgetPrivate
::qSlicerFilterTopFooBarWidgetPrivate(
  qSlicerFilterTopFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerFilterTopFooBarWidgetPrivate
::setupUi(qSlicerFilterTopFooBarWidget* widget)
{
  this->Ui_qSlicerFilterTopFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerFilterTopFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerFilterTopFooBarWidget
::qSlicerFilterTopFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerFilterTopFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerFilterTopFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerFilterTopFooBarWidget
::~qSlicerFilterTopFooBarWidget()
{
}
