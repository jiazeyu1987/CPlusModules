cmake_minimum_required(VERSION 3.13.4)

project(GLExtensionFilter)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "https://www.slicer.org/wiki/Documentation/Nightly/Extensions/GLExtensionFilter")
set(EXTENSION_CATEGORY "GL")
set(EXTENSION_CONTRIBUTORS "JZY (GL)")
set(EXTENSION_DESCRIPTION "NONE")
set(EXTENSION_ICONURL "http://www.example.com/Slicer/Extensions/GLExtensionFilter.png")
set(EXTENSION_SCREENSHOTURLS "http://www.example.com/Slicer/Extensions/GLExtensionFilter/Screenshots/1.png")
set(EXTENSION_DEPENDS "NA") # Specified as a list or "NA" if no dependencies

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(FilterRight)
add_subdirectory(FilterLeft)
add_subdirectory(FilterTop)
## NEXT_MODULE

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
