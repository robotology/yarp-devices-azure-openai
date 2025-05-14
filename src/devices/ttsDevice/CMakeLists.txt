# SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(ttsDevice
  CATEGORY device
  TYPE TtsDevice
  INCLUDE TtsDevice.h
  INTERNAL ON
)

if(NOT SKIP_ttsDevice)
  find_package(CURL REQUIRED)
  yarp_add_plugin(yarp_ttsDevice)
  generateDeviceParamsParser(TtsDevice ttsDevice)

  target_sources(yarp_ttsDevice
    PRIVATE
      TtsDevice.cpp
      TtsDevice.h
      TtsDevice_ParamsParser.cpp
      TtsDevice_ParamsParser.h
      dr_mp3.h
  )

  target_link_libraries(yarp_ttsDevice
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      CURL::libcurl
  )

  yarp_install(
    TARGETS yarp_ttsDevice
    EXPORT yarp-device-ttsDevice
    COMPONENT yarp-device-ttsDevice
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

  set_property(TARGET yarp_ttsDevice PROPERTY FOLDER "Plugins/Device")
endif()
