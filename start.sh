#!/bin/bash

export LD_LIBRARY_PATH=./libs:$LD_LIBRARY_PATH

mkdir log 2>/dev/null
./shield -f conf/shield.conf &
