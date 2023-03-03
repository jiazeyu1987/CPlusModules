project(vtkSlicer${MODULE_NAME}ModuleLogic)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "VTK_SLICER_${MODULE_NAME_UPPER}_MODULE_LOGIC_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
${Slicer_CMAKE_DIR}/../S4D/Slicer-build/Applications/SlicerApp
  ${Slicer_CMAKE_DIR}/../Applications/SlicerApp
  )
 
set(${KIT}_SRCS
  vtkSlicer${MODULE_NAME}Logic.cxx
  vtkSlicer${MODULE_NAME}Logic.h
  vtkMitkThickSlicesFilter.h
  vtkMitkThickSlicesFilter.cpp
  DoubleSlider.h
  DoubleSlider.cpp
  itkBinaryThinningImageFilter3D.h
  itkBinaryThinningImageFilter3D.hxx
  )
 
 set(JLib 
	${Slicer_CMAKE_DIR}/../S4D/Slicer-build/lib/Slicer-5.2/Debug/qSlicerApp.lib)
set(${KIT}_TARGET_LIBRARIES
vtkSlicerMarkupsModuleMRML
vtkSlicerModelsModuleLogic 
vtkSlicerSegmentationsModuleLogic
  ${ITK_LIBRARIES}
  ${JLib}
  )

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleLogic(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )
