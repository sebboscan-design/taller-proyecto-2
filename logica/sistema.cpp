#include "sistema.hpp"
#include "utilidades.hpp"
#include "../persistencia/archivos.hpp"

Resultado prepararArchivos() {
    if (!prepararCarpetaDatos()) {
        anotarErrorArchivo(RUTA_DATOS);
        return ERROR_ARCHIVO;
    }
    const char* rutas[] = {RUTA_VEHICULOS, RUTA_MECANICOS, RUTA_REPUESTOS, RUTA_ORDENES};
    for (int i = 0; i < 4; i++) {
        if (!inicializarArchivo(rutas[i])) return ERROR_ARCHIVO;
    }
    return OK;
}

bool tallerRegistrado() {
    return existeTaller();
}

Resultado registrarTaller(Taller datos) {
    if (esTextoVacio(datos.nombre) || esTextoVacio(datos.rif)) return ERROR_DATO_INVALIDO;
    if (!guardarTaller(&datos)) return ERROR_ARCHIVO;
    return OK;
}

Resultado obtenerTaller(Taller* destino) {
    if (!leerTaller(destino)) return ERROR_ARCHIVO;
    return OK;
}

const char* archivoConError() {
    return ultimoArchivoConError();
}
