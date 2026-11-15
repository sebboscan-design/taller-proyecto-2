#ifndef VEHICULOS_ARCHIVO_HPP
#define VEHICULOS_ARCHIVO_HPP

// vehiculos_archivo.hpp
// Persistencia de vehiculos: las cinco operaciones que pide el enunciado, mas
// el total para poder recorrer los IDs. Trabaja sobre datos/vehiculos.bin.
// Son iguales en las cuatro entidades; solo cambian el tipo y la ruta.

#include "archivos.hpp"

// guardarVehiculoNuevo: agrega un vehiculo al final del archivo.
// Pasos: lee el header, le pone el ID (proximoID), eliminado = false y las dos
// fechas, escribe el registro en su posicion y despues actualiza el header
// (cantidadRegistros++, registrosActivos++, proximoID++).
// Recibe un puntero para que quien llama vea el ID asignado en vehiculo->id.
// Devuelve true si pudo.
bool guardarVehiculoNuevo(Vehiculo* vehiculo);

// leerVehiculo: lee por calculo de posicion (seekg), sin recorrer el archivo.
// Lo entrega aunque este eliminado; eso lo decide la logica.
// Devuelve ARCHIVO_OK, ARCHIVO_ID_NO_EXISTE o ARCHIVO_ERROR.
ResultadoArchivo leerVehiculo(int id, Vehiculo* destino);

// sobrescribirVehiculo: escribe el registro en su misma posicion (segun vehiculo->id) y
// actualiza su fechaUltimaModificacion. Devuelve true si pudo.
bool sobrescribirVehiculo(Vehiculo* vehiculo);

// marcarVehiculoEliminado: borrado logico. Pone eliminado = true, sobrescribe el
// registro y descuenta registrosActivos en el header. Si ya estaba eliminado no
// descuenta otra vez. Devuelve true si pudo.
bool marcarVehiculoEliminado(int id);

// contarVehiculosActivos: registros activos segun el header, sin recorrer. -1 si falla.
int contarVehiculosActivos();

// totalVehiculos: cantidadRegistros del header (incluye eliminados), para recorrer
// los IDs del 1 al total. -1 si falla.
int totalVehiculos();

#endif
