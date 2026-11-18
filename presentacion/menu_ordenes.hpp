#ifndef MENU_ORDENES_HPP
#define MENU_ORDENES_HPP

// menu_ordenes.hpp
// Menu de ordenes de servicio. El vehiculo se pide por placa y el repuesto por
// codigo; los mensajes de error dicen el dato exacto que fallo.

#include "../estructuras.hpp"

// menuOrdenes: muestra el submenu y repite hasta que elijan 0.
void menuOrdenes();

// menuAbrirOrden: pide placa, mecanico, km y falla, confirma y abre la orden.
void menuAbrirOrden();

// menuAgregarRepuestoOrden: pide orden, codigo y cantidad y los agrega.
void menuAgregarRepuestoOrden();

// menuQuitarRepuestoOrden: pide orden y codigo y quita esa linea.
void menuQuitarRepuestoOrden();

// menuCerrarOrden: pide orden y mano de obra, confirma y la cierra.
void menuCerrarOrden();

// menuAnularOrden: pide la orden, confirma y la anula (devuelve el stock).
void menuAnularOrden();

// menuVerDetalleOrden: pide la orden y muestra su detalle completo.
void menuVerDetalleOrden();

// menuListarOrdenes: todas, por estado o el historial de un vehiculo.
void menuListarOrdenes();

// mostrarDetalleOrden: la orden completa: estado, fechas, vehiculo, dueno,
// mecanico, falla, una fila por linea y los totales.
void mostrarDetalleOrden(const OrdenServicio* orden);

// mostrarFilaOrden: una fila de tabla con numero, fecha, vehiculo, mecanico, estado y total.
void mostrarFilaOrden(const OrdenServicio* orden);

#endif
