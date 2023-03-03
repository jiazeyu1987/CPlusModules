cmake_minimum_required(VERSION 3.13.4)

project(GLExtensionPatient)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "https://www.slicer.org/wiki/Documentation/Nightly/Extensions/GLExtensionPatient")
set(EXTENSION_CATEGORY "GL")
set(EXTENSION_CONTRIBUTORS "JZY (GL)")
set(EXTENSION_DESCRIPTION "This is an example of a simple extension")
set(EXTENSION_ICONURL "http://www.example.com/Slicer/Extensions/GLExtensionPatient.png")
set(EXTENSION_SCREENSHOTURLS "http://www.example.com/Slicer/Extensions/GLExtensionPatient/Screenshots/1.png")
set(EXTENSION_DEPENDS "NA") # Specified as a list or "NA" if no dependencies

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(PatientManager)
## NEXT_MODULE

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
