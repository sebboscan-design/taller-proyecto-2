#include "comunes.hpp"
#include "../logica/sistema.hpp"
#include "../logica/vehiculos.hpp"
#include "../logica/mecanicos.hpp"
#include "../logica/repuestos.hpp"
#include <iostream>
#include <cstdio>

using namespace std;

void mostrarErrorArchivo() {
    cout << "ERROR: No se pudo acceder al archivo '" << archivoConError()
         << "'. La operacion no se realizo.\n";
}

void avisarSiHayMas(bool hayMas, int mostrados) {
    if (hayMas) {
        cout << "(Hay mas resultados. Solo se muestran los primeros " << mostrados
             << "; use una busqueda mas especifica.)\n";
    }
}

void describirVehiculo(int id, char* destino, int tamano) {
    Vehiculo v;
    Resultado lectura = obtenerVehiculoPorID(id, &v);
    if (lectura == OK || lectura == ERROR_ELIMINADO) {
        snprintf(destino, tamano, "%s - %s %s", v.placa, v.marca, v.modelo);
    } else {
        snprintf(destino, tamano, "(vehiculo ID %d no encontrado)", id);
    }
}

void describirMecanico(int id, char* destino, int tamano) {
    Mecanico m;
    Resultado lectura = obtenerMecanicoPorID(id, &m);
    if (lectura == OK) {
        snprintf(destino, tamano, "%s", m.nombre);
    } else if (lectura == ERROR_ELIMINADO) {
        snprintf(destino, tamano, "%s (BAJA)", m.nombre);
    } else {
        snprintf(destino, tamano, "(mecanico ID %d no encontrado)", id);
    }
}

void describirRepuesto(int id, char* destino, int tamano) {
    Repuesto r;
    Resultado lectura = obtenerRepuestoPorID(id, &r);
    if (lectura == OK || lectura == ERROR_ELIMINADO) {
        snprintf(destino, tamano, "%s - %s", r.codigo, r.nombre);
    } else {
        snprintf(destino, tamano, "(repuesto ID %d no encontrado)", id);
    }
}
