#include "ordenes.hpp"
#include "vehiculos.hpp"
#include "mecanicos.hpp"
#include "repuestos.hpp"
#include "utilidades.hpp"
#include "../persistencia/ordenes_archivo.hpp"
#include "../persistencia/vehiculos_archivo.hpp"
#include "../persistencia/repuestos_archivo.hpp"
#include <cstring>

// Posicion de la linea de ese repuesto dentro de la orden, o -1 si no esta
static int buscarLinea(const OrdenServicio* orden, int idRepuesto) {
    for (int i = 0; i < orden->numLineas; i++) {
        if (orden->lineas[i].idRepuesto == idRepuesto) return i;
    }
    return -1;
}

Resultado abrirOrden(int idVehiculo, int idMecanico, int kilometrajeIngreso,
                     const char* descripcionFalla, int* idNuevaOrden) {
    // primero todas las validaciones
    Vehiculo vehiculo;
    Resultado lecturaVehiculo = obtenerVehiculoPorID(idVehiculo, &vehiculo);
    if (lecturaVehiculo == ERROR_ARCHIVO) return ERROR_ARCHIVO;
    if (lecturaVehiculo != OK) return ERROR_VEHICULO_NO_DISPONIBLE;

    Mecanico mecanico;
    Resultado lecturaMecanico = obtenerMecanicoPorID(idMecanico, &mecanico);
    if (lecturaMecanico == ERROR_ARCHIVO) return ERROR_ARCHIVO;
    if (lecturaMecanico == ERROR_NO_EXISTE) return ERROR_MECANICO_NO_EXISTE;
    if (lecturaMecanico == ERROR_ELIMINADO) return ERROR_MECANICO_DE_BAJA;

    if (esTextoVacio(descripcionFalla)) return ERROR_DATO_INVALIDO;

    int ordenAbierta;
    Resultado abierta = buscarOrdenAbiertaDeVehiculo(idVehiculo, &ordenAbierta);
    if (abierta == ERROR_ARCHIVO) return ERROR_ARCHIVO;
    if (abierta == OK) return ERROR_VEHICULO_CON_ORDEN_ABIERTA;

    int abiertasMecanico = contarOrdenesAbiertasMecanico(idMecanico);
    if (abiertasMecanico < 0) return ERROR_ARCHIVO;
    if (abiertasMecanico >= MAX_ORDENES_ABIERTAS_MECANICO) return ERROR_MECANICO_SIN_CUPO;

    if (kilometrajeIngreso < vehiculo.kilometraje) return ERROR_KILOMETRAJE_MENOR;

    // se arma la orden desde cero para que no queden bytes basura en el archivo
    OrdenServicio orden;
    memset(&orden, 0, sizeof(OrdenServicio));
    orden.idVehiculo = idVehiculo;
    orden.idMecanico = idMecanico;
    obtenerFechaActual(orden.fechaApertura);
    copiarTexto(orden.estado, "ABIERTA", 12);
    copiarTexto(orden.descripcionFalla, descripcionFalla, 200);
    orden.kilometrajeIngreso = kilometrajeIngreso;
    orden.manoObra = 0;
    orden.numLineas = 0;

    // escritura 1: la orden. Escritura 2: el km del vehiculo
    if (!guardarOrdenNueva(&orden)) return ERROR_ARCHIVO;
    vehiculo.kilometraje = kilometrajeIngreso;
    if (!sobrescribirVehiculo(&vehiculo)) return ERROR_ARCHIVO;

    *idNuevaOrden = orden.id;
    return OK;
}

Resultado agregarRepuestoAOrden(int idOrden, int idRepuesto, int cantidad) {
    if (cantidad <= 0) return ERROR_DATO_INVALIDO;

    OrdenServicio orden;
    Resultado lecturaOrden = obtenerOrdenPorID(idOrden, &orden);
    if (lecturaOrden != OK) return lecturaOrden;
    if (strcmp(orden.estado, "ABIERTA") != 0) return ERROR_ESTADO_INVALIDO;

    Repuesto repuesto;
    Resultado lecturaRepuesto = obtenerRepuestoPorID(idRepuesto, &repuesto);
    if (lecturaRepuesto != OK) return lecturaRepuesto;
    if (repuesto.stock < cantidad) return ERROR_STOCK_INSUFICIENTE;

    int posicion = buscarLinea(&orden, idRepuesto);
    if (posicion == -1 && orden.numLineas == MAX_LINEAS_ORDEN) return ERROR_LIMITE_LINEAS;

    if (posicion != -1) {
        // ya estaba: se suma a su linea y se queda con el precio que tenia
        orden.lineas[posicion].cantidad += cantidad;
    } else {
        orden.lineas[orden.numLineas].idRepuesto = idRepuesto;
        orden.lineas[orden.numLineas].cantidad = cantidad;
        orden.lineas[orden.numLineas].precioUnitario = repuesto.precio;
        orden.numLineas++;
    }
    repuesto.stock -= cantidad;

    // escritura 1: el stock baja. Escritura 2: la linea en la orden.
    // Si se corta en medio, falta stock pero nunca sobra
    if (!sobrescribirRepuesto(&repuesto)) return ERROR_ARCHIVO;
    if (!sobrescribirOrden(&orden)) return ERROR_ARCHIVO;
    return OK;
}

Resultado quitarRepuestoDeOrden(int idOrden, int idRepuesto) {
    OrdenServicio orden;
    Resultado lecturaOrden = obtenerOrdenPorID(idOrden, &orden);
    if (lecturaOrden != OK) return lecturaOrden;
    if (strcmp(orden.estado, "ABIERTA") != 0) return ERROR_ESTADO_INVALIDO;

    int posicion = buscarLinea(&orden, idRepuesto);
    if (posicion == -1) return ERROR_REPUESTO_NO_ESTA;

    Repuesto repuesto;
    if (leerRepuesto(idRepuesto, &repuesto) != ARCHIVO_OK) return ERROR_ARCHIVO;

    int unidades = orden.lineas[posicion].cantidad;

    // se corren las lineas de despues para tapar el hueco
    for (int i = posicion; i < orden.numLineas - 1; i++) {
        orden.lineas[i] = orden.lineas[i + 1];
    }
    orden.numLineas--;
    memset(&orden.lineas[orden.numLineas], 0, sizeof(DetalleOrden)); // la que sobra queda en cero
    repuesto.stock += unidades;

    // escritura 1: se quita la linea. Escritura 2: vuelve el stock
    if (!sobrescribirOrden(&orden)) return ERROR_ARCHIVO;
    if (!sobrescribirRepuesto(&repuesto)) return ERROR_ARCHIVO;
    return OK;
}

Resultado cerrarOrden(int idOrden, float manoObra) {
    OrdenServicio orden;
    Resultado lecturaOrden = obtenerOrdenPorID(idOrden, &orden);
    if (lecturaOrden != OK) return lecturaOrden;
    if (strcmp(orden.estado, "ABIERTA") != 0) return ERROR_ESTADO_INVALIDO;
    if (manoObra < 0) return ERROR_DATO_INVALIDO;
    if (orden.numLineas == 0 && manoObra == 0) return ERROR_ORDEN_VACIA;

    orden.manoObra = manoObra;
    copiarTexto(orden.estado, "CERRADA", 12);
    obtenerFechaActual(orden.fechaCierre);

    if (!sobrescribirOrden(&orden)) return ERROR_ARCHIVO;
    return OK;
}

Resultado anularOrden(int idOrden) {
    OrdenServicio orden;
    Resultado lecturaOrden = obtenerOrdenPorID(idOrden, &orden);
    if (lecturaOrden != OK) return lecturaOrden;
    if (strcmp(orden.estado, "ANULADA") == 0) return ERROR_ESTADO_INVALIDO;

    // se revisa antes que todos los repuestos se puedan leer
    for (int i = 0; i < orden.numLineas; i++) {
        Repuesto repuesto;
        if (leerRepuesto(orden.lineas[i].idRepuesto, &repuesto) != ARCHIVO_OK) return ERROR_ARCHIVO;
    }

    // escritura 1: la orden queda ANULADA. Despues se devuelve el stock linea por linea
    copiarTexto(orden.estado, "ANULADA", 12);
    if (!sobrescribirOrden(&orden)) return ERROR_ARCHIVO;

    for (int i = 0; i < orden.numLineas; i++) {
        Repuesto repuesto;
        if (leerRepuesto(orden.lineas[i].idRepuesto, &repuesto) != ARCHIVO_OK) return ERROR_ARCHIVO;
        repuesto.stock += orden.lineas[i].cantidad;
        if (!sobrescribirRepuesto(&repuesto)) return ERROR_ARCHIVO;
    }
    return OK;
}

float calcularSubtotalRepuestos(const OrdenServicio* orden) {
    float subtotal = 0;
    for (int i = 0; i < orden->numLineas; i++) {
        subtotal += orden->lineas[i].cantidad * orden->lineas[i].precioUnitario;
    }
    return subtotal;
}

float calcularTotalOrden(const OrdenServicio* orden) {
    // el total no se guarda: se calcula siempre con los precios historicos
    return calcularSubtotalRepuestos(orden) + orden->manoObra;
}

Resultado obtenerOrdenPorID(int id, OrdenServicio* destino) {
    ResultadoArchivo lectura = leerOrden(id, destino);
    if (lectura == ARCHIVO_ERROR) return ERROR_ARCHIVO;
    if (lectura == ARCHIVO_ID_NO_EXISTE) return ERROR_NO_EXISTE;
    return OK;
}

// Junta ordenes que cumplan el filtro. estado vacio = cualquiera, idVehiculo 0 = cualquiera
static Resultado juntarOrdenes(const char* estado, int idVehiculo, ListaOrdenes* resultado) {
    resultado->cantidad = 0;
    resultado->hayMas = false;

    int total = totalOrdenes();
    if (total < 0) return ERROR_ARCHIVO;
    for (int id = 1; id <= total; id++) {
        OrdenServicio actual;
        if (leerOrden(id, &actual) != ARCHIVO_OK) return ERROR_ARCHIVO;
        if (estado[0] != '\0' && strcmp(actual.estado, estado) != 0) continue;
        if (idVehiculo != 0 && actual.idVehiculo != idVehiculo) continue;

        if (resultado->cantidad == MAX_RESULTADOS) {
            resultado->hayMas = true;
            break;
        }
        resultado->items[resultado->cantidad] = actual;
        resultado->cantidad++;
    }
    return OK;
}

Resultado listarOrdenes(ListaOrdenes* resultado) {
    return juntarOrdenes("", 0, resultado);
}

Resultado listarOrdenesPorEstado(const char* estado, ListaOrdenes* resultado) {
    return juntarOrdenes(estado, 0, resultado);
}

Resultado listarOrdenesPorVehiculo(int idVehiculo, ListaOrdenes* resultado) {
    return juntarOrdenes("", idVehiculo, resultado);
}

int contarOrdenesDeVehiculo(int idVehiculo) {
    int total = totalOrdenes();
    if (total < 0) return -1;
    int cuenta = 0;
    for (int id = 1; id <= total; id++) {
        OrdenServicio actual;
        if (leerOrden(id, &actual) != ARCHIVO_OK) return -1;
        if (actual.idVehiculo == idVehiculo) cuenta++;
    }
    return cuenta;
}

int contarOrdenesAbiertasMecanico(int idMecanico) {
    int total = totalOrdenes();
    if (total < 0) return -1;
    int cuenta = 0;
    for (int id = 1; id <= total; id++) {
        OrdenServicio actual;
        if (leerOrden(id, &actual) != ARCHIVO_OK) return -1;
        if (actual.idMecanico == idMecanico && strcmp(actual.estado, "ABIERTA") == 0) cuenta++;
    }
    return cuenta;
}

int contarOrdenesConRepuesto(int idRepuesto) {
    int total = totalOrdenes();
    if (total < 0) return -1;
    int cuenta = 0;
    for (int id = 1; id <= total; id++) {
        OrdenServicio actual;
        if (leerOrden(id, &actual) != ARCHIVO_OK) return -1;
        if (buscarLinea(&actual, idRepuesto) != -1) cuenta++;
    }
    return cuenta;
}

Resultado buscarOrdenAbiertaDeVehiculo(int idVehiculo, int* idOrden) {
    int total = totalOrdenes();
    if (total < 0) return ERROR_ARCHIVO;
    for (int id = 1; id <= total; id++) {
        OrdenServicio actual;
        if (leerOrden(id, &actual) != ARCHIVO_OK) return ERROR_ARCHIVO;
        if (actual.idVehiculo == idVehiculo && strcmp(actual.estado, "ABIERTA") == 0) {
            *idOrden = id;
            return OK;
        }
    }
    return ERROR_NO_EXISTE;
}
