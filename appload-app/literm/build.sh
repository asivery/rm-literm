#!/bin/bash
rm -rf literm
mkdir literm
cp icon.png manifest.json literm
rcc --binary -o literm/resources.rcc application.qrc
