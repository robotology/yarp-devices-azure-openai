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
    yCWarning(TTSDEVICE) << "setVoice not implemented";
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue TtsDevice::getVoice(std::string& voice_name)
{
    yCWarning(TTSDEVICE) << "getVoice not implemented";
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
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
    yCWarning(TTSDEVICE) << "synthesize not implemented";
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}
