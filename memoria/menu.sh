#!/bin/bash

# Nombre del programa compilado
PROGRAMA="./bin/memoria"

# Función para mostrar el menú
mostrar_menu() {
    echo "Seleccione una opción:"
    echo "1) ./bin/memoria plani-memoria.config"
    echo "2) ./bin/memoria dl-memoria.config"
    echo "3) ./bin/memoria tlb-memoria.config"
    echo "4) ./bin/memoria io-memoria.config"
    echo "5) ./bin/memoria fs-memoria.config"
    echo "6) ./bin/memoria salv-memoria.config"
    echo "0) SALIR"
}

# Función para ejecutar el programa con el parámetro correspondiente
ejecutar_programa() {
    case $1 in
        1)
            $PROGRAMA plani-memoria-config
            ;;
        2)
            $PROGRAMA dl-memoria.config
            ;;
        3)
            $PROGRAMA tlb-memoria.config
            ;;
        4)
            $PROGRAMA io-memoria.config
            ;;
        5)
            $PROGRAMA fs-memoria.config
            ;;
        6)
            $PROGRAMA salv-memoria.config
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