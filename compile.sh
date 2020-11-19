#!/bin/bash

mkdir -p build && cd build && cmake -DLexerTest=ON -DCMAKE_BUILD_TYPE=DEBUG .. && make -j9
