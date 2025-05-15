/*
 * SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include "TtsDevice.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

// Include dr_mp3 for decoding MP3
#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

#include <cmath>

using namespace yarp::os;
using namespace yarp::dev;


YARP_LOG_COMPONENT(TTSDEVICE, "yarp.ttsDevice", yarp::os::Log::TraceType);


TtsDevice::TtsDevice()
{

}

bool TtsDevice::open(yarp::os::Searchable &config)
{
    if (!parseParams(config))  { return false; }
    if(std::getenv(m_ENVS_api_key_name.c_str()) == nullptr)
    {
        yCError(TTSDEVICE) << "Environment variable" << m_ENVS_api_key_name << "not set";
        return false;
    }
    if(std::getenv(m_ENVS_end_point_name.c_str()) == nullptr)
    {
        yCError(TTSDEVICE) << "Environment variable" << m_ENVS_end_point_name << "not set";
        return false;
    }
    if(std::getenv(m_ENVS_deployment_id_name.c_str()) == nullptr)
    {
        yCError(TTSDEVICE) << "Environment variable" << m_ENVS_deployment_id_name << "not set";
        return false;
    }
    if(std::getenv(m_ENVS_api_version_name.c_str()) == nullptr)
    {
        yCError(TTSDEVICE) << "Environment variable" << m_ENVS_api_version_name << "not set";
        return false;
    }
    m_apiKey = std::getenv(m_ENVS_api_key_name.c_str());
    std::string endpoint = std::getenv(m_ENVS_end_point_name.c_str());
    std::string deployment_id = std::getenv(m_ENVS_deployment_id_name.c_str());
    std::string api_version = std::getenv(m_ENVS_api_version_name.c_str());
    m_url = endpoint + "/openai/deployments/" + deployment_id + "/audio/speech?api-version=" + api_version;

    yCInfo(TTSDEVICE) << "Open";
    return true;
}

bool TtsDevice::close()
{
    yCInfo(TTSDEVICE) << "Close";
    return true;
}

ReturnValue TtsDevice::setLanguage(const std::string& language)
{
    yCWarning(TTSDEVICE) << "setLanguage not implemented";
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue TtsDevice::getLanguage(std::string& language)
{
    yCWarning(TTSDEVICE) << "getLanguage not implemented";
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue TtsDevice::setVoice(const std::string& voice_name)
{
    if(voice_name.empty())
    {
        yCError(TTSDEVICE) << "setVoice not implemented";
        return ReturnValue::return_code::return_value_error_generic;
    }
    if (!_voiceNameIsValid(voice_name)) {
        yCError(TTSDEVICE) << "Invalid voice name" << voice_name;
        return ReturnValue::return_code::return_value_error_generic;
    }
    m_voiceName = voice_name;

    return ReturnValue_ok;
}

ReturnValue TtsDevice::getVoice(std::string& voice_name)
{
    voice_name = m_voiceName;
    return ReturnValue_ok;
}

ReturnValue TtsDevice::setSpeed(const double speed)
{
    yCWarning(TTSDEVICE) << "setSpeed not implemented";
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue TtsDevice::getSpeed(double& speed)
{
    yCWarning(TTSDEVICE) << "getSpeed not implemented";
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue TtsDevice::setPitch(const double pitch)
{
    yCWarning(TTSDEVICE) << "setPitch not implemented";
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue TtsDevice::getPitch(double& pitch)
{
    yCWarning(TTSDEVICE) << "getPitch not implemented";
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue TtsDevice::synthesize(const std::string& text, yarp::sig::Sound& sound)
{
    CURL *curl = curl_easy_init();
    if (!curl) {
        yCError(TTSDEVICE) << "Failed to initialize cURL";
        return ReturnValue::return_code::return_value_error_generic;
    }

    std::string payload = "{\"model\": \"tts-1\", \"input\": \"" + _escapeJsonString(text) + "\", \"voice\": \""+ m_voiceName + "\"}";

    std::vector<uint8_t> audioData;

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, ("api-key: " + m_apiKey).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &audioData);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        yCError(TTSDEVICE) << "cURL request failed: " << curl_easy_strerror(res);
        return ReturnValue::return_code::return_value_error_generic;
    }

    yCInfo(TTSDEVICE) << "Downloaded MP3 data: " << audioData.size() << " bytes";

    // Decode MP3 using dr_mp3
    drmp3 mp3;
    if (!drmp3_init_memory(&mp3, audioData.data(), audioData.size(), NULL)) {
        yCError(TTSDEVICE) << "Failed to decode MP3";
        return ReturnValue::return_code::return_value_error_generic;
    }

    drmp3_uint64 totalFrames = drmp3_get_pcm_frame_count(&mp3);
    std::vector<int16_t> pcmData(totalFrames * mp3.channels);

    drmp3_uint64 samplesRead = drmp3_read_pcm_frames_s16(&mp3, totalFrames, pcmData.data());
    drmp3_uninit(&mp3);

    pcmData.resize(samplesRead * mp3.channels);

    yCInfo(TTSDEVICE) << "Decoded " << samplesRead << " frames, channels: " << mp3.channels;

    sound.clear();
    sound.resize(samplesRead, mp3.channels);
    sound.setFrequency(mp3.sampleRate);

    for (size_t i = 0; i < samplesRead; ++i) {
        for (uint32_t ch = 0; ch < mp3.channels; ++ch) {
            sound.set(pcmData[i * mp3.channels + ch], i, ch);
        }
    }

    return ReturnValue_ok;
}

size_t TtsDevice::_writeCallback(void *contents, size_t size, size_t nmemb, std::vector<uint8_t> *output) {
    size_t totalSize = size * nmemb;
    output->insert(output->end(), (uint8_t *)contents, (uint8_t *)contents + totalSize);
    return totalSize;
}

std::string TtsDevice::_escapeJsonString(const std::string &input) {
    std::ostringstream ss;
    for (const auto &c : input) {
        switch (c) {
            case '\"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\b': ss << "\\b";  break;
            case '\f': ss << "\\f";  break;
            case '\n': ss << "\\n";  break;
            case '\r': ss << "\\r";  break;
            case '\t': ss << "\\t";  break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                } else {
                    ss << c;
                }
        }
    }
    return ss.str();
}

bool TtsDevice::_voiceNameIsValid(const std::string& voice_name)
{
    if (std::find(VOICES.begin(), VOICES.end(), voice_name) != VOICES.end()) {
        return true;
    } else {
        yCError(TTSDEVICE) << "Invalid voice name" << voice_name;
        return false;
    }
}
