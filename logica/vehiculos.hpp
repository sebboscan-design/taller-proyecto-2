#ifndef VEHICULOS_HPP
#define VEHICULOS_HPP

// vehiculos.hpp
// Reglas de los vehiculos. Lee y escribe a traves de persistencia/vehiculos_archivo.
//
// Ninguna funcion devuelve un puntero a un vehiculo: los datos viven en disco,
// asi que las que leen dejan una COPIA en "destino" y el Resultado dice si se pudo.

#include "../estructuras.hpp"
#include "resultados.hpp"
#include "listas.hpp"

// registrarVehiculo: valida los datos y guarda un vehiculo nuevo al final del archivo.
// Reglas: placa de 6-7 letras/numeros y unica entre los activos (la de un
// eliminado se puede reusar), anio 1950-2027, km >= 0, cedula valida y marca,
// modelo y propietario no vacios. La placa se guarda en mayusculas.
// Recibe: datos (por valor, la funcion trabaja sobre su copia) y idNuevo.
// Devuelve: OK y el ID asignado en *idNuevo; o ERROR_DUPLICADO,
// ERROR_DATO_INVALIDO o ERROR_ARCHIVO.
Resultado registrarVehiculo(Vehiculo datos, int* idNuevo);

// obtenerVehiculoPorID: lee el vehiculo por calculo de posicion, sin recorrer.
// Devuelve: OK; ERROR_ELIMINADO (igual llena destino, para poder mostrar su placa);
// ERROR_NO_EXISTE si nunca hubo un vehiculo con ese ID; o ERROR_ARCHIVO.
Resultado obtenerVehiculoPorID(int id, Vehiculo* destino);

// obtenerVehiculoPorPlaca: recorre el archivo (un vehiculo a la vez) buscando
// esa placa entre los activos. No importan las mayusculas.
// Devuelve: OK y la copia en destino; ERROR_NO_EXISTE; o ERROR_ARCHIVO.
Resultado obtenerVehiculoPorPlaca(const char* placa, Vehiculo* destino);

// posicionVehiculoEnArchivo: byte de vehiculos.bin donde empieza el vehiculo
// con ese ID (16 + (id - 1) * 288). Es para el mensaje de depuracion del caso 2.
long posicionVehiculoEnArchivo(int id);

// buscarVehiculosPorPropietario: los activos cuyo propietario contiene "texto"
// (parcial, sin importar mayusculas). Llena hasta MAX_RESULTADOS.
// Devuelve: OK (aunque no encuentre ninguno, entonces cantidad = 0) o ERROR_ARCHIVO.
Resultado buscarVehiculosPorPropietario(const char* texto, ListaVehiculos* resultado);

// listarVehiculos: todos los activos, hasta MAX_RESULTADOS.
Resultado listarVehiculos(ListaVehiculos* resultado);

// actualizarVehiculo: cambia marca, modelo, anio y datos del propietario.
// La placa, el kilometraje y el ID no se tocan (de "datos" se ignoran).
// Devuelve: OK, ERROR_NO_EXISTE, ERROR_ELIMINADO, ERROR_DATO_INVALIDO o ERROR_ARCHIVO.
Resultado actualizarVehiculo(int id, Vehiculo datos);

// eliminarVehiculo: borrado logico (eliminado = true), solo si no tiene
// ninguna orden en ningun estado.
// Devuelve: OK; ERROR_TIENE_ORDENES y cuantas en *ordenesEncontradas;
// ERROR_NO_EXISTE, ERROR_ELIMINADO o ERROR_ARCHIVO.
Resultado eliminarVehiculo(int id, int* ordenesEncontradas);

#endif
