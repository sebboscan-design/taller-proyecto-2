#ifndef MENU_REPUESTOS_HPP
#define MENU_REPUESTOS_HPP

// menu_repuestos.hpp
// Menu del inventario. Los repuestos se piden por codigo (no por ID).

#include "../estructuras.hpp"

// menuRepuestos: muestra el submenu y repite hasta que elijan 0.
void menuRepuestos();

// menuRegistrarRepuesto: pide codigo, nombre, marca, precio, stock y minimo, confirma y registra.
void menuRegistrarRepuesto();

// menuBuscarRepuesto: busca por parte del codigo o del nombre.
void menuBuscarRepuesto();

// menuEntradaMercancia: pide el codigo y cuantas unidades llegaron.
void menuEntradaMercancia();

// menuCambiarPrecio: pide el codigo y el precio nuevo, confirma y lo cambia.
void menuCambiarPrecio();

// menuEliminarRepuesto: pide el codigo, confirma y lo elimina si no esta en ninguna orden.
void menuEliminarRepuesto();

// menuListarRepuestos: todos los activos, marcando los de BAJO STOCK.
void menuListarRepuestos();

// mostrarRepuesto: una linea con los datos del repuesto y [BAJO STOCK] si toca.
void mostrarRepuesto(const Repuesto* repuesto);

#endif
