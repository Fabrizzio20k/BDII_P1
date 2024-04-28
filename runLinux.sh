#!/bin/bash

# Lista de paquetes a verificar e instalar si es necesario
packages=("bison" "flex" "gcc" "make")

# Verificar e instalar los paquetes
for package in "${packages[@]}"; do
    if ! command -v "$package" &> /dev/null; then
        echo "El paquete $package no está instalado, instalándolo..."
        sudo apt-get update
        sudo apt-get install -y "$package"
    else
        echo "El paquete $package ya está instalado."
    fi
done

echo "Verificación e instalación completadas."
echo "Compilando la librería..."
make
echo "Compilación completada."
echo "Compilando el proyecto con CMake..."
mkdir -p build
cd build
cmake ..
make
echo "Compilación completada."
echo "Ejecutando el programa..."
./sql_parser
echo "Ejecución completada."
