# Install script for directory: /Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/external/JUCE

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/build/external/JUCE/modules/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/build/external/JUCE/extras/Build/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/JUCE-8.0.3" TYPE FILE FILES
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/build/external/JUCE/JUCEConfigVersion.cmake"
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/build/external/JUCE/JUCEConfig.cmake"
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/external/JUCE/extras/Build/CMake/JUCECheckAtomic.cmake"
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/external/JUCE/extras/Build/CMake/JUCEHelperTargets.cmake"
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/external/JUCE/extras/Build/CMake/JUCEModuleSupport.cmake"
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/external/JUCE/extras/Build/CMake/JUCEUtils.cmake"
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/external/JUCE/extras/Build/CMake/JuceLV2Defines.h.in"
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/external/JUCE/extras/Build/CMake/LaunchScreen.storyboard"
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/external/JUCE/extras/Build/CMake/PIPAudioProcessor.cpp.in"
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/external/JUCE/extras/Build/CMake/PIPAudioProcessorWithARA.cpp.in"
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/external/JUCE/extras/Build/CMake/PIPComponent.cpp.in"
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/external/JUCE/extras/Build/CMake/PIPConsole.cpp.in"
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/external/JUCE/extras/Build/CMake/RecentFilesMenuTemplate.nib"
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/external/JUCE/extras/Build/CMake/UnityPluginGUIScript.cs.in"
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/external/JUCE/extras/Build/CMake/checkBundleSigning.cmake"
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/external/JUCE/extras/Build/CMake/copyDir.cmake"
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/external/JUCE/extras/Build/CMake/juce_runtime_arch_detection.cpp"
    "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/external/JUCE/extras/Build/CMake/juce_LinuxSubprocessHelper.cpp"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/build/external/JUCE/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
