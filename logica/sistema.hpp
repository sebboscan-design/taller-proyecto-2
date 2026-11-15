#ifndef SISTEMA_HPP
#define SISTEMA_HPP

// sistema.hpp
// Lo que hace el programa al arrancar, y los datos del taller.

#include "../estructuras.hpp"
#include "resultados.hpp"

// prepararArchivos: crea la carpeta datos/ y los .bin que falten, cada uno
// con su header inicial. Los que ya existen no se tocan.
// Devuelve OK o ERROR_ARCHIVO.
Resultado prepararArchivos();

// tallerRegistrado: true si ya existe taller.bin (o sea, no es el primer arranque).
bool tallerRegistrado();

// registrarTaller: crea taller.bin con los datos del taller.
// Devuelve ERROR_DATO_INVALIDO si el nombre o el RIF estan vacios.
Resultado registrarTaller(Taller datos);

// obtenerTaller: lee taller.bin y deja una copia en destino.
Resultado obtenerTaller(Taller* destino);

// archivoConError: ruta del ultimo archivo que no se pudo abrir.
// La usa el menu para decir cual archivo fallo cuando recibe ERROR_ARCHIVO.
const char* archivoConError();

#endif
