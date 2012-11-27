#!/bin/bash

qmake ../tests.pro && \
make -j2 && \
./tests -callgrind
