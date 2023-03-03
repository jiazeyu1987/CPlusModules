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

#ifndef __qSlicerFilterLeftFooBarWidget_h
#define __qSlicerFilterLeftFooBarWidget_h

// Qt includes
#include <QWidget>

// FooBar Widgets includes
#include "qSlicerFilterLeftModuleWidgetsExport.h"

class qSlicerFilterLeftFooBarWidgetPrivate;

/// \ingroup Slicer_QtModules_FilterLeft
class Q_SLICER_MODULE_FILTERLEFT_WIDGETS_EXPORT qSlicerFilterLeftFooBarWidget
  : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  qSlicerFilterLeftFooBarWidget(QWidget *parent=0);
  virtual ~qSlicerFilterLeftFooBarWidget();

protected slots:

protected:
  QScopedPointer<qSlicerFilterLeftFooBarWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerFilterLeftFooBarWidget);
  Q_DISABLE_COPY(qSlicerFilterLeftFooBarWidget);
};

#endif
