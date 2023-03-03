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
#include "qSlicerTopMenuFooBarWidget.h"
#include "ui_qSlicerTopMenuFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_TopMenu
class qSlicerTopMenuFooBarWidgetPrivate
  : public Ui_qSlicerTopMenuFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerTopMenuFooBarWidget);
protected:
  qSlicerTopMenuFooBarWidget* const q_ptr;

public:
  qSlicerTopMenuFooBarWidgetPrivate(
    qSlicerTopMenuFooBarWidget& object);
  virtual void setupUi(qSlicerTopMenuFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerTopMenuFooBarWidgetPrivate
::qSlicerTopMenuFooBarWidgetPrivate(
  qSlicerTopMenuFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerTopMenuFooBarWidgetPrivate
::setupUi(qSlicerTopMenuFooBarWidget* widget)
{
  this->Ui_qSlicerTopMenuFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerTopMenuFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerTopMenuFooBarWidget
::qSlicerTopMenuFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerTopMenuFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerTopMenuFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerTopMenuFooBarWidget
::~qSlicerTopMenuFooBarWidget()
{
}
