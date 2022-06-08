#!/bin/bash

SCRIPT_PATH=$(dirname "$0")

cd ${SCRIPT_PATH}

cmake -S . -B build

cmake --build build
