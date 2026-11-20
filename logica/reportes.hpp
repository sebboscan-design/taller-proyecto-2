#ifndef REPORTES_HPP
#define REPORTES_HPP

// reportes.hpp
// Los tres reportes. Todos leen los archivos un registro a la vez.

#include "../estructuras.hpp"
#include "resultados.hpp"
#include "listas.hpp"

// historialVehiculo (reporte 1): busca el vehiculo activo con esa placa,
// llena "ordenes" con sus ordenes (hasta MAX_RESULTADOS, con hayMas) y suma en
// *totalFacturado el total de TODAS sus ordenes CERRADAS (no solo las de la lista).
// Devuelve: OK, ERROR_NO_EXISTE o ERROR_ARCHIVO.
Resultado historialVehiculo(const char* placa, Vehiculo* vehiculo, ListaOrdenes* ordenes,
                            float* totalFacturado);

// Cuantos repuestos muestra el reporte 2
const int TOP_REPUESTOS = 10;

struct FilaTopRepuesto {
    Repuesto repuesto;
    int unidades; // unidades consumidas en ordenes CERRADAS
};

struct TopRepuestos {
    FilaTopRepuesto filas[TOP_REPUESTOS]; // ordenadas de mayor a menor
    int cantidad;                         // cuantas filas valen (puede ser menos de 10)
};

// repuestosMasUtilizados (reporte 2): los 10 repuestos con mas unidades en
// ordenes CERRADAS. En memoria solo se guarda el top, nunca todos los repuestos.
// Devuelve: OK o ERROR_ARCHIVO.
Resultado repuestosMasUtilizados(TopRepuestos* top);

struct FilaFacturacion {
    Mecanico mecanico;
    int ordenesCerradas;
    float manoObra;  // suma de mano de obra de sus ordenes CERRADAS
    float repuestos; // suma de repuestos de esas mismas ordenes
};

struct ListaFacturacion {
    FilaFacturacion filas[MAX_RESULTADOS];
    int cantidad;
    bool hayMas;
};

// facturacionPorMecanico (reporte 3): por cada mecanico (activos y de baja)
// cuenta sus ordenes CERRADAS y suma mano de obra y repuestos. Las ABIERTAS y
// ANULADAS no cuentan.
// Devuelve: OK o ERROR_ARCHIVO.
Resultado facturacionPorMecanico(ListaFacturacion* resultado);

#endif
