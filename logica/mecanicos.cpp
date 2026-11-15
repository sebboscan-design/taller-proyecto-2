#include "mecanicos.hpp"
#include "ordenes.hpp"
#include "utilidades.hpp"
#include "../persistencia/mecanicos_archivo.hpp"
#include <cstring>

// Busca la cedula entre todos los mecanicos. true si ya existe
static Resultado cedulaYaRegistrada(const char* cedula, bool* existe) {
    *existe = false;
    int total = totalMecanicos();
    if (total < 0) return ERROR_ARCHIVO;
    for (int id = 1; id <= total; id++) {
        Mecanico actual;
        if (leerMecanico(id, &actual) != ARCHIVO_OK) return ERROR_ARCHIVO;
        if (strcmp(actual.cedula, cedula) == 0) {
            *existe = true;
            return OK;
        }
    }
    return OK;
}

Resultado registrarMecanico(Mecanico datos, int* idNuevo) {
    convertirAMayusculas(datos.especialidad);
    if (esTextoVacio(datos.nombre)) return ERROR_DATO_INVALIDO;
    if (!esCedulaValida(datos.cedula)) return ERROR_DATO_INVALIDO;
    if (!esEspecialidadValida(datos.especialidad)) return ERROR_DATO_INVALIDO;
    if (!esFechaValida(datos.fechaIngreso)) return ERROR_DATO_INVALIDO;

    bool repetida;
    if (cedulaYaRegistrada(datos.cedula, &repetida) != OK) return ERROR_ARCHIVO;
    if (repetida) return ERROR_DUPLICADO;

    if (!guardarMecanicoNuevo(&datos)) return ERROR_ARCHIVO;
    *idNuevo = datos.id;
    return OK;
}

Resultado obtenerMecanicoPorID(int id, Mecanico* destino) {
    ResultadoArchivo lectura = leerMecanico(id, destino);
    if (lectura == ARCHIVO_ERROR) return ERROR_ARCHIVO;
    if (lectura == ARCHIVO_ID_NO_EXISTE) return ERROR_NO_EXISTE;
    if (destino->eliminado) return ERROR_ELIMINADO;
    return OK;
}

Resultado listarMecanicos(ListaMecanicos* resultado) {
    resultado->cantidad = 0;
    resultado->hayMas = false;

    int total = totalMecanicos();
    if (total < 0) return ERROR_ARCHIVO;
    for (int id = 1; id <= total; id++) {
        if (resultado->cantidad == MAX_RESULTADOS) {
            resultado->hayMas = true;
            break;
        }
        Mecanico actual;
        if (leerMecanico(id, &actual) != ARCHIVO_OK) return ERROR_ARCHIVO;

        int abiertas = contarOrdenesAbiertasMecanico(id);
        if (abiertas < 0) return ERROR_ARCHIVO;

        resultado->items[resultado->cantidad] = actual;
        resultado->ordenesAbiertas[resultado->cantidad] = abiertas;
        resultado->cantidad++;
    }
    return OK;
}

Resultado darDeBajaMecanico(int id, int* ordenesAbiertas) {
    Mecanico actual;
    Resultado lectura = obtenerMecanicoPorID(id, &actual);
    if (lectura != OK) return lectura; // incluye ERROR_ELIMINADO si ya estaba de baja

    int abiertas = contarOrdenesAbiertasMecanico(id);
    if (abiertas < 0) return ERROR_ARCHIVO;
    if (abiertas > 0) {
        *ordenesAbiertas = abiertas;
        return ERROR_TIENE_ORDENES;
    }

    if (!marcarMecanicoEliminado(id)) return ERROR_ARCHIVO;
    return OK;
}
