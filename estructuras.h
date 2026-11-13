#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <ctime>

// Todo lo que se guarda en un .bin tiene que tener tamano fijo:
// nada de punteros, std::string ni arreglos dinamicos

// Va al inicio de cada archivo de entidades
struct ArchivoHeader {
    int cantidadRegistros; // total historico (incluye eliminados)
    int proximoID;         // siguiente ID a asignar
    int registrosActivos;  // registros con eliminado == false
    int version;           // version del formato (empieza en 1)
};

// taller.bin tiene un solo registro de estos y no lleva header
struct Taller {
    char nombre[100];
    char rif[20];
    char direccion[200];
    char telefono[20];
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct Vehiculo {
    int id;
    char placa[10];
    char marca[50];
    char modelo[50];
    int anio;
    int kilometraje;
    char nombrePropietario[100];
    char cedulaPropietario[20];
    char telefonoPropietario[20];
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct Mecanico {
    int id;
    char nombre[100];
    char cedula[20];
    char especialidad[20];
    char fechaIngreso[11];
    bool eliminado; // true = dado de baja
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct Repuesto {
    int id;
    char codigo[20];
    char nombre[100];
    char marca[50];
    float precio;
    int stock;
    int stockMinimo;
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

// Diseno del detalle (seccion 3): cada orden trae adentro un arreglo fijo
// de lineas. Asi todas las ordenes miden lo mismo en disco y con un solo
// read se tiene la orden completa con sus lineas
const int MAX_LINEAS_ORDEN = 15;

struct DetalleOrden {
    int idRepuesto;
    int cantidad;
    float precioUnitario; // precio del repuesto cuando se agrego (historico)
};

struct OrdenServicio {
    int id;
    int idVehiculo;
    int idMecanico;
    char fechaApertura[11];
    char fechaCierre[11];
    char estado[12]; // "ABIERTA", "CERRADA" o "ANULADA"
    char descripcionFalla[200];
    int kilometrajeIngreso;
    float manoObra;
    DetalleOrden lineas[MAX_LINEAS_ORDEN]; // solo las primeras numLineas valen
    int numLineas;
    bool eliminado; // las ordenes no se eliminan: siempre false
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

#endif
