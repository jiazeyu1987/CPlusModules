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

#ifndef __qSlicerLoginFooBarWidget_h
#define __qSlicerLoginFooBarWidget_h

// Qt includes
#include <QWidget>

// FooBar Widgets includes
#include "qSlicerLoginModuleWidgetsExport.h"

class qSlicerLoginFooBarWidgetPrivate;

/// \ingroup Slicer_QtModules_Login
class Q_SLICER_MODULE_LOGIN_WIDGETS_EXPORT qSlicerLoginFooBarWidget
  : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  qSlicerLoginFooBarWidget(QWidget *parent=0);
  virtual ~qSlicerLoginFooBarWidget();

protected slots:

protected:
  QScopedPointer<qSlicerLoginFooBarWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerLoginFooBarWidget);
  Q_DISABLE_COPY(qSlicerLoginFooBarWidget);
};

#endif
