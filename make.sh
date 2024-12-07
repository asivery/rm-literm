#!/bin/bash
python3 $XOVI_HOME/util/xovigen.py -o xoviout literm.xovi
qmake6 .
make -j`nproc`

