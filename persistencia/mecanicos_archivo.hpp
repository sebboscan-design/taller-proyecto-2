#ifndef MECANICOS_ARCHIVO_HPP
#define MECANICOS_ARCHIVO_HPP

// mecanicos_archivo.hpp
// Persistencia de mecanicos: las cinco operaciones que pide el enunciado, mas
// el total para poder recorrer los IDs. Trabaja sobre datos/mecanicos.bin.
// Son iguales en las cuatro entidades; solo cambian el tipo y la ruta.

#include "archivos.hpp"

// guardarMecanicoNuevo: agrega un mecanico al final del archivo.
// Pasos: lee el header, le pone el ID (proximoID), eliminado = false y las dos
// fechas, escribe el registro en su posicion y despues actualiza el header
// (cantidadRegistros++, registrosActivos++, proximoID++).
// Recibe un puntero para que quien llama vea el ID asignado en mecanico->id.
// Devuelve true si pudo.
bool guardarMecanicoNuevo(Mecanico* mecanico);

// leerMecanico: lee por calculo de posicion (seekg), sin recorrer el archivo.
// Lo entrega aunque este eliminado; eso lo decide la logica.
// Devuelve ARCHIVO_OK, ARCHIVO_ID_NO_EXISTE o ARCHIVO_ERROR.
ResultadoArchivo leerMecanico(int id, Mecanico* destino);

// sobrescribirMecanico: escribe el registro en su misma posicion (segun mecanico->id) y
// actualiza su fechaUltimaModificacion. Devuelve true si pudo.
bool sobrescribirMecanico(Mecanico* mecanico);

// marcarMecanicoEliminado: borrado logico. Pone eliminado = true, sobrescribe el
// registro y descuenta registrosActivos en el header. Si ya estaba eliminado no
// descuenta otra vez. Devuelve true si pudo.
bool marcarMecanicoEliminado(int id);

// contarMecanicosActivos: registros activos segun el header, sin recorrer. -1 si falla.
int contarMecanicosActivos();

// totalMecanicos: cantidadRegistros del header (incluye eliminados), para recorrer
// los IDs del 1 al total. -1 si falla.
int totalMecanicos();

#endif
