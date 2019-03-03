cmake_minimum_required(VERSION 3.13.1 FATAL_ERROR)

add_definitions(-DWelderRevisionId=${WELDER_REVISION})
add_definitions(-DWelderShortChangeSet=${WELDER_SHORT_CHANGESET})
add_definitions(-DWelderChangeSet=${WELDER_CHANGESET})
add_definitions(-DWelderChangeSetDate=${WELDER_CHANGESET_DATE})

set(CMAKE_CONFIGURATION_TYPES Debug RelWithDebInfo Release MinSizeRel)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT WelderEditor)

project(Welder)
add_definitions(-DPLATFORM_ARCHITECTURE="${WELDER_ARCHITECTURE}")

set(WELDER_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(WELDER_CMAKE_DIR ${WELDER_CORE_DIR}/CMakeFiles/)
set(WELDER_LIBRARIES_DIR ${WELDER_CORE_DIR}/Libraries/)
set(WELDER_PLATFORM_DATA_DIR ${WELDER_CORE_DIR}/PlatformData/${WELDER_TOOLCHAIN})
set(WELDER_VIRTUAL_FILE_SYSTEM_ZIP ${CMAKE_BINARY_DIR}/FileSystem.zip)



################################################################################
# Set up options for projects depending on various ZeroCore features.
################################################################################

# Helper for Option sets, this will turn any builds not turned on by the option
# sets off.
function(welder_turn_remaining_builds_off)
    # ZeroLibraries
    option(WELDER_BUILD_Common "Build the Common project" OFF)
    option(WELDER_BUILD_Dash "Build the Dash project" OFF)
    option(WELDER_BUILD_Geometry "Build the Geometry project" OFF)
    option(WELDER_BUILD_Meta "Build the Meta project" OFF)
    option(WELDER_BUILD_Platform "Build the Platform project" OFF)
    option(WELDER_BUILD_Serialization "Build the Serialization project" OFF)
    option(WELDER_BUILD_SpatialPartition "Build the SpatialPartition project" OFF)
    option(WELDER_BUILD_Support "Build the Support project" OFF)
    option(WELDER_BUILD_Zilch "Build the Zilch project" OFF)

    # Projects
    option(WELDER_BUILD_ZeroEditor "Build the ZeroEditor project" OFF)
    option(WELDER_BUILD_ZeroLauncher "Build the ZeroLauncher project" OFF)
    option(WELDER_BUILD_ZeroLauncherSharedLibrary "Build the ZeroLauncherSharedLibrary project" OFF)
    option(WELDER_BUILD_BrowserSubProcess "Build the BrowserSubProcess project" OFF)

    # Systems
    option(WELDER_BUILD_Content "Build the Content project" OFF)
    option(WELDER_BUILD_Engine "Build the Engine project" OFF)
    option(WELDER_BUILD_Graphics "Build the Graphics project" OFF)
    option(WELDER_BUILD_Networking "Build the Networking project" OFF)
    option(WELDER_BUILD_Physics "Build the Physics project" OFF)
    option(WELDER_BUILD_Sound "Build the Sound project" OFF)
    option(WELDER_BUILD_Startup "Build the Startup project" OFF)
    option(WELDER_BUILD_ZilchScript "Build the ZilchScript project" OFF)

    # Extensions
    option(WELDER_BUILD_CodeTranslator "Build the CodeTranslator project" OFF)
    option(WELDER_BUILD_Editor "Build the Editor project" OFF)
    option(WELDER_BUILD_Gameplay "Build the Gameplay project" OFF)
    option(WELDER_BUILD_UiWidget "Build the UiWidget project" OFF)
    option(WELDER_BUILD_Widget "Build the Widget project" OFF)
    option(WELDER_BUILD_ZilchShaders "Build the ZilchShaders project" OFF)
endfunction()

###################################
# Option Sets, these default off.
###################################
# Set up option sets, these will then set base options, these will all default
# to off. If you select any option sets, we default to turning all other
# projects off and only turning on what's required for your selected option
# set. 
#
# NOTE: Keep in mind that if you change options without regenerating the build
# folder in it's entirety, you might not see your requested changes until
# after you do.

# If you only want to link against Zilch and the ZilchShader extension this
# option is for you.
option(WELDER_OPTIONSET_ZilchShader "The ZilchShaders option set, will turn on everything required for ZilchShaders" OFF)

if(${WELDER_OPTIONSET_ZilchShader})
    option(WELDER_OPTIONSET_Zilch "Build the Zilch Option Set" ON)
    option(WELDER_BUILD_Geometry "Build the Geometry project" ON)
    option(WELDER_BUILD_ZilchShaders "Build the ZilchShaders project" ON)
endif()

# If you only want to link against Zilch this option is for you.
option(WELDER_OPTIONSET_Zilch "The Zilch option set, will turn on everything required for Zilch" OFF)

if(${WELDER_OPTIONSET_Zilch})
    option(WELDER_BUILD_Common "Build the Common project" ON)
    option(WELDER_BUILD_Zilch "Build the Zilch project" ON)
endif()

# Options are weird, and the first option set in the cmake hierarchy goes first
# so if any of our optionsets are set, then we want to turn off any extra
# builds/dependencies.
if(${WELDER_OPTIONSET_Zilch} OR
   ${WELDER_OPTIONSET_ZilchShader})
   welder_turn_remaining_builds_off()
endif()

###################################
# Base options, these default on.
###################################
#####################
# ZeroLibraries
#####################
option(WELDER_BUILD_Common "Build the Common project" ON)
option(WELDER_BUILD_Dash "Build the Dash project" ON)
option(WELDER_BUILD_Geometry "Build the Geometry project" ON)
option(WELDER_BUILD_Meta "Build the Meta project" ON)
option(WELDER_BUILD_Platform "Build the Platform project" ON)
option(WELDER_BUILD_Serialization "Build the Serialization project" ON)
option(WELDER_BUILD_SpatialPartition "Build the SpatialPartition project" ON)
option(WELDER_BUILD_Support "Build the Support project" ON)
option(WELDER_BUILD_Zilch "Build the Zilch project" ON)

#####################
# External
#####################
option(WELDER_BUILD_Assimp "Build the Assimp project" ON)
option(WELDER_BUILD_CEF "Build the CEF project" ON)
option(WELDER_BUILD_Curl "Build the Curl project" ON)
option(WELDER_BUILD_FreeType "Build the FreeType project" ON)
option(WELDER_BUILD_GL "Build the GL project" ON)
option(WELDER_BUILD_Glew "Build the Glew project" ON)
option(WELDER_BUILD_Libpng "Build the Libpng project" ON)
option(WELDER_BUILD_Nvtt "Build the Nvtt project" ON)
option(WELDER_BUILD_Opus "Build the Opus project" ON)
option(WELDER_BUILD_Scintilla "Build the Scintilla project" ON)
option(WELDER_BUILD_SDL "Build the SDL project" ON)
option(WELDER_BUILD_SpirvCross "Build the SpirvCross project" ON)
option(WELDER_BUILD_SpirvHeaders "Build the SpirvHeaders project" ON)
option(WELDER_BUILD_SpirvTools "Build the SpirvTools project" ON)
option(WELDER_BUILD_ZLib "Build the ZLib project" ON)

#####################
# Systems
#####################
option(WELDER_BUILD_Content "Build the Content project" ON)
option(WELDER_BUILD_Engine "Build the Engine project" ON)
option(WELDER_BUILD_Graphics "Build the Graphics project" ON)
option(WELDER_BUILD_Networking "Build the Networking project" ON)
option(WELDER_BUILD_Physics "Build the Physics project" ON)
option(WELDER_BUILD_Sound "Build the Sound project" ON)
option(WELDER_BUILD_Startup "Build the Startup project" ON)
option(WELDER_BUILD_ZilchScript "Build the ZilchScript project" ON)

#####################
# Extensions
#####################
option(WELDER_BUILD_CodeTranslator "Build the CodeTranslator project" ON)
option(WELDER_BUILD_Editor "Build the Editor project" ON)
option(WELDER_BUILD_Gameplay "Build the Gameplay project" ON)
option(WELDER_BUILD_UiWidget "Build the UiWidget project" ON)
option(WELDER_BUILD_Widget "Build the Widget project" ON)
option(WELDER_BUILD_ZilchShaders "Build the ZilchShaders project" ON)

#####################
# Projects
#####################
option(WELDER_BUILD_ZeroEditor "Build the ZeroEditor project" ON)
option(WELDER_BUILD_ZeroLauncher "Build the ZeroLauncher project" ON)
option(WELDER_BUILD_ZeroLauncherSharedLibrary "Build the ZeroLauncherSharedLibrary project" ON)
option(WELDER_BUILD_BrowserSubProcess "Build the BrowserSubProcess project" ON)










add_custom_target(VirtualFileSystemZip
  #COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${WELDER_VIRTUAL_FILE_SYSTEM_ZIP}" --format=zip
  COMMAND 7z a -tzip -mx=9 -mfb=128 -mpass=10 ${WELDER_VIRTUAL_FILE_SYSTEM_ZIP}
    "Resources"
    "Data"
    ".welder"
  WORKING_DIRECTORY ${WELDER_CORE_DIR}
)

function(welder_setup_library target directory)
  target_include_directories(${target} PUBLIC ${directory} ${WELDER_LIBRARIES_DIR})
endfunction()

function(welder_get_all_dependencies outputList TARGET)
    list(APPEND visitedTargets ${TARGET})
    get_target_property(dependencies ${TARGET} INTERFACE_LINK_LIBRARIES)
    get_target_property(targetType ${TARGET} TYPE)
    if (NOT targetType STREQUAL "INTERFACE_LIBRARY")
      get_target_property(manualDependencies ${TARGET} MANUALLY_ADDED_DEPENDENCIES)
      list(APPEND dependencies ${manualDependencies})
    endif()
    set(allLinkedLibs "")
    foreach(dependency ${dependencies})
        if (TARGET ${dependency})
            list(FIND visitedTargets ${dependency} VISITED)
            if (${VISITED} EQUAL -1)
                welder_get_all_dependencies(childLinkedLibs ${dependency})
                list(APPEND allLinkedLibs ${dependency} ${childLinkedLibs})
            endif()
        endif()
    endforeach()
    set(visitedTargets ${visitedTargets} PARENT_SCOPE)
    set(${outputList} ${allLinkedLibs} PARENT_SCOPE)
endfunction()

function(welder_copy_from_linked_libraries target)
  welder_get_all_dependencies(dependencies ${target})
  foreach(dependency ${dependencies})
    set(directory "${${dependency}CopyDirectories}")
    if(directory)
      add_custom_command(TARGET ${target} POST_BUILD COMMAND
        ${CMAKE_COMMAND} -E copy_directory ${directory} $<TARGET_FILE_DIR:${target}>)
    endif()
  endforeach()
endfunction()

function(welder_target_includes target)
  set(multiValueArgs PUBLIC)
  cmake_parse_arguments(WELDER "" "" "${multiValueArgs}" "${ARGN}")

  foreach(fromTarget ${WELDER_PUBLIC})
  target_include_directories(${target} PUBLIC $<TARGET_PROPERTY:${fromTarget},INTERFACE_INCLUDE_DIRECTORIES>)
  target_compile_definitions(${target} PUBLIC $<TARGET_PROPERTY:${fromTarget},INTERFACE_COMPILE_DEFINITIONS>)
  endforeach()
endfunction()

#Emscripten
#MSVC
include(${WELDER_CMAKE_DIR}/${WELDER_TOOLCHAIN}.cmake)

set(CMAKE_C_FLAGS                             "${CMAKE_C_FLAGS}                             ${WELDER_C_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS                           "${CMAKE_CXX_FLAGS}                           ${WELDER_C_CXX_FLAGS}")

set(CMAKE_C_FLAGS_DEBUG                       "${CMAKE_C_FLAGS_DEBUG}                       ${WELDER_C_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_DEBUG                     "${CMAKE_CXX_FLAGS_DEBUG}                     ${WELDER_C_CXX_FLAGS_DEBUG}")

set(CMAKE_C_FLAGS_RELWITHDEBINFO              "${CMAKE_C_FLAGS_RELWITHDEBINFO}              ${WELDER_C_CXX_FLAGS_RELWITHDEBINFO}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO            "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}            ${WELDER_C_CXX_FLAGS_RELWITHDEBINFO}")

set(CMAKE_C_FLAGS_RELEASE                     "${CMAKE_C_FLAGS_RELEASE}                     ${WELDER_C_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELEASE                   "${CMAKE_CXX_FLAGS_RELEASE}                   ${WELDER_C_CXX_FLAGS_RELEASE}")

set(CMAKE_C_FLAGS_MINSIZEREL                  "${CMAKE_C_FLAGS_MINSIZEREL}                  ${WELDER_C_CXX_FLAGS_MINSIZEREL}")
set(CMAKE_CXX_FLAGS_MINSIZEREL                "${CMAKE_CXX_FLAGS_MINSIZEREL}                ${WELDER_C_CXX_FLAGS_MINSIZEREL}")

set(CMAKE_STATIC_LINKER_FLAGS                 "${CMAKE_STATIC_LINKER_FLAGS}                 ${WELDER_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS                 "${CMAKE_SHARED_LINKER_FLAGS}                 ${WELDER_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS                 "${CMAKE_MODULE_LINKER_FLAGS}                 ${WELDER_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS                    "${CMAKE_EXE_LINKER_FLAGS}                    ${WELDER_LINKER_FLAGS}")

set(CMAKE_STATIC_LINKER_FLAGS_DEBUG           "${CMAKE_STATIC_LINKER_FLAGS_DEBUG}           ${WELDER_LINKER_FLAGS_DEBUG}")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG           "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}           ${WELDER_LINKER_FLAGS_DEBUG}")
set(CMAKE_MODULE_LINKER_FLAGS_DEBUG           "${CMAKE_MODULE_LINKER_FLAGS_DEBUG}           ${WELDER_LINKER_FLAGS_DEBUG}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG              "${CMAKE_EXE_LINKER_FLAGS_DEBUG}              ${WELDER_LINKER_FLAGS_DEBUG}")

set(CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO  "${CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO}  ${WELDER_LINKER_FLAGS_RELWITHDEBINFO}")
set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO  "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO}  ${WELDER_LINKER_FLAGS_RELWITHDEBINFO}")
set(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO  "${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO}  ${WELDER_LINKER_FLAGS_RELWITHDEBINFO}")
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO     "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}     ${WELDER_LINKER_FLAGS_RELWITHDEBINFO}")

set(CMAKE_STATIC_LINKER_FLAGS_RELEASE         "${CMAKE_STATIC_LINKER_FLAGS_RELEASE}         ${WELDER_LINKER_FLAGS_RELEASE}")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE         "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}         ${WELDER_LINKER_FLAGS_RELEASE}")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE         "${CMAKE_MODULE_LINKER_FLAGS_RELEASE}         ${WELDER_LINKER_FLAGS_RELEASE}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE            "${CMAKE_EXE_LINKER_FLAGS_RELEASE}            ${WELDER_LINKER_FLAGS_RELEASE}")

set(CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL      "${CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL}      ${WELDER_LINKER_FLAGS_MINSIZEREL}")
set(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL      "${CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL}      ${WELDER_LINKER_FLAGS_MINSIZEREL}")
set(CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL      "${CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL}      ${WELDER_LINKER_FLAGS_MINSIZEREL}")
set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL         "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL}         ${WELDER_LINKER_FLAGS_MINSIZEREL}")

#Windows
#Emscripten
#SDLSTDEmpty
#Empty
add_subdirectory(Libraries/Platform/${WELDER_PLATFORM})

add_subdirectory(External)
add_subdirectory(Libraries)
