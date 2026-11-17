#ifndef MANTENIMIENTO_HPP
#define MANTENIMIENTO_HPP

// mantenimiento.hpp
// Verificacion de integridad y backup de los archivos.

#include "resultados.hpp"

const int MAX_PROBLEMAS = 50;   // problemas que se guardan con su mensaje
const int LARGO_PROBLEMA = 160; // largo maximo de cada mensaje

// ReporteIntegridad
// Lo que encontro verificarIntegridad. La logica arma los mensajes ("ROTO: ...")
// y la presentacion solo los imprime.
struct ReporteIntegridad {
    int vehiculosVerificados;
    int mecanicosVerificados;
    int repuestosVerificados;
    int ordenesVerificadas;
    int problemas;                                // total encontrado (puede pasar de MAX_PROBLEMAS)
    char mensajes[MAX_PROBLEMAS][LARGO_PROBLEMA]; // los primeros MAX_PROBLEMAS
};

// verificarIntegridad: recorre todos los .bin, un registro a la vez y sin
// confiar en el header, y reporta:
//  - ordenes que apuntan a un vehiculo o mecanico que no existe
//  - ordenes ABIERTAS con vehiculo eliminado o mecanico de baja
//  - lineas que apuntan a un repuesto que no existe
//  - repuestos con stock negativo
//  - headers cuyo conteo no coincide con lo que tiene el archivo
// Devuelve: OK (aunque haya problemas: estan en reporte) o ERROR_ARCHIVO.
Resultado verificarIntegridad(ReporteIntegridad* reporte);

// crearBackup: crea la carpeta backup_AAAA-MM-DD_HH-MM y copia byte a byte
// todos los .bin de datos/ (incluido taller.bin).
// Deja en rutaCreada el nombre de la carpeta (rutaCreada debe tener tamanoRuta caracteres).
// Devuelve: OK o ERROR_ARCHIVO.
Resultado crearBackup(char* rutaCreada, int tamanoRuta);

#endif
