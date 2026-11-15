#ifndef ARCHIVOS_HPP
#define ARCHIVOS_HPP

// archivos.hpp
// Capa de persistencia, parte general: rutas, header, calculo de posicion,
// taller.bin y herramientas para integridad y backup.
//
// Esta capa solo lee y escribe bytes. No usa cin ni cout y no aplica reglas
// de negocio (por ejemplo, no decide si un registro eliminado sirve o no:
// lo entrega igual y eso lo decide la logica).

#include "../estructuras.hpp"

// Rutas de los archivos (relativas a la carpeta desde donde se corre el programa)
const char RUTA_DATOS[] = "datos";
const char RUTA_TALLER[] = "datos/taller.bin";
const char RUTA_VEHICULOS[] = "datos/vehiculos.bin";
const char RUTA_MECANICOS[] = "datos/mecanicos.bin";
const char RUTA_REPUESTOS[] = "datos/repuestos.bin";
const char RUTA_ORDENES[] = "datos/ordenes.bin";

// Lo que devuelve una lectura por ID. Separa "ese ID no existe" de "el archivo
// fallo", porque el menu tiene que mostrar mensajes distintos.
enum ResultadoArchivo {
    ARCHIVO_OK,
    ARCHIVO_ID_NO_EXISTE, // el ID es menor que 1 o mayor que cantidadRegistros
    ARCHIVO_ERROR         // no se pudo abrir o leer el archivo
};

// anotarErrorArchivo: guarda la ruta del archivo que no se pudo abrir.
// La llaman todas las funciones de esta capa cuando falla un open.
void anotarErrorArchivo(const char* ruta);

// ultimoArchivoConError: la ultima ruta anotada (para el mensaje de error).
const char* ultimoArchivoConError();

// prepararCarpetaDatos: crea la carpeta datos/ si no existe. true si quedo lista.
bool prepararCarpetaDatos();

// inicializarArchivo: si el archivo no existe lo crea con el header inicial
// (0 registros, proximoID 1, 0 activos, version 1). Si ya existe no lo toca,
// para no borrar lo que habia. true si el archivo quedo listo para usarse.
bool inicializarArchivo(const char* ruta);

// leerHeader: lee los primeros 16 bytes del archivo en destino. true si pudo.
bool leerHeader(const char* ruta, ArchivoHeader* destino);

// actualizarHeader: escribe el header en la posicion 0 sin tocar el resto del
// archivo (abre con in|out para no truncarlo). true si pudo.
bool actualizarHeader(const char* ruta, ArchivoHeader header);

// posicionRegistro: byte donde empieza el registro con ese ID.
// = sizeof(ArchivoHeader) + (id - 1) * tamanoRegistro
// Funciona porque los IDs empiezan en 1, se dan en orden y los registros nunca
// se mueven ni se borran fisicamente: el indice fisico siempre es id - 1.
long posicionRegistro(int id, int tamanoRegistro);

// registrosRealesEnArchivo: cuantos registros completos hay de verdad segun el
// tamano del archivo en bytes, sin creerle al header. -1 si no se pudo abrir.
int registrosRealesEnArchivo(const char* ruta, int tamanoRegistro);

// leerRegistroCrudo: lee el registro numero "indice" (0, 1, 2...) sin mirar el
// header. Solo la usa la verificacion de integridad. true si pudo leerlo completo.
bool leerRegistroCrudo(const char* ruta, int indice, void* destino, int tamanoRegistro);

// crearCarpeta: crea una carpeta (para el backup). true si quedo lista.
bool crearCarpeta(const char* ruta);

// copiarArchivo: copia un archivo byte a byte (para el backup). true si pudo.
bool copiarArchivo(const char* origen, const char* destino);

// existeTaller: true si ya existe taller.bin.
bool existeTaller();

// leerTaller: lee el unico registro de taller.bin. true si pudo.
bool leerTaller(Taller* destino);

// guardarTaller: crea taller.bin con ese taller y le pone las dos fechas. true si pudo.
bool guardarTaller(Taller* taller);

#endif
