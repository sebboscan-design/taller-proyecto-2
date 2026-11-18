#ifndef MENU_VEHICULOS_HPP
#define MENU_VEHICULOS_HPP

// menu_vehiculos.hpp
// Menu de vehiculos. Pide los datos, llama a logica/vehiculos y muestra el resultado.

#include "../estructuras.hpp"

// menuVehiculos: muestra el submenu y repite hasta que elijan 0 (volver).
void menuVehiculos();

// menuRegistrarVehiculo: pide los 8 datos, confirma y llama a registrarVehiculo.
void menuRegistrarVehiculo();

// menuBuscarVehiculo: busca por ID (con el mensaje de depuracion del byte leido),
// por placa exacta o por parte del nombre del propietario.
void menuBuscarVehiculo();

// menuActualizarVehiculo: pide la placa, muestra el vehiculo, pide los datos nuevos,
// confirma y llama a actualizarVehiculo.
void menuActualizarVehiculo();

// menuEliminarVehiculo: pide la placa, confirma y llama a eliminarVehiculo.
// Si tiene ordenes, dice cuantas.
void menuEliminarVehiculo();

// menuListarVehiculos: tabla con todos los activos y el total.
void menuListarVehiculos();

// mostrarVehiculo: imprime los datos de un vehiculo.
void mostrarVehiculo(const Vehiculo* vehiculo);

#endif
