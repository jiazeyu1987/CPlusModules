cmake_minimum_required(VERSION 3.13.4)

project(GLExtensionNavi)
  
#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "https://www.slicer.org/wiki/Documentation/Nightly/Extensions/GLExtensionNavi")
set(EXTENSION_CATEGORY "Examples")
set(EXTENSION_CONTRIBUTORS "John Doe (AnyWare Corp.)")
set(EXTENSION_DESCRIPTION "This is an example of a simple extension")
set(EXTENSION_ICONURL "http://www.example.com/Slicer/Extensions/GLExtensionNavi.png")
set(EXTENSION_SCREENSHOTURLS "http://www.example.com/Slicer/Extensions/GLExtensionNavi/Screenshots/1.png")
set(EXTENSION_DEPENDS "NA") # Specified as a list or "NA" if no dependencies

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})
 
# Sanity checks
if(NOT ${Slicer_USE_vtkTeem})
  message(FATAL_ERROR "SlicerDMRI requires Slicer_USE_vtkTeem")
endif()
#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(Navi)
## NEXT_MODULE

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
