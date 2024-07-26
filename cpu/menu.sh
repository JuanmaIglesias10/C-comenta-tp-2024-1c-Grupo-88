#!/bin/bash

# Nombre del programa compilado
PROGRAMA="./bin/cpu"

# Función para mostrar el menú
mostrar_menu() {
    echo "Seleccione una opción:"
    echo "1) ./bin/cpu plani-cpu.config"
    echo "2) ./bin/cpu dl-cpu.config"
    echo "3) ./bin/cpu tlb-cpu.config"
    echo "4) ./bin/cpu io-cpu.config"
    echo "5) ./bin/cpu fs-cpu.config"
    echo "6) ./bin/cpu salv-cpu.config"
    echo "0) SALIR"
}

# Función para ejecutar el programa con el parámetro correspondiente
ejecutar_programa() {
    case $1 in
        1)
            $PROGRAMA plani-cpu-config
            ;;
        2)
            $PROGRAMA dl-cpu.config
            ;;
        3)
            $PROGRAMA tlb-cpu.config
            ;;
        4)
            $PROGRAMA io-cpu.config
            ;;
        5)
            $PROGRAMA fs-cpu.config
            ;;
        6)
            $PROGRAMA salv-cpu.config
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