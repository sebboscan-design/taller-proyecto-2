#ifndef LISTAS_HPP
#define LISTAS_HPP

// listas.hpp
// Las listas que llenan las busquedas de varios registros.
//
// Como los datos estan en disco, una busqueda no puede devolver punteros a un
// arreglo en memoria (ese arreglo ya no existe). En su lugar llena una de estas
// listas con COPIAS de los registros que encontro, hasta MAX_RESULTADOS.
// Si habia mas, pone hayMas en true para que el menu lo avise.
// Son arreglos de tamano fijo, asi que no hacen falta new ni delete[].

#include "../estructuras.hpp"

// Maximo de resultados que entrega una busqueda
const int MAX_RESULTADOS = 20;

struct ListaVehiculos {
    Vehiculo items[MAX_RESULTADOS]; // copias de los vehiculos encontrados
    int cantidad;                   // cuantas posiciones de items valen
    bool hayMas;                    // true si quedaron vehiculos sin entrar
};

struct ListaMecanicos {
    Mecanico items[MAX_RESULTADOS];
    int ordenesAbiertas[MAX_RESULTADOS]; // ordenes ABIERTAS de items[i]
    int cantidad;
    bool hayMas;
};

struct ListaRepuestos {
    Repuesto items[MAX_RESULTADOS];
    int cantidad;
    bool hayMas;
};

struct ListaOrdenes {
    OrdenServicio items[MAX_RESULTADOS];
    int cantidad;
    bool hayMas;
};

#endif
