#!/bin/bash
if [ -z "${XOVI_HOME}" ]; then
  echo "Environment variable XOVI_HOME is empty or not set."
  exit 1
fi

if [ ! -e "${HOME}/Tools/remarkable-toolchain/environment-setup-cortexa53-crypto-remarkable-linux" ]; then
  echo "The remarkable toolchain should be accessible via '~/Tools/remarkable-toolchain'."
fi

source ~/Tools/remarkable-toolchain/environment-setup-cortexa53-crypto-remarkable-linux
python3 $XOVI_HOME/util/xovigen.py -o xoviout literm.xovi
qmake6 .
make -j`nproc`
