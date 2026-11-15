#ifndef REPUESTOS_ARCHIVO_HPP
#define REPUESTOS_ARCHIVO_HPP

// repuestos_archivo.hpp
// Persistencia de repuestos: las cinco operaciones que pide el enunciado, mas
// el total para poder recorrer los IDs. Trabaja sobre datos/repuestos.bin.
// Son iguales en las cuatro entidades; solo cambian el tipo y la ruta.

#include "archivos.hpp"

// guardarRepuestoNuevo: agrega un repuesto al final del archivo.
// Pasos: lee el header, le pone el ID (proximoID), eliminado = false y las dos
// fechas, escribe el registro en su posicion y despues actualiza el header
// (cantidadRegistros++, registrosActivos++, proximoID++).
// Recibe un puntero para que quien llama vea el ID asignado en repuesto->id.
// Devuelve true si pudo.
bool guardarRepuestoNuevo(Repuesto* repuesto);

// leerRepuesto: lee por calculo de posicion (seekg), sin recorrer el archivo.
// Lo entrega aunque este eliminado; eso lo decide la logica.
// Devuelve ARCHIVO_OK, ARCHIVO_ID_NO_EXISTE o ARCHIVO_ERROR.
ResultadoArchivo leerRepuesto(int id, Repuesto* destino);

// sobrescribirRepuesto: escribe el registro en su misma posicion (segun repuesto->id) y
// actualiza su fechaUltimaModificacion. Devuelve true si pudo.
bool sobrescribirRepuesto(Repuesto* repuesto);

// marcarRepuestoEliminado: borrado logico. Pone eliminado = true, sobrescribe el
// registro y descuenta registrosActivos en el header. Si ya estaba eliminado no
// descuenta otra vez. Devuelve true si pudo.
bool marcarRepuestoEliminado(int id);

// contarRepuestosActivos: registros activos segun el header, sin recorrer. -1 si falla.
int contarRepuestosActivos();

// totalRepuestos: cantidadRegistros del header (incluye eliminados), para recorrer
// los IDs del 1 al total. -1 si falla.
int totalRepuestos();

#endif
