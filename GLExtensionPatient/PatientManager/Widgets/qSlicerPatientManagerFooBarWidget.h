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

#ifndef __qSlicerPatientManagerFooBarWidget_h
#define __qSlicerPatientManagerFooBarWidget_h

// Qt includes
#include <QWidget>

// FooBar Widgets includes
#include "qSlicerPatientManagerModuleWidgetsExport.h"

class qSlicerPatientManagerFooBarWidgetPrivate;

/// \ingroup Slicer_QtModules_PatientManager
class Q_SLICER_MODULE_PATIENTMANAGER_WIDGETS_EXPORT qSlicerPatientManagerFooBarWidget
  : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  qSlicerPatientManagerFooBarWidget(QWidget *parent=0);
  virtual ~qSlicerPatientManagerFooBarWidget();

protected slots:

protected:
  QScopedPointer<qSlicerPatientManagerFooBarWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerPatientManagerFooBarWidget);
  Q_DISABLE_COPY(qSlicerPatientManagerFooBarWidget);
};

#endif
