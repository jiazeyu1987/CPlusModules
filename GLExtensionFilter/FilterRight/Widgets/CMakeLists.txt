project(qSlicer${MODULE_NAME}ModuleWidgets)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "Q_SLICER_MODULE_${MODULE_NAME_UPPER}_WIDGETS_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
	${Slicer_CMAKE_DIR}/../S4D/Slicer-build/Applications/SlicerApp
	${Slicer_CMAKE_DIR}/../Applications/SlicerApp
  )

set(${KIT}_SRCS
  qSlicer${MODULE_NAME}FooBarWidget.cxx
  qSlicer${MODULE_NAME}FooBarWidget.h
  qGLChooseDialog.h
  qGLChooseDialog.cpp
  )

set(${KIT}_MOC_SRCS
  qSlicer${MODULE_NAME}FooBarWidget.h
  qGLChooseDialog.h 
  )

set(${KIT}_UI_SRCS
  ../Resources/UI/qSlicer${MODULE_NAME}FooBarWidget.ui
  ../Resources/UI/qGLChooseDialog.ui 
  )

set(${KIT}_RESOURCES
  ../Resources/qSlicer${MODULE_NAME}Module.qrc
  )

set(JLib 
	${Slicer_CMAKE_DIR}/../S4D/Slicer-build/lib/Slicer-5.2/Debug/qSlicerApp.lib)
set(${KIT}_TARGET_LIBRARIES
  vtkSlicer${MODULE_NAME}ModuleLogic
  ${JLib}
  )

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleWidgets(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  MOC_SRCS ${${KIT}_MOC_SRCS}
  UI_SRCS ${${KIT}_UI_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  RESOURCES ${${KIT}_RESOURCES}
  WRAP_PYTHONQT
  )
