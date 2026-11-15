#ifndef ESTRUCTURAS_HPP
#define ESTRUCTURAS_HPP

// estructuras.hpp
// Todos los structs que se guardan en los archivos .bin de la carpeta datos/.
//
// Regla de este archivo: todo lo que va a disco tiene que medir siempre lo
// mismo. Por eso aqui no hay punteros, ni std::string, ni arreglos dinamicos:
// solo numeros, bool, time_t y arreglos de tamano fijo (char[], DetalleOrden[]).
// Un puntero guardado en un archivo solo guarda una direccion de memoria, y esa
// direccion ya no sirve cuando el programa se vuelve a abrir.
//
// Tamanos medidos con sizeof en 64 bits (time_t de 8 bytes):
//   ArchivoHeader 16, Taller 360, Vehiculo 288, Mecanico 176,
//   Repuesto 208, DetalleOrden 12, OrdenServicio 464

#include <ctime>

// ArchivoHeader
// Va en los primeros 16 bytes de vehiculos.bin, mecanicos.bin, repuestos.bin
// y ordenes.bin. Los registros empiezan justo despues.
struct ArchivoHeader {
    int cantidadRegistros; // registros escritos en total (incluye los eliminados)
    int proximoID;         // ID que le toca al proximo registro nuevo
    int registrosActivos;  // registros con eliminado == false
    int version;           // version del formato del archivo (empieza en 1)
};

// Taller
// Datos generales del taller. taller.bin tiene un solo registro de estos y
// no lleva header. Se crea la primera vez que se corre el programa.
struct Taller {
    char nombre[100];
    char rif[20];
    char direccion[200];
    char telefono[20];
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

// Vehiculo
// Un carro con los datos de su dueno. La placa es unica entre los activos.
struct Vehiculo {
    int id;                      // tambien dice su posicion en el archivo (id - 1)
    char placa[10];              // se guarda en mayusculas
    char marca[50];
    char modelo[50];
    int anio;                    // 1950 a 2027
    int kilometraje;             // ultimo km registrado; solo puede subir
    char nombrePropietario[100];
    char cedulaPropietario[20];
    char telefonoPropietario[20];
    bool eliminado;              // borrado logico: el registro se queda en el archivo
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

// Mecanico
// eliminado == true significa "dado de baja": no recibe ordenes nuevas, pero
// sus ordenes viejas lo siguen mostrando.
struct Mecanico {
    int id;
    char nombre[100];
    char cedula[20];
    char especialidad[20];  // MOTOR, FRENOS, ELECTRICIDAD, SUSPENSION o GENERAL
    char fechaIngreso[11];  // AAAA-MM-DD
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

// Repuesto
// Un articulo del inventario. El codigo es unico entre los activos.
struct Repuesto {
    int id;
    char codigo[20];   // se guarda en mayusculas
    char nombre[100];
    char marca[50];
    float precio;      // precio actual; las ordenes guardan su propia copia
    int stock;
    int stockMinimo;   // si stock <= stockMinimo se marca BAJO STOCK
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

// Maximo de repuestos distintos por orden (el enunciado pide al menos 15)
const int MAX_LINEAS_ORDEN = 15;

// DetalleOrden
// Una linea de una orden: que repuesto, cuantas unidades y a que precio.
struct DetalleOrden {
    int idRepuesto;
    int cantidad;
    float precioUnitario; // copia del precio del repuesto cuando se agrego
};

// OrdenServicio
// Diseno del detalle: la orden trae adentro un arreglo fijo de 15 lineas y un
// contador numLineas. Asi todas las ordenes miden 464 bytes, se ubican por
// calculo y con un solo read se tiene la orden completa con sus lineas.
// Solo valen las primeras numLineas posiciones de lineas[].
struct OrdenServicio {
    int id;
    int idVehiculo;            // se relacionan por ID, nunca por puntero
    int idMecanico;
    char fechaApertura[11];    // AAAA-MM-DD
    char fechaCierre[11];      // vacia mientras la orden este ABIERTA
    char estado[12];           // "ABIERTA", "CERRADA" o "ANULADA"
    char descripcionFalla[200];
    int kilometrajeIngreso;
    float manoObra;
    DetalleOrden lineas[MAX_LINEAS_ORDEN];
    int numLineas;             // de 0 a MAX_LINEAS_ORDEN
    bool eliminado;            // las ordenes no se eliminan, se anulan: siempre false
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

#endif
