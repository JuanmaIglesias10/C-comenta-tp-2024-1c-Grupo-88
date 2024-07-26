#!/bin/bash

# Nombre del programa compilado
PROGRAMA="./bin/entradasalida"

# Función para mostrar el menú
mostrar_menu() {
    echo "Seleccione una opción:"
    echo "PRUEBA PLANI"
    echo "1) ./bin/entradasalida SLP1 plani-SLP1.config"
    echo "PRUEBA DEADLOCK"
    echo "2) ./bin/entradasalida ESPERA dl-ESPERA.config"
    echo "PRUEBA MEMORIA Y TLB"
    echo "3) ./bin/entradasalida IO_GEN_SLEEP tlb-IO_GEN_SLEEP.config"
    echo "PRUEBA IO"
    echo "4) ./bin/entradasalida MONITOR io-MONITOR.config"
    echo "5) ./bin/entradasalida TECLADO io-TECLADO.config"
    echo "6) ./bin/entradasalida GENERICA io-GENERICA.config"
    echo "PRUEBA FS"
    echo "7) ./bin/entradasalida FS fs-FS.config"
    echo "8) ./bin/entradasalida MONITOR fs-MONITOR.config"
    echo "9) ./bin/entradasalida TECLADO fs-TECLADO.config"
    echo "PRUEBA SALVATIONS"
    echo "10) ./bin/entradasalida MONITOR salv-MONITOR.config"
    echo "11) ./bin/entradasalida TECLADO salv-TECLADO.config"
    echo "12) ./bin/entradasalida ESPERA salv-ESPERA.config"
    echo "13) ./bin/entradasalida GENERICA salv-GENERICA.config"
    echo "14) ./bin/entradasalida SLP1 salv-SLP1.config"
    echo "0) SALIR"
}

# Función para ejecutar el programa con el parámetro correspondiente
ejecutar_programa() {
    case $1 in
        1)
            $PROGRAMA SLP1 plani-SLP1.config
            ;;
        2)
            $PROGRAMA ESPERA dl-ESPERA.config
            ;;
        3)
            $PROGRAMA IO_GEN_SLEEP tlb-IO_GEN_SLEEP.config
            ;;
        4)
            $PROGRAMA MONITOR io-MONITOR.config
            ;;
        5)
            $PROGRAMA TECLADO io-TECLADO.config
            ;;
        6)
            $PROGRAMA GENERICA io-GENERICA.config
            ;;
        7)
            $PROGRAMA FS fs-FS.config
            ;;
        8)
            $PROGRAMA MONITOR fs-MONITOR.config
            ;;
        9)
            $PROGRAMA TECLADO fs-TECLADO.config
            ;;
        10)
            $PROGRAMA MONITOR salv-MONITOR.config
            ;;
        11)
            $PROGRAMA TECLADO salv-TECLADO.config
            ;;
        12)
            $PROGRAMA ESPERA salv-ESPERA.config
            ;;
        13)
            $PROGRAMA GENERICA salv-GENERICA.config
            ;;
        14)
            $PROGRAMA SLP1 salv-SLP1.config
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