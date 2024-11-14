#!/bin/bash

mkdir WindowsBinaries

TOP=$(pwd)  

chmod +x build_linux.sh

./build_linux.sh

cp $(find . -name 'glassman.dll') $TOP/WindowsBinaries

cd ..

cd metallic && chmod +x build_linux.sh

./build_linux.sh

cp $(find . -name 'metal.dll') $TOP/WindowsBinaries


