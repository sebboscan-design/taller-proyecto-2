#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include "../estructuras.h"

// Capa de persistencia: solo lee y escribe bytes en los .bin
// No usa cin ni cout y no aplica reglas de negocio (por ejemplo no filtra
// eliminados, eso lo decide la logica)

const char RUTA_DATOS[] = "datos";
const char RUTA_TALLER[] = "datos/taller.bin";
const char RUTA_VEHICULOS[] = "datos/vehiculos.bin";
const char RUTA_MECANICOS[] = "datos/mecanicos.bin";
const char RUTA_REPUESTOS[] = "datos/repuestos.bin";
const char RUTA_ORDENES[] = "datos/ordenes.bin";

// Lo que devuelve una lectura por ID. Se separa "no existe" de "fallo el
// archivo" porque la presentacion tiene que mostrar mensajes distintos
enum ResultadoArchivo {
    ARCHIVO_OK,
    ARCHIVO_ID_NO_EXISTE,
    ARCHIVO_ERROR // no se pudo abrir o leer el archivo
};

// Ruta del ultimo archivo que no se pudo abrir (para que el mensaje diga cual fue)
void anotarErrorArchivo(const char* ruta);
const char* ultimoArchivoConError();

// Carpeta datos/: la crea si no existe. true si quedo lista
bool prepararCarpetaDatos();

// Header (sirve para cualquier archivo de entidades)
// Si el archivo no existe lo crea con el header inicial; si ya existe no lo toca.
// true si el archivo quedo listo para usarse
bool inicializarArchivo(const char* ruta);
bool leerHeader(const char* ruta, ArchivoHeader* destino);
bool actualizarHeader(const char* ruta, ArchivoHeader header); // solo escribe en la posicion 0

// Byte donde empieza el registro con ese ID.
// Como los registros nunca se mueven y los IDs empiezan en 1, el indice fisico es id - 1
long posicionRegistro(int id, int tamanoRegistro);

// Cuantos registros caben realmente en el archivo segun su tamano en bytes
// (para la verificacion de integridad). -1 si no se pudo abrir
int registrosRealesEnArchivo(const char* ruta, int tamanoRegistro);

// Lee el registro numero "indice" (0, 1, 2...) sin mirar el header.
// Solo lo usa la verificacion de integridad, que no puede confiar en el header
bool leerRegistroCrudo(const char* ruta, int indice, void* destino, int tamanoRegistro);

// Crea una carpeta (para el backup). true si quedo lista
bool crearCarpeta(const char* ruta);

// Copia un archivo byte a byte (para el backup)
bool copiarArchivo(const char* origen, const char* destino);

// taller.bin: un solo registro, sin header
bool existeTaller();
bool leerTaller(Taller* destino);
bool guardarTaller(Taller* taller); // pone las fechas de creacion/modificacion

#endif
