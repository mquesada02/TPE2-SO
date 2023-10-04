#!/bin/bash

echo ""
echo "Instalando GCC"
echo ""
sudo apt install gcc

echo ""
echo "Instalando Clang-format"
echo ""
sudo apt install clang-format

echo ""
echo "Instalando Cppcheck"
echo ""
sudo apt install cppcheck

echo ""
echo "Instalando PVStudio"
echo ""
sudo apt install pvs-studio

echo "Agregando repositorio de git"
sudo add-apt-repository ppa:git-core/ppa
echo "Corriendo apt update"
sudo apt update
echo "Instalando git"
sudo apt install git

echo ""
echo "Instalando los hooks"
echo ""
cp .hooks/* .git/hooks/

git config core.hooksPath .hooks

echo "Done!"
