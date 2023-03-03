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

#ifndef __qSlicerOutputFooBarWidget_h
#define __qSlicerOutputFooBarWidget_h

// Qt includes
#include <QWidget>

// FooBar Widgets includes
#include "qSlicerOutputModuleWidgetsExport.h"

class qSlicerOutputFooBarWidgetPrivate;

/// \ingroup Slicer_QtModules_Output
class Q_SLICER_MODULE_OUTPUT_WIDGETS_EXPORT qSlicerOutputFooBarWidget
  : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  qSlicerOutputFooBarWidget(QWidget *parent=0);
  ~qSlicerOutputFooBarWidget() override;

protected slots:

protected:
  QScopedPointer<qSlicerOutputFooBarWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerOutputFooBarWidget);
  Q_DISABLE_COPY(qSlicerOutputFooBarWidget);
};

#endif
