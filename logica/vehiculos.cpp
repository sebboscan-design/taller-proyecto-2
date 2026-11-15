#include "vehiculos.hpp"
#include "ordenes.hpp"
#include "utilidades.hpp"
#include "../persistencia/vehiculos_archivo.hpp"
#include <cstring>

// Revisa los datos que se pueden cambiar (los mismos al registrar y al actualizar)
static bool datosVehiculoValidos(const Vehiculo* datos) {
    if (esTextoVacio(datos->marca) || esTextoVacio(datos->modelo)) return false;
    if (esTextoVacio(datos->nombrePropietario)) return false;
    if (datos->anio < 1950 || datos->anio > 2027) return false;
    if (!esCedulaValida(datos->cedulaPropietario)) return false;
    return true;
}

Resultado registrarVehiculo(Vehiculo datos, int* idNuevo) {
    convertirAMayusculas(datos.placa);
    if (!esPlacaValida(datos.placa)) return ERROR_DATO_INVALIDO;
    if (!datosVehiculoValidos(&datos)) return ERROR_DATO_INVALIDO;
    if (datos.kilometraje < 0) return ERROR_DATO_INVALIDO;

    // la placa de un vehiculo eliminado si se puede volver a usar
    Vehiculo existente;
    Resultado busqueda = obtenerVehiculoPorPlaca(datos.placa, &existente);
    if (busqueda == OK) return ERROR_DUPLICADO;
    if (busqueda == ERROR_ARCHIVO) return ERROR_ARCHIVO;

    if (!guardarVehiculoNuevo(&datos)) return ERROR_ARCHIVO;
    *idNuevo = datos.id;
    return OK;
}

Resultado obtenerVehiculoPorID(int id, Vehiculo* destino) {
    ResultadoArchivo lectura = leerVehiculo(id, destino);
    if (lectura == ARCHIVO_ERROR) return ERROR_ARCHIVO;
    if (lectura == ARCHIVO_ID_NO_EXISTE) return ERROR_NO_EXISTE;
    if (destino->eliminado) return ERROR_ELIMINADO;
    return OK;
}

Resultado obtenerVehiculoPorPlaca(const char* placa, Vehiculo* destino) {
    char buscada[10];
    copiarTexto(buscada, placa, 10);
    convertirAMayusculas(buscada);

    // aqui si hay que recorrer, pero con un solo vehiculo en memoria a la vez
    int total = totalVehiculos();
    if (total < 0) return ERROR_ARCHIVO;
    for (int id = 1; id <= total; id++) {
        Vehiculo actual;
        if (leerVehiculo(id, &actual) != ARCHIVO_OK) return ERROR_ARCHIVO;
        if (!actual.eliminado && strcmp(actual.placa, buscada) == 0) {
            *destino = actual;
            return OK;
        }
    }
    return ERROR_NO_EXISTE;
}

long posicionVehiculoEnArchivo(int id) {
    return posicionRegistro(id, sizeof(Vehiculo));
}

// Recorre los activos y guarda los que cumplan el filtro (texto vacio = todos)
static Resultado juntarVehiculos(const char* textoPropietario, ListaVehiculos* resultado) {
    resultado->cantidad = 0;
    resultado->hayMas = false;

    int total = totalVehiculos();
    if (total < 0) return ERROR_ARCHIVO;
    for (int id = 1; id <= total; id++) {
        Vehiculo actual;
        if (leerVehiculo(id, &actual) != ARCHIVO_OK) return ERROR_ARCHIVO;
        if (actual.eliminado) continue;
        if (!contieneSinMayusculas(actual.nombrePropietario, textoPropietario)) continue;

        if (resultado->cantidad == MAX_RESULTADOS) {
            resultado->hayMas = true;
            break;
        }
        resultado->items[resultado->cantidad] = actual;
        resultado->cantidad++;
    }
    return OK;
}

Resultado buscarVehiculosPorPropietario(const char* texto, ListaVehiculos* resultado) {
    return juntarVehiculos(texto, resultado);
}

Resultado listarVehiculos(ListaVehiculos* resultado) {
    return juntarVehiculos("", resultado);
}

Resultado actualizarVehiculo(int id, Vehiculo datos) {
    Vehiculo actual;
    Resultado lectura = obtenerVehiculoPorID(id, &actual);
    if (lectura != OK) return lectura;
    if (!datosVehiculoValidos(&datos)) return ERROR_DATO_INVALIDO;

    // se copian solo los campos que se pueden cambiar
    copiarTexto(actual.marca, datos.marca, 50);
    copiarTexto(actual.modelo, datos.modelo, 50);
    actual.anio = datos.anio;
    copiarTexto(actual.nombrePropietario, datos.nombrePropietario, 100);
    copiarTexto(actual.cedulaPropietario, datos.cedulaPropietario, 20);
    copiarTexto(actual.telefonoPropietario, datos.telefonoPropietario, 20);

    if (!sobrescribirVehiculo(&actual)) return ERROR_ARCHIVO;
    return OK;
}

Resultado eliminarVehiculo(int id, int* ordenesEncontradas) {
    Vehiculo actual;
    Resultado lectura = obtenerVehiculoPorID(id, &actual);
    if (lectura != OK) return lectura;

    // no se puede si tiene ordenes en cualquier estado
    int ordenes = contarOrdenesDeVehiculo(id);
    if (ordenes < 0) return ERROR_ARCHIVO;
    if (ordenes > 0) {
        *ordenesEncontradas = ordenes;
        return ERROR_TIENE_ORDENES;
    }

    if (!marcarVehiculoEliminado(id)) return ERROR_ARCHIVO;
    return OK;
}
