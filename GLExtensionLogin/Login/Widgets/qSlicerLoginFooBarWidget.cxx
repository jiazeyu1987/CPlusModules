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
#include "qSlicerLoginFooBarWidget.h"
#include "ui_qSlicerLoginFooBarWidget.h"

#include "vtkSlicerMarkupsLogic.h"
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Login
class qSlicerLoginFooBarWidgetPrivate
  : public Ui_qSlicerLoginFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerLoginFooBarWidget);
protected:
  qSlicerLoginFooBarWidget* const q_ptr;

public:
  qSlicerLoginFooBarWidgetPrivate(
    qSlicerLoginFooBarWidget& object);
  virtual void setupUi(qSlicerLoginFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerLoginFooBarWidgetPrivate
::qSlicerLoginFooBarWidgetPrivate(
  qSlicerLoginFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerLoginFooBarWidgetPrivate
::setupUi(qSlicerLoginFooBarWidget* widget)
{
  this->Ui_qSlicerLoginFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerLoginFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerLoginFooBarWidget
::qSlicerLoginFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLoginFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerLoginFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerLoginFooBarWidget
::~qSlicerLoginFooBarWidget()
{
}
