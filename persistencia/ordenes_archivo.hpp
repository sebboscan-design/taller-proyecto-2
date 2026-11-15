#ifndef ORDENES_ARCHIVO_HPP
#define ORDENES_ARCHIVO_HPP

// ordenes_archivo.hpp
// Persistencia de ordenes: las cinco operaciones que pide el enunciado, mas
// el total para poder recorrer los IDs. Trabaja sobre datos/ordenes.bin.
// Son iguales en las cuatro entidades; solo cambian el tipo y la ruta.

#include "archivos.hpp"

// guardarOrdenNueva: agrega una orden al final del archivo.
// Pasos: lee el header, le pone el ID (proximoID), eliminado = false y las dos
// fechas, escribe el registro en su posicion y despues actualiza el header
// (cantidadRegistros++, registrosActivos++, proximoID++).
// Recibe un puntero para que quien llama vea el ID asignado en orden->id.
// Devuelve true si pudo.
bool guardarOrdenNueva(OrdenServicio* orden);

// leerOrden: lee por calculo de posicion (seekg), sin recorrer el archivo.
// Lo entrega aunque este eliminado; eso lo decide la logica.
// Devuelve ARCHIVO_OK, ARCHIVO_ID_NO_EXISTE o ARCHIVO_ERROR.
ResultadoArchivo leerOrden(int id, OrdenServicio* destino);

// sobrescribirOrden: escribe el registro en su misma posicion (segun orden->id) y
// actualiza su fechaUltimaModificacion. Devuelve true si pudo.
bool sobrescribirOrden(OrdenServicio* orden);

// marcarOrdenEliminada: borrado logico. Pone eliminado = true, sobrescribe el
// registro y descuenta registrosActivos en el header. Si ya estaba eliminado no
// descuenta otra vez. Devuelve true si pudo.
bool marcarOrdenEliminada(int id);

// contarOrdenesActivas: registros activos segun el header, sin recorrer. -1 si falla.
int contarOrdenesActivas();

// totalOrdenes: cantidadRegistros del header (incluye eliminados), para recorrer
// los IDs del 1 al total. -1 si falla.
int totalOrdenes();

#endif
