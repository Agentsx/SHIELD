#!/bin/bash

export LD_LIBRARY_PATH=./libs:$LD_LIBRARY_PATH

./shield -f ./conf/shield.cfg
