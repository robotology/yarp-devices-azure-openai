/*
 * SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_WHISPERDEVICE_H
#define YARP_WHISPERDEVICE_H

#include <curl/curl.h>
#include <iomanip> // for std::setw, std::hex, std::setfill
#include <iostream>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <iterator>
#include <cstring>

#include <nlohmann/json.hpp>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ISpeechTranscription.h>
#include <yarp/os/all.h>
#include <yarp/sig/Sound.h>

#include "WhisperDevice_ParamsParser.h"

/**
 *  @ingroup dev_impl_other
 *
 * \section whisperDevice
 *
 * \brief `whisperDevice`: A yarp device for speech synthesis using azure openai APIs
 *
 *  Parameters required by this device are described in class WhisperDevice_ParamsParser
 *
 */

class WhisperDevice :
        public yarp::dev::DeviceDriver,
        public yarp::dev::ISpeechTranscription,
        public WhisperDevice_ParamsParser
{
public:
    WhisperDevice();
    WhisperDevice(const WhisperDevice&) = delete;
    WhisperDevice(WhisperDevice&&) noexcept = delete;
    WhisperDevice& operator=(const WhisperDevice&) = delete;
    WhisperDevice& operator=(WhisperDevice&&) noexcept = delete;
    ~WhisperDevice() override = default;

    // DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // yarp::dev::ISpeechTranscription
    yarp::dev::ReturnValue setLanguage(const std::string& language="auto") override;
    yarp::dev::ReturnValue getLanguage(std::string& language) override;
    yarp::dev::ReturnValue transcribe(const yarp::sig::Sound& sound, std::string& transcription, double& score) override;

private:
    std::string m_url;
    std::string m_apiKey;
    struct curl_slist *headers{nullptr};

    std::vector<uint8_t> _createWavHeader(int sampleRate, int numSamples);
    static size_t _writeCallback(void *contents, size_t size, size_t nmemb, std::string *output);
};

#endif // YARP_WHISPERDEVICE_H
