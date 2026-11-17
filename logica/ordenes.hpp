#ifndef ORDENES_HPP
#define ORDENES_HPP

// ordenes.hpp
// Reglas de las ordenes de servicio. Es el modulo que mas archivos toca a la vez.
//
// Las operaciones que escriben en dos archivos (abrir, agregar, quitar, anular)
// validan TODO antes de la primera escritura: si algo falla no se escribe nada.
// El orden de las escrituras esta pensado para que, si el programa se corta
// entre una y otra, el stock nunca quede mas alto de lo real (ver README_V2).

#include "../estructuras.hpp"
#include "resultados.hpp"
#include "listas.hpp"

// Ordenes ABIERTAS que puede tener un mecanico al mismo tiempo
const int MAX_ORDENES_ABIERTAS_MECANICO = 3;

// abrirOrden: crea una orden ABIERTA para un vehiculo con un mecanico.
// Reglas: vehiculo activo y sin otra orden ABIERTA; mecanico que exista, este
// activo y tenga menos de 3 ABIERTAS; km de ingreso >= km del vehiculo; falla no vacia.
// Escribe: 1) la orden en ordenes.bin  2) el km nuevo en vehiculos.bin
// Devuelve: OK y el numero en *idNuevaOrden; o ERROR_VEHICULO_NO_DISPONIBLE,
// ERROR_MECANICO_NO_EXISTE, ERROR_MECANICO_DE_BAJA, ERROR_VEHICULO_CON_ORDEN_ABIERTA,
// ERROR_MECANICO_SIN_CUPO, ERROR_KILOMETRAJE_MENOR, ERROR_DATO_INVALIDO o ERROR_ARCHIVO.
Resultado abrirOrden(int idVehiculo, int idMecanico, int kilometrajeIngreso,
                     const char* descripcionFalla, int* idNuevaOrden);

// agregarRepuestoAOrden: agrega "cantidad" unidades de un repuesto a una orden ABIERTA.
// Si el repuesto ya estaba, suma a su linea (y conserva el precio que tenia).
// Si no estaba, crea una linea con el precio actual del repuesto.
// Escribe: 1) el stock (baja) en repuestos.bin  2) la orden con la linea
// Devuelve: OK; ERROR_DATO_INVALIDO (cantidad <= 0), ERROR_NO_EXISTE,
// ERROR_ESTADO_INVALIDO, ERROR_ELIMINADO (repuesto), ERROR_STOCK_INSUFICIENTE,
// ERROR_LIMITE_LINEAS o ERROR_ARCHIVO.
Resultado agregarRepuestoAOrden(int idOrden, int idRepuesto, int cantidad);

// quitarRepuestoDeOrden: quita la linea completa de ese repuesto de una orden
// ABIERTA (corriendo las de despues) y devuelve sus unidades al stock.
// Escribe: 1) la orden sin la linea  2) el stock (sube) en repuestos.bin
// Devuelve: OK; ERROR_NO_EXISTE, ERROR_ESTADO_INVALIDO, ERROR_REPUESTO_NO_ESTA o ERROR_ARCHIVO.
Resultado quitarRepuestoDeOrden(int idOrden, int idRepuesto);

// cerrarOrden: pasa una orden ABIERTA a CERRADA, con su mano de obra y fecha de cierre.
// Devuelve: OK; ERROR_DATO_INVALIDO (mano de obra < 0); ERROR_ORDEN_VACIA (sin
// lineas y mano de obra 0); ERROR_NO_EXISTE, ERROR_ESTADO_INVALIDO o ERROR_ARCHIVO.
Resultado cerrarOrden(int idOrden, float manoObra);

// anularOrden: pasa una orden ABIERTA o CERRADA a ANULADA y devuelve al
// inventario las unidades de todas sus lineas. La orden no se borra.
// Escribe: 1) la orden como ANULADA  2) el stock de cada linea
// Devuelve: OK; ERROR_ESTADO_INVALIDO si ya estaba ANULADA; ERROR_NO_EXISTE o ERROR_ARCHIVO.
Resultado anularOrden(int idOrden);

// calcularSubtotalRepuestos: suma cantidad * precioUnitario de cada linea.
float calcularSubtotalRepuestos(const OrdenServicio* orden);

// calcularTotalOrden: subtotal de repuestos + mano de obra. El total nunca se
// guarda: se calcula siempre con los precios historicos de las lineas.
float calcularTotalOrden(const OrdenServicio* orden);

// obtenerOrdenPorID: lee la orden por calculo de posicion (con todas sus lineas).
// Devuelve: OK, ERROR_NO_EXISTE o ERROR_ARCHIVO.
Resultado obtenerOrdenPorID(int id, OrdenServicio* destino);

// listarOrdenes / listarOrdenesPorEstado / listarOrdenesPorVehiculo:
// llenan la lista con las ordenes que cumplan el filtro, hasta MAX_RESULTADOS.
// estado es "ABIERTA", "CERRADA" o "ANULADA".
Resultado listarOrdenes(ListaOrdenes* resultado);
Resultado listarOrdenesPorEstado(const char* estado, ListaOrdenes* resultado);
Resultado listarOrdenesPorVehiculo(int idVehiculo, ListaOrdenes* resultado);

// Conteos que usan las reglas de los otros modulos. Devuelven -1 si falla el archivo.
int contarOrdenesDeVehiculo(int idVehiculo);       // en cualquier estado
int contarOrdenesAbiertasMecanico(int idMecanico); // solo ABIERTAS
int contarOrdenesConRepuesto(int idRepuesto);      // ordenes que tienen ese repuesto en una linea

// buscarOrdenAbiertaDeVehiculo: OK y el numero en *idOrden si el vehiculo tiene
// una orden ABIERTA; ERROR_NO_EXISTE si no tiene; ERROR_ARCHIVO si fallo.
Resultado buscarOrdenAbiertaDeVehiculo(int idVehiculo, int* idOrden);

#endif
