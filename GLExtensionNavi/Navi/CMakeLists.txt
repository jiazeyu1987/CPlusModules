
#-----------------------------------------------------------------------------
set(MODULE_NAME Navi)
set(MODULE_TITLE ${MODULE_NAME})

string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)

#-----------------------------------------------------------------------------
add_subdirectory(Logic)
add_subdirectory(Widgets)

#-----------------------------------------------------------------------------
set(MODULE_EXPORT_DIRECTIVE "Q_SLICER_QTMODULES_${MODULE_NAME_UPPER}_EXPORT")


set(GL_EFFECT_INCLUDE
	${Slicer_CMAKE_DIR}/../Modules/Loadable/Segmentations/EditorEffects
	${Slicer_CMAKE_DIR}/../S4D/Slicer-build/Modules/Loadable/Segmentations/EditorEffects
)
set(GL_DMRI_INCLUDE 
	)
set(MODULE_INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}/Logic
  ${CMAKE_CURRENT_BINARY_DIR}/Logic
  ${CMAKE_CURRENT_SOURCE_DIR}/Widgets
  ${CMAKE_CURRENT_BINARY_DIR}/Widgets
  ${GL_EFFECT_INCLUDE}
  ${Slicer_ModuleWidgets_INCLUDE_DIRS}
  ${Slicer_ModuleMRML_INCLUDE_DIRS}
  ${Slicer_ModuleLogic_INCLUDE_DIRS}
  ${vtkTeem_INCLUDE_DIRS}
  ${GL_DMRI_INCLUDE}
  ${Slicer_CMAKE_DIR}/../S4D/Slicer-build/Applications/SlicerApp
  ${Slicer_CMAKE_DIR}/../Applications/SlicerApp
  )

set(MODULE_SRCS
  qSlicer${MODULE_NAME}Module.cxx
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}ModuleWidget.cxx
  qSlicer${MODULE_NAME}ModuleWidget.h
  NaviWidget2D.h
  NaviWidget2D.cpp
  NaviBase2D.h
  NaviBase2D.cpp
  BuildPhaseStructrueView.cpp
  BuildPhaseStructrueView.h
  NaviWidgetPhase.cpp
  NaviWidgetPhase.h
  NaviWidget3D.cpp
  NaviWidget3D.h
  NaviWidgetStructure.cpp
  NaviWidgetStructure.h
  FiberData.h
  FiberData.cpp
  NaviWidgetFiber.h
  NaviWidgetFiber.cpp
  SwitchControl.cpp
  SwitchControl.h
  vtkEllipsoidSource.h
  vtkEllipsoidSource.cxx
  )

set(MODULE_MOC_SRCS
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}ModuleWidget.h
  NaviWidget2D.h
  NaviBase2D.h
  BuildPhaseStructrueView.h
  NaviWidgetPhase.h
  NaviWidget3D.h
  NaviWidgetStructure.h
  FiberData.h
  NaviWidgetFiber.h
  SwitchControl.h
  )

set(MODULE_UI_SRCS
  Resources/UI/qSlicer${MODULE_NAME}ModuleWidget.ui
  Resources/UI/NaviWidget2D.ui
  Resources/UI/BuildPhaseStructrueView.ui
  Resources/UI/NaviWidgetPhase.ui  
  Resources/UI/NaviWidgetStructure.ui  
  Resources/UI/NaviWidgetFiber.ui
  Resources/UI/NaviWidget3D.ui
  Resources/UI/NaviWidgetAblation.ui
  ) 


set(GL_DMRI 
	${Slicer_CMAKE_DIR}/../GitModule/SlicerDMRI/lib/Slicer-5.3/qt-loadable-modules/vtkDMRI.lib
	${Slicer_CMAKE_DIR}/../GitModule/SlicerDMRI/lib/Slicer-5.3/qt-loadable-modules/vtkSlicerTractographyDisplayModuleLogic.lib
	${Slicer_CMAKE_DIR}/../GitModule/SlicerDMRI/lib/Slicer-5.3/qt-loadable-modules/vtkSlicerTractographyDisplayModuleMRML.lib
	)
set(JLib 
	${Slicer_CMAKE_DIR}/../S4D/Slicer-build/lib/Slicer-5.2/Debug/qSlicerApp.lib)
	
set(MODULE_TARGET_LIBRARIES
  vtkSlicer${MODULE_NAME}ModuleLogic
  qSlicer${MODULE_NAME}ModuleWidgets
  qSlicerSegmentationsEditorEffects
  qSlicerSegmentationsModuleWidgets
  qSlicerVolumeRenderingModuleWidgets
  vtkSlicerVolumesModuleLogic 
  ${GL_DMRI}
  ${JLib}
  )

set(MODULE_RESOURCES
  Resources/qSlicer${MODULE_NAME}Module.qrc
  )


#-----------------------------------------------------------------------------
slicerMacroBuildLoadableModule(
  NAME ${MODULE_NAME}
  TITLE ${MODULE_TITLE}
  EXPORT_DIRECTIVE ${MODULE_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${MODULE_INCLUDE_DIRECTORIES}
  SRCS ${MODULE_SRCS}
  MOC_SRCS ${MODULE_MOC_SRCS}
  UI_SRCS ${MODULE_UI_SRCS}
  TARGET_LIBRARIES ${MODULE_TARGET_LIBRARIES}
  RESOURCES ${MODULE_RESOURCES}
  WITH_GENERIC_TESTS
  )

#-----------------------------------------------------------------------------
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
