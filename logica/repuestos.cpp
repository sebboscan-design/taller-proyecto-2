#include "repuestos.hpp"
#include "ordenes.hpp"
#include "utilidades.hpp"
#include "../persistencia/repuestos_archivo.hpp"
#include <cstring>

Resultado registrarRepuesto(Repuesto datos, int* idNuevo) {
    convertirAMayusculas(datos.codigo);
    if (esTextoVacio(datos.codigo) || esTextoVacio(datos.nombre) || esTextoVacio(datos.marca)) {
        return ERROR_DATO_INVALIDO;
    }
    if (datos.precio <= 0 || datos.stock < 0 || datos.stockMinimo < 0) return ERROR_DATO_INVALIDO;

    Repuesto existente;
    Resultado busqueda = obtenerRepuestoPorCodigo(datos.codigo, &existente);
    if (busqueda == OK) return ERROR_DUPLICADO;
    if (busqueda == ERROR_ARCHIVO) return ERROR_ARCHIVO;

    if (!guardarRepuestoNuevo(&datos)) return ERROR_ARCHIVO;
    *idNuevo = datos.id;
    return OK;
}

Resultado obtenerRepuestoPorID(int id, Repuesto* destino) {
    ResultadoArchivo lectura = leerRepuesto(id, destino);
    if (lectura == ARCHIVO_ERROR) return ERROR_ARCHIVO;
    if (lectura == ARCHIVO_ID_NO_EXISTE) return ERROR_NO_EXISTE;
    if (destino->eliminado) return ERROR_ELIMINADO;
    return OK;
}

Resultado obtenerRepuestoPorCodigo(const char* codigo, Repuesto* destino) {
    char buscado[20];
    copiarTexto(buscado, codigo, 20);
    convertirAMayusculas(buscado);

    int total = totalRepuestos();
    if (total < 0) return ERROR_ARCHIVO;
    for (int id = 1; id <= total; id++) {
        Repuesto actual;
        if (leerRepuesto(id, &actual) != ARCHIVO_OK) return ERROR_ARCHIVO;
        if (!actual.eliminado && strcmp(actual.codigo, buscado) == 0) {
            *destino = actual;
            return OK;
        }
    }
    return ERROR_NO_EXISTE;
}

// Recorre los activos y guarda los que tengan el texto en el codigo o el nombre
static Resultado juntarRepuestos(const char* texto, ListaRepuestos* resultado) {
    resultado->cantidad = 0;
    resultado->hayMas = false;

    int total = totalRepuestos();
    if (total < 0) return ERROR_ARCHIVO;
    for (int id = 1; id <= total; id++) {
        Repuesto actual;
        if (leerRepuesto(id, &actual) != ARCHIVO_OK) return ERROR_ARCHIVO;
        if (actual.eliminado) continue;
        if (!contieneSinMayusculas(actual.codigo, texto) && !contieneSinMayusculas(actual.nombre, texto)) {
            continue;
        }
        if (resultado->cantidad == MAX_RESULTADOS) {
            resultado->hayMas = true;
            break;
        }
        resultado->items[resultado->cantidad] = actual;
        resultado->cantidad++;
    }
    return OK;
}

Resultado buscarRepuestos(const char* texto, ListaRepuestos* resultado) {
    return juntarRepuestos(texto, resultado);
}

Resultado listarRepuestos(ListaRepuestos* resultado) {
    return juntarRepuestos("", resultado);
}

Resultado registrarEntradaMercancia(int idRepuesto, int cantidad) {
    if (cantidad <= 0) return ERROR_DATO_INVALIDO;
    Repuesto actual;
    Resultado lectura = obtenerRepuestoPorID(idRepuesto, &actual);
    if (lectura != OK) return lectura;

    actual.stock += cantidad;
    if (!sobrescribirRepuesto(&actual)) return ERROR_ARCHIVO;
    return OK;
}

Resultado cambiarPrecioRepuesto(int idRepuesto, float nuevoPrecio) {
    if (nuevoPrecio <= 0) return ERROR_DATO_INVALIDO;
    Repuesto actual;
    Resultado lectura = obtenerRepuestoPorID(idRepuesto, &actual);
    if (lectura != OK) return lectura;

    // las ordenes guardan su propio precioUnitario, asi que no se enteran de esto
    actual.precio = nuevoPrecio;
    if (!sobrescribirRepuesto(&actual)) return ERROR_ARCHIVO;
    return OK;
}

Resultado eliminarRepuesto(int idRepuesto) {
    Repuesto actual;
    Resultado lectura = obtenerRepuestoPorID(idRepuesto, &actual);
    if (lectura != OK) return lectura;

    int usos = contarOrdenesConRepuesto(idRepuesto);
    if (usos < 0) return ERROR_ARCHIVO;
    if (usos > 0) return ERROR_EN_USO;

    if (!marcarRepuestoEliminado(idRepuesto)) return ERROR_ARCHIVO;
    return OK;
}

bool tieneBajoStock(const Repuesto* repuesto) {
    return repuesto->stock <= repuesto->stockMinimo;
}
