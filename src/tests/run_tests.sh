#!/bin/bash

qmake ../PlainText.pro CONFIG+=tests && \
make -j2 && \
./tests
