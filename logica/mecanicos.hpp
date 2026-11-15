#ifndef MECANICOS_HPP
#define MECANICOS_HPP

// mecanicos.hpp
// Reglas de los mecanicos. Lee y escribe a traves de persistencia/mecanicos_archivo.
// "Eliminado" en un mecanico significa "dado de baja".

#include "../estructuras.hpp"
#include "resultados.hpp"
#include "listas.hpp"

// registrarMecanico: valida y guarda un mecanico nuevo.
// Reglas: nombre no vacio, cedula valida y unica (entre todos, incluso los de
// baja), especialidad permitida (se pasa a mayusculas) y fecha AAAA-MM-DD real.
// Devuelve: OK y el ID en *idNuevo; ERROR_DUPLICADO, ERROR_DATO_INVALIDO o ERROR_ARCHIVO.
Resultado registrarMecanico(Mecanico datos, int* idNuevo);

// obtenerMecanicoPorID: lee el mecanico por calculo de posicion.
// Devuelve: OK; ERROR_ELIMINADO si esta de baja (igual llena destino, asi las
// ordenes viejas pueden mostrar su nombre); ERROR_NO_EXISTE o ERROR_ARCHIVO.
Resultado obtenerMecanicoPorID(int id, Mecanico* destino);

// listarMecanicos: todos, activos y de baja, y al lado de cada uno cuantas
// ordenes ABIERTAS tiene (en resultado->ordenesAbiertas). Hasta MAX_RESULTADOS.
Resultado listarMecanicos(ListaMecanicos* resultado);

// darDeBajaMecanico: borrado logico del mecanico, solo si no tiene ordenes ABIERTAS.
// Sus ordenes cerradas y anuladas no se tocan.
// Devuelve: OK; ERROR_TIENE_ORDENES y cuantas abiertas en *ordenesAbiertas;
// ERROR_ELIMINADO si ya estaba de baja; ERROR_NO_EXISTE o ERROR_ARCHIVO.
Resultado darDeBajaMecanico(int id, int* ordenesAbiertas);

#endif
