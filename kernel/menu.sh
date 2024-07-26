#!/bin/bash

# Nombre del programa compilado
PROGRAMA="./bin/kernel"

# Función para mostrar el menú
mostrar_menu() {
    echo "Seleccione una opción:"
    echo "1) ./bin/kernel plani-kernel.config"
    echo "2) ./bin/kernel dl-kernel.config"
    echo "3) ./bin/kernel tlb-kernel.config"
    echo "4) ./bin/kernel io-kernel.config"
    echo "5) ./bin/kernel fs-kernel.config"
    echo "6) ./bin/kernel salv-kernel.config"
    echo "0) SALIR"
}

# Función para ejecutar el programa con el parámetro correspondiente
ejecutar_programa() {
    case $1 in
        1)
            $PROGRAMA plani-kernel-config
            ;;
        2)
            $PROGRAMA dl-kernel.config
            ;;
        3)
            $PROGRAMA tlb-kernel.config
            ;;
        4)
            $PROGRAMA io-kernel.config
            ;;
        5)
            $PROGRAMA fs-kernel.config
            ;;
        6)
            $PROGRAMA salv-kernel.config
            ;;
        0)
            echo "Saliendo..."
            exit 0
            ;;
        *)
            echo "Opción no válida"
            ;;
    esac
}

# Bucle principal
while true; do
    mostrar_menu
    read -p "Ingrese su opción: " opcion
    ejecutar_programa $opcion
    echo
done