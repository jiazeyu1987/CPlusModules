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
#include "qSlicerOutputBottomFooBarWidget.h"
#include "ui_qSlicerOutputBottomFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_OutputBottom
class qSlicerOutputBottomFooBarWidgetPrivate
  : public Ui_qSlicerOutputBottomFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerOutputBottomFooBarWidget);
protected:
  qSlicerOutputBottomFooBarWidget* const q_ptr;

public:
  qSlicerOutputBottomFooBarWidgetPrivate(
    qSlicerOutputBottomFooBarWidget& object);
  virtual void setupUi(qSlicerOutputBottomFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerOutputBottomFooBarWidgetPrivate
::qSlicerOutputBottomFooBarWidgetPrivate(
  qSlicerOutputBottomFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerOutputBottomFooBarWidgetPrivate
::setupUi(qSlicerOutputBottomFooBarWidget* widget)
{
  this->Ui_qSlicerOutputBottomFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerOutputBottomFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerOutputBottomFooBarWidget
::qSlicerOutputBottomFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerOutputBottomFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerOutputBottomFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerOutputBottomFooBarWidget
::~qSlicerOutputBottomFooBarWidget()
{
}
