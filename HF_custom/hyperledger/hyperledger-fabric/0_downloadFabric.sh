#!/bin/bash
:<<'END'
/*
*
*  *********************************************************************************************************
*   Copyright 2020 Korea University 
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*   *********************************************************************************************************
*   Developed by Kwanhoon Lee, Gyuwon Song, Stella team, Operating Systems Lab of Korea University
*   *********************************************************************************************************
*
*/
END
# Exit on first error, print all commands.
set -ev

# Set ARCH
ARCH=`uname -m`

# Grab the current directory.
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Fabric tools
curl -sSL http://bit.ly/2ysbOFE | bash -s 1.1.0

# Pull and tag the latest Hyperledger Fabric base image.
# docker pull hyperledger/fabric-peer:$ARCH-1.1.0
# docker pull hyperledger/fabric-ca:$ARCH-1.1.0
# docker pull hyperledger/fabric-ccenv:$ARCH-1.1.0
# docker pull hyperledger/fabric-orderer:$ARCH-1.1.0
# docker pull hyperledger/fabric-couchdb:$ARCH-1.1.0
