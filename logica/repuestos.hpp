#ifndef REPUESTOS_HPP
#define REPUESTOS_HPP

// repuestos.hpp
// Reglas del inventario. Lee y escribe a traves de persistencia/repuestos_archivo.

#include "../estructuras.hpp"
#include "resultados.hpp"
#include "listas.hpp"

// registrarRepuesto: valida y guarda un repuesto nuevo.
// Reglas: codigo unico entre los activos (se guarda en mayusculas), codigo,
// nombre y marca no vacios, precio > 0, stock y stock minimo >= 0.
// Devuelve: OK y el ID en *idNuevo; ERROR_DUPLICADO, ERROR_DATO_INVALIDO o ERROR_ARCHIVO.
Resultado registrarRepuesto(Repuesto datos, int* idNuevo);

// obtenerRepuestoPorID: lee el repuesto por calculo de posicion.
// Devuelve: OK; ERROR_ELIMINADO (igual llena destino, para que las ordenes
// viejas muestren su nombre); ERROR_NO_EXISTE o ERROR_ARCHIVO.
Resultado obtenerRepuestoPorID(int id, Repuesto* destino);

// obtenerRepuestoPorCodigo: busca el codigo exacto entre los activos (sin importar mayusculas).
// Devuelve: OK, ERROR_NO_EXISTE o ERROR_ARCHIVO.
Resultado obtenerRepuestoPorCodigo(const char* codigo, Repuesto* destino);

// buscarRepuestos: activos cuyo codigo o nombre contienen "texto". Hasta MAX_RESULTADOS.
Resultado buscarRepuestos(const char* texto, ListaRepuestos* resultado);

// listarRepuestos: todos los activos. Hasta MAX_RESULTADOS.
Resultado listarRepuestos(ListaRepuestos* resultado);

// registrarEntradaMercancia: suma "cantidad" al stock (llego mercancia del proveedor).
// Devuelve: OK; ERROR_DATO_INVALIDO si cantidad <= 0; ERROR_NO_EXISTE,
// ERROR_ELIMINADO o ERROR_ARCHIVO.
Resultado registrarEntradaMercancia(int idRepuesto, int cantidad);

// cambiarPrecioRepuesto: cambia el precio actual. Las ordenes ya hechas no
// cambian porque cada linea guarda su propio precioUnitario.
// Devuelve: OK; ERROR_DATO_INVALIDO si el precio <= 0; ERROR_NO_EXISTE,
// ERROR_ELIMINADO o ERROR_ARCHIVO.
Resultado cambiarPrecioRepuesto(int idRepuesto, float nuevoPrecio);

// eliminarRepuesto: borrado logico, solo si no aparece en ninguna orden.
// Devuelve: OK; ERROR_EN_USO; ERROR_NO_EXISTE, ERROR_ELIMINADO o ERROR_ARCHIVO.
Resultado eliminarRepuesto(int idRepuesto);

// tieneBajoStock: true si stock <= stockMinimo. Recibe el repuesto ya leido;
// devuelve bool porque solo compara dos numeros y no puede fallar.
bool tieneBajoStock(const Repuesto* repuesto);

#endif
