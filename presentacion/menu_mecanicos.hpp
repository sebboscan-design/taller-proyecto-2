#ifndef MENU_MECANICOS_HPP
#define MENU_MECANICOS_HPP

// menu_mecanicos.hpp
// Menu de mecanicos. Pide los datos, llama a logica/mecanicos y muestra el resultado.

// menuMecanicos: muestra el submenu y repite hasta que elijan 0.
void menuMecanicos();

// menuRegistrarMecanico: pide nombre, cedula, especialidad y fecha, confirma y registra.
void menuRegistrarMecanico();

// menuBuscarMecanico: muestra un mecanico por ID, si esta activo o de baja y sus ordenes abiertas.
void menuBuscarMecanico();

// menuListarMecanicos: tabla con todos (activos y de baja) y sus ordenes abiertas.
void menuListarMecanicos();

// menuDarDeBajaMecanico: pide el ID, confirma y llama a darDeBajaMecanico.
// Si tiene ordenes abiertas, dice cuantas.
void menuDarDeBajaMecanico();

#endif
