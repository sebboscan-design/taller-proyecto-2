#!/bin/bash
# Corre los 8 casos de uso desde cero y guarda las salidas en pruebas/
# Uso (desde la carpeta del proyecto, ya compilado ./taller): bash pruebas/correr_pruebas.sh

rm -rf datos backup_*

for caso in 1a 1b 2 3a 3b 3c 4a 4b 5a 5b 6 7a; do
    ./taller < pruebas/entrada_$caso.txt > pruebas/salida_$caso.txt
    echo "Caso $caso listo"
done

# caso 7: se reemplaza vehiculos.bin por la copia del backup
cp backup_*/vehiculos.bin datos/vehiculos.bin
echo "datos/vehiculos.bin reemplazado con la copia del backup"

for caso in 7b 8; do
    ./taller < pruebas/entrada_$caso.txt > pruebas/salida_$caso.txt
    echo "Caso $caso listo"
done
echo "Listo. La carpeta datos quedo con los datos pre-poblados."
