![YARP logo](doc/images/yarp-robot-24.png?raw=true "yarp-devices-azure-openai")
Yarp devices for speech transcription and synthesis using Azure OpenAI APIs
=====================

This repository contains the YARP plugins for speech transcription and synthesis using Azure OpenAI APIs

:construction: This repository is currently work in progress. :construction:
:construction: The software contained is this repository is currently under testing. :construction: APIs may change without any warning. :construction: This code should be not used before its first official release :construction:

Documentation
-------------

Documentation of the individual devices is provided in the official Yarp documentation page:
[![YARP documentation](https://img.shields.io/badge/Documentation-yarp.it-19c2d8.svg)](https://yarp.it/latest/group__dev__impl.html)


Installation
-------------

### Dependencies
Apart from `YARP` the repo depends only from `curl`

On `Ubuntu`
~~~
sudo apt install curl
~~~

### Build with pure CMake commands

~~~
# Configure, compile and install
cmake -S. -Bbuild -DCMAKE_INSTALL_PREFIX=<install_prefix>
cmake --build build
cmake --build build --target install
~~~

CI Status
---------

:construction: This repository is currently work in progress. :construction:

[![Build Status](https://github.com/robotology/yarp-devices-azure-openai/workflows/CI%20Workflow/badge.svg)](https://github.com/robotology/yarp-devices-azure-openai/actions?query=workflow%3A%22CI+Workflow%22)

License
---------

:construction: This repository is currently work in progress. :construction:

Maintainers
--------------
This repository is maintained by:

| | |
|:---:|:---:|
| [<img src="https://github.com/elandini84.png" width="40">](https://github.com/elandini84) | [@elandini84](https://github.com/elandini84) |
