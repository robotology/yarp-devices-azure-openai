# SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(whisperDevice
  CATEGORY device
  TYPE WhisperDevice
  INCLUDE WhisperDevice.h
  INTERNAL ON
)

if(NOT SKIP_whisperDevice)
  find_package(CURL REQUIRED)
  yarp_add_plugin(yarp_whisperDevice)
  generateDeviceParamsParser(WhisperDevice whisperDevice)

  target_sources(yarp_whisperDevice
    PRIVATE
      WhisperDevice.cpp
      WhisperDevice.h
      WhisperDevice_ParamsParser.cpp
      WhisperDevice_ParamsParser.h
  )

  include(FetchContent)

  FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz)
  FetchContent_MakeAvailable(json)

  target_link_libraries(yarp_whisperDevice
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      CURL::libcurl
      nlohmann_json::nlohmann_json
  )

  yarp_install(
    TARGETS yarp_whisperDevice
    EXPORT yarp-device-whisperDevice
    COMPONENT yarp-device-whisperDevice
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

  set_property(TARGET yarp_whisperDevice PROPERTY FOLDER "Plugins/Device")
endif()
