/*
 * SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_TTSDEVICE_H
#define YARP_TTSDEVICE_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ISpeechSynthesizer.h>
#include <curl/curl.h>
#include <yarp/os/all.h>
#include <yarp/sig/Sound.h>
#include <iomanip> // for std::setw, std::hex, std::setfill

#include "TtsDevice_ParamsParser.h"

/**
 *  @ingroup dev_impl_other
 *
 * \section ttsDevice
 *
 * \brief `ttsDevice`: A yarp device for speech synthesis using azure openai APIs
 *
 *  Parameters required by this device are described in class TtsDevice_ParamsParser
 *
 */

class TtsDevice :
        public yarp::dev::DeviceDriver,
        public yarp::dev::ISpeechSynthesizer,
        public TtsDevice_ParamsParser
{
public:
    TtsDevice();
    TtsDevice(const TtsDevice&) = delete;
    TtsDevice(TtsDevice&&) noexcept = delete;
    TtsDevice& operator=(const TtsDevice&) = delete;
    TtsDevice& operator=(TtsDevice&&) noexcept = delete;
    ~TtsDevice() override = default;

    // DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // ISpeechSynthesizer
    yarp::dev::ReturnValue setLanguage(const std::string& language="auto") override;
    yarp::dev::ReturnValue getLanguage(std::string& language) override;
    yarp::dev::ReturnValue setVoice(const std::string& voice_name = "auto") override;
    yarp::dev::ReturnValue getVoice(std::string& voice_name) override;
    yarp::dev::ReturnValue setSpeed(const double speed=0) override;
    yarp::dev::ReturnValue getSpeed(double& speed) override;
    yarp::dev::ReturnValue setPitch(const double pitch) override;
    yarp::dev::ReturnValue getPitch(double& pitch) override;
    yarp::dev::ReturnValue synthesize(const std::string& text, yarp::sig::Sound& sound) override;

private:
    std::string m_voiceName;
    std::string m_url;
    std::string m_apiKey;
    struct curl_slist *headers{nullptr};
};

#endif // YARP_TTSDEVICE_H
