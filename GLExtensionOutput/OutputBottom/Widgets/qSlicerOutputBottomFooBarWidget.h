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

#ifndef __qSlicerOutputBottomFooBarWidget_h
#define __qSlicerOutputBottomFooBarWidget_h

// Qt includes
#include <QWidget>

// FooBar Widgets includes
#include "qSlicerOutputBottomModuleWidgetsExport.h"

class qSlicerOutputBottomFooBarWidgetPrivate;

/// \ingroup Slicer_QtModules_OutputBottom
class Q_SLICER_MODULE_OUTPUTBOTTOM_WIDGETS_EXPORT qSlicerOutputBottomFooBarWidget
  : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  qSlicerOutputBottomFooBarWidget(QWidget *parent=0);
  ~qSlicerOutputBottomFooBarWidget() override;

protected slots:

protected:
  QScopedPointer<qSlicerOutputBottomFooBarWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerOutputBottomFooBarWidget);
  Q_DISABLE_COPY(qSlicerOutputBottomFooBarWidget);
};

#endif
