#include "reportes.hpp"
#include "vehiculos.hpp"
#include "ordenes.hpp"
#include "../persistencia/ordenes_archivo.hpp"
#include "../persistencia/repuestos_archivo.hpp"
#include "../persistencia/mecanicos_archivo.hpp"
#include <cstring>

// Todos los reportes leen de los archivos un registro a la vez

Resultado historialVehiculo(const char* placa, Vehiculo* vehiculo, ListaOrdenes* ordenes,
                            float* totalFacturado) {
    Resultado busqueda = obtenerVehiculoPorPlaca(placa, vehiculo);
    if (busqueda != OK) return busqueda;

    Resultado lista = listarOrdenesPorVehiculo(vehiculo->id, ordenes);
    if (lista != OK) return lista;

    // el total se saca recorriendo todas sus ordenes, no solo las que caben en la lista
    *totalFacturado = 0;
    int total = totalOrdenes();
    if (total < 0) return ERROR_ARCHIVO;
    for (int id = 1; id <= total; id++) {
        OrdenServicio orden;
        if (leerOrden(id, &orden) != ARCHIVO_OK) return ERROR_ARCHIVO;
        if (orden.idVehiculo == vehiculo->id && strcmp(orden.estado, "CERRADA") == 0) {
            *totalFacturado += calcularTotalOrden(&orden);
        }
    }
    return OK;
}

// Unidades de ese repuesto en todas las ordenes CERRADAS
static int unidadesConsumidas(int idRepuesto, int totalDeOrdenes, bool* error) {
    int unidades = 0;
    for (int id = 1; id <= totalDeOrdenes; id++) {
        OrdenServicio orden;
        if (leerOrden(id, &orden) != ARCHIVO_OK) {
            *error = true;
            return 0;
        }
        if (strcmp(orden.estado, "CERRADA") != 0) continue;
        for (int i = 0; i < orden.numLineas; i++) {
            if (orden.lineas[i].idRepuesto == idRepuesto) unidades += orden.lineas[i].cantidad;
        }
    }
    return unidades;
}

Resultado repuestosMasUtilizados(TopRepuestos* top) {
    top->cantidad = 0;
    int totalDeRepuestos = totalRepuestos();
    int totalDeOrdenes = totalOrdenes();
    if (totalDeRepuestos < 0 || totalDeOrdenes < 0) return ERROR_ARCHIVO;

    // por cada repuesto se cuentan sus unidades y se mete en el top si le toca.
    // Asi en memoria solo esta el top de 10, nunca todos los repuestos
    for (int idRepuesto = 1; idRepuesto <= totalDeRepuestos; idRepuesto++) {
        bool error = false;
        int unidades = unidadesConsumidas(idRepuesto, totalDeOrdenes, &error);
        if (error) return ERROR_ARCHIVO;
        if (unidades == 0) continue;

        // si el top esta lleno y este no le gana al ultimo, no entra
        if (top->cantidad == TOP_REPUESTOS && unidades <= top->filas[TOP_REPUESTOS - 1].unidades) continue;

        Repuesto repuesto;
        if (leerRepuesto(idRepuesto, &repuesto) != ARCHIVO_OK) return ERROR_ARCHIVO;

        // se busca su lugar y se corren los de abajo (como insertar en una lista ordenada)
        int lugar = top->cantidad < TOP_REPUESTOS ? top->cantidad : TOP_REPUESTOS - 1;
        while (lugar > 0 && top->filas[lugar - 1].unidades < unidades) {
            if (lugar < TOP_REPUESTOS) top->filas[lugar] = top->filas[lugar - 1];
            lugar--;
        }
        top->filas[lugar].repuesto = repuesto;
        top->filas[lugar].unidades = unidades;
        if (top->cantidad < TOP_REPUESTOS) top->cantidad++;
    }
    return OK;
}

Resultado facturacionPorMecanico(ListaFacturacion* resultado) {
    resultado->cantidad = 0;
    resultado->hayMas = false;

    int totalDeMecanicos = totalMecanicos();
    int totalDeOrdenes = totalOrdenes();
    if (totalDeMecanicos < 0 || totalDeOrdenes < 0) return ERROR_ARCHIVO;

    for (int idMecanico = 1; idMecanico <= totalDeMecanicos; idMecanico++) {
        if (resultado->cantidad == MAX_RESULTADOS) {
            resultado->hayMas = true;
            break;
        }
        FilaFacturacion fila;
        if (leerMecanico(idMecanico, &fila.mecanico) != ARCHIVO_OK) return ERROR_ARCHIVO;
        fila.ordenesCerradas = 0;
        fila.manoObra = 0;
        fila.repuestos = 0;

        // solo cuentan las CERRADAS: las abiertas no se han cobrado y las anuladas no valen
        for (int id = 1; id <= totalDeOrdenes; id++) {
            OrdenServicio orden;
            if (leerOrden(id, &orden) != ARCHIVO_OK) return ERROR_ARCHIVO;
            if (orden.idMecanico != idMecanico || strcmp(orden.estado, "CERRADA") != 0) continue;
            fila.ordenesCerradas++;
            fila.manoObra += orden.manoObra;
            fila.repuestos += calcularSubtotalRepuestos(&orden);
        }

        resultado->filas[resultado->cantidad] = fila;
        resultado->cantidad++;
    }
    return OK;
}
