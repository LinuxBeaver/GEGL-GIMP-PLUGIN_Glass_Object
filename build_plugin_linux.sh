#!/bin/bash

mkdir LinuxBinaries

TOP=$(pwd)  

chmod +x build_linux.sh

./build_linux.sh

cp $(find . -name 'glassman.so') $TOP/LinuxBinaries

cd ..

cd metallic && chmod +x build_linux.sh

./build_linux.sh

cp $(find . -name 'metal.so') $TOP/LinuxBinaries


