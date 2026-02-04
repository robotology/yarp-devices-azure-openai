# SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(gptTranscribeDevice
  CATEGORY device
  TYPE GptTranscribeDevice
  INCLUDE GptTranscribeDevice.h
  INTERNAL ON
)

if(NOT SKIP_gptTranscribeDevice)
  find_package(CURL REQUIRED)
  yarp_add_plugin(yarp_gptTranscribeDevice)
  generateDeviceParamsParser(GptTranscribeDevice gptTranscribeDevice)

  target_sources(yarp_gptTranscribeDevice
    PRIVATE
      GptTranscribeDevice.cpp
      GptTranscribeDevice.h
      GptTranscribeDevice_ParamsParser.cpp
      GptTranscribeDevice_ParamsParser.h
  )

  include(FetchContent)

  FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz)
  FetchContent_MakeAvailable(json)

  target_link_libraries(yarp_gptTranscribeDevice
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      CURL::libcurl
      nlohmann_json::nlohmann_json
  )

  yarp_install(
    TARGETS yarp_gptTranscribeDevice
    EXPORT yarp-device-gptTranscribeDevice
    COMPONENT yarp-device-gptTranscribeDevice
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

  set_property(TARGET yarp_whisperDevice PROPERTY FOLDER "Plugins/Device")
endif()
