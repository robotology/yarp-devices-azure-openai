/*
 * SPDX-FileCopyrightText: 2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include "WhisperDevice.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>


#include <cmath>

using namespace yarp::os;
using namespace yarp::dev;


YARP_LOG_COMPONENT(WHISPERDEVICE, "yarp.whisperDevice", yarp::os::Log::TraceType);


WhisperDevice::WhisperDevice()
{

}

bool WhisperDevice::open(yarp::os::Searchable &config)
{
    if (!parseParams(config))  { return false; }

    if(std::getenv(m_ENVS_api_key_name.c_str()) == nullptr)
    {
        yCError(WHISPERDEVICE) << "Environment variable" << m_ENVS_api_key_name << "not set";
        return false;
    }
    if(std::getenv(m_ENVS_end_point_name.c_str()) == nullptr)
    {
        yCError(WHISPERDEVICE) << "Environment variable" << m_ENVS_end_point_name << "not set";
        return false;
    }
    if(std::getenv(m_ENVS_deployment_id_name.c_str()) == nullptr)
    {
        yCError(WHISPERDEVICE) << "Environment variable" << m_ENVS_deployment_id_name << "not set";
        return false;
    }
    if(std::getenv(m_ENVS_api_version_name.c_str()) == nullptr)
    {
        yCError(WHISPERDEVICE) << "Environment variable" << m_ENVS_api_version_name << "not set";
        return false;
    }
    m_apiKey = std::getenv(m_ENVS_api_key_name.c_str());
    std::string endpoint = std::getenv(m_ENVS_end_point_name.c_str());
    std::string deployment_id = std::getenv(m_ENVS_deployment_id_name.c_str());
    std::string api_version = std::getenv(m_ENVS_api_version_name.c_str());
    m_url = endpoint + "/openai/deployments/" + deployment_id + "/audio/transcriptions?api-version=" + api_version;

    yCInfo(WHISPERDEVICE) << "Open";
    return true;
}

bool WhisperDevice::close()
{
    yCInfo(WHISPERDEVICE) << "Close";
    return true;
}

ReturnValue WhisperDevice::setLanguage(const std::string& language)
{
    yCWarning(WHISPERDEVICE) << "setLanguage not implemented";
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue WhisperDevice::getLanguage(std::string& language)
{
    yCWarning(WHISPERDEVICE) << "getLanguage not implemented";
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue WhisperDevice::transcribe(const yarp::sig::Sound& sound, std::string& transcription, double& score)
{
    score = 0.0;
    CURL *curl = curl_easy_init();
    if (!curl) {
        yCError(WHISPERDEVICE) << "Failed to initialize cURL";
        return yarp::dev::ReturnValue::return_code::return_value_error_generic;
    }
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, ("api-key: " + m_apiKey).c_str());
    headers = curl_slist_append(headers, "Content-Type: multipart/form-data");

    int sampleRate = sound.getFrequency();
    std::vector<uint8_t> wavHeader = _createWavHeader(sampleRate, sound.getSamples());
    std::vector<uint8_t> audioData(wavHeader.begin(), wavHeader.end());

    for (size_t i = 0; i < sound.getSamples(); ++i) {
        int16_t sample = static_cast<int16_t>(sound.get(i));
        audioData.push_back(sample & 0xFF);
        audioData.push_back((sample >> 8) & 0xFF);
    }

    struct curl_httppost *post = NULL;
    struct curl_httppost *last = NULL;
    curl_formadd(&post, &last,
                 CURLFORM_COPYNAME, "file",
                 CURLFORM_BUFFER, "audio.wav",
                 CURLFORM_BUFFERPTR, audioData.data(),
                 CURLFORM_BUFFERLENGTH, audioData.size(),
                 CURLFORM_CONTENTTYPE, "audio/wav",
                 CURLFORM_END);
    curl_formadd(&post, &last,
                 CURLFORM_COPYNAME, "response_format",
                 CURLFORM_COPYCONTENTS, "verbose_json",
                 CURLFORM_END);

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        yCError(WHISPERDEVICE) << "cURL request failed: " << curl_easy_strerror(res);
    } else {
        yCDebug(WHISPERDEVICE) << "Transcription response: " << response;
    }

    // Parse the JSON response
    try {
        auto jsonResponse = nlohmann::json::parse(response);
        if (jsonResponse.contains("text")) {
            transcription = jsonResponse["text"].get<std::string>();
        } else {
            yCError(WHISPERDEVICE) << "No 'text' field in the response";
            return ReturnValue::return_code::return_value_error_generic;
        }
        if (jsonResponse.contains("segments") && !jsonResponse["segments"].empty()) {
            if (jsonResponse["segments"][0].contains("avg_logprob") && jsonResponse["segments"][0].contains("no_speech_prob")) {
                score = std::exp(jsonResponse["segments"][0]["avg_logprob"].get<double>()) * (1.0 - jsonResponse["segments"][0]["no_speech_prob"].get<double>());
            } else {
                yCWarning(WHISPERDEVICE) << "No 'avg_logprob' or 'no_speech_prob' field in the first segment, setting score to 0.0";
                score = 1.0;
            }
        } else if (jsonResponse.contains("confidence")) {
            score = jsonResponse["confidence"].get<double>();

        } else {
            yCWarning(WHISPERDEVICE) << "No 'confidence' field in the response, setting score to 0.0";
            score = 0.0;
        }
    } catch (const nlohmann::json::parse_error& e) {
        yCError(WHISPERDEVICE) << "JSON parse error: " << e.what();
        return ReturnValue::return_code::return_value_error_generic;
    }

    curl_easy_cleanup(curl);
    curl_formfree(post);
    curl_slist_free_all(headers);

    return ReturnValue::return_code::return_value_ok;
}

std::vector<uint8_t> WhisperDevice::_createWavHeader(int sampleRate, int numSamples)
{
    int byteRate = sampleRate * 2; // 16-bit mono
    int blockAlign = 2;
    int subChunk2Size = numSamples * blockAlign;
    int chunkSize = 36 + subChunk2Size;

    std::vector<uint8_t> header(44);
    std::memcpy(header.data(), "RIFF", 4);
    std::memcpy(header.data() + 4, &chunkSize, 4);
    std::memcpy(header.data() + 8, "WAVE", 4);
    std::memcpy(header.data() + 12, "fmt ", 4);
    int subChunk1Size = 16;
    short audioFormat = 1;
    short numChannels = 1;
    std::memcpy(header.data() + 16, &subChunk1Size, 4);
    std::memcpy(header.data() + 20, &audioFormat, 2);
    std::memcpy(header.data() + 22, &numChannels, 2);
    std::memcpy(header.data() + 24, &sampleRate, 4);
    std::memcpy(header.data() + 28, &byteRate, 4);
    std::memcpy(header.data() + 32, &blockAlign, 2);
    short bitsPerSample = 16;
    std::memcpy(header.data() + 34, &bitsPerSample, 2);
    std::memcpy(header.data() + 36, "data", 4);
    std::memcpy(header.data() + 40, &subChunk2Size, 4);

    return header;
}

size_t WhisperDevice::_writeCallback(void *contents, size_t size, size_t nmemb, std::string *output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}