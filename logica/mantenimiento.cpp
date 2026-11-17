#include "mantenimiento.hpp"
#include "utilidades.hpp"
#include "../estructuras.hpp"
#include "../persistencia/archivos.hpp"
#include <cstdio>
#include <cstring>
#include <ctime>

static void anotarProblema(ReporteIntegridad* reporte, const char* mensaje) {
    if (reporte->problemas < MAX_PROBLEMAS) {
        copiarTexto(reporte->mensajes[reporte->problemas], mensaje, LARGO_PROBLEMA);
    }
    reporte->problemas++;
}

// Compara el header con lo que de verdad tiene el archivo.
// Devuelve cuantos registros reales hay (o -1 si no se pudo leer el archivo)
static int revisarHeader(const char* ruta, const char* nombre, int tamano, ReporteIntegridad* reporte,
                         int activosContados) {
    ArchivoHeader header;
    char mensaje[LARGO_PROBLEMA];
    if (!leerHeader(ruta, &header)) {
        snprintf(mensaje, LARGO_PROBLEMA, "ROTO: No se pudo leer el header de %s", nombre);
        anotarProblema(reporte, mensaje);
        return -1;
    }
    int reales = registrosRealesEnArchivo(ruta, tamano);
    if (header.cantidadRegistros != reales) {
        snprintf(mensaje, LARGO_PROBLEMA, "ROTO: Header de %s indica %d registros; el archivo contiene %d",
                 nombre, header.cantidadRegistros, reales);
        anotarProblema(reporte, mensaje);
    }
    if (activosContados >= 0 && header.registrosActivos != activosContados) {
        snprintf(mensaje, LARGO_PROBLEMA, "ROTO: Header de %s indica %d activos; se contaron %d",
                 nombre, header.registrosActivos, activosContados);
        anotarProblema(reporte, mensaje);
    }
    return reales;
}

Resultado verificarIntegridad(ReporteIntegridad* reporte) {
    memset(reporte, 0, sizeof(ReporteIntegridad));
    char mensaje[LARGO_PROBLEMA];

    // cuantos registros hay de verdad en cada archivo (sin creerle al header)
    int vehiculosReales = registrosRealesEnArchivo(RUTA_VEHICULOS, sizeof(Vehiculo));
    int mecanicosReales = registrosRealesEnArchivo(RUTA_MECANICOS, sizeof(Mecanico));
    int repuestosReales = registrosRealesEnArchivo(RUTA_REPUESTOS, sizeof(Repuesto));
    int ordenesReales = registrosRealesEnArchivo(RUTA_ORDENES, sizeof(OrdenServicio));
    if (vehiculosReales < 0 || mecanicosReales < 0 || repuestosReales < 0 || ordenesReales < 0) {
        return ERROR_ARCHIVO;
    }

    // vehiculos: contar activos
    int activos = 0;
    for (int i = 0; i < vehiculosReales; i++) {
        Vehiculo v;
        if (!leerRegistroCrudo(RUTA_VEHICULOS, i, &v, sizeof(Vehiculo))) return ERROR_ARCHIVO;
        if (!v.eliminado) activos++;
    }
    revisarHeader(RUTA_VEHICULOS, "vehiculos.bin", sizeof(Vehiculo), reporte, activos);
    reporte->vehiculosVerificados = vehiculosReales;

    // mecanicos: contar activos
    activos = 0;
    for (int i = 0; i < mecanicosReales; i++) {
        Mecanico m;
        if (!leerRegistroCrudo(RUTA_MECANICOS, i, &m, sizeof(Mecanico))) return ERROR_ARCHIVO;
        if (!m.eliminado) activos++;
    }
    revisarHeader(RUTA_MECANICOS, "mecanicos.bin", sizeof(Mecanico), reporte, activos);
    reporte->mecanicosVerificados = mecanicosReales;

    // repuestos: activos y stock negativo
    activos = 0;
    for (int i = 0; i < repuestosReales; i++) {
        Repuesto r;
        if (!leerRegistroCrudo(RUTA_REPUESTOS, i, &r, sizeof(Repuesto))) return ERROR_ARCHIVO;
        if (!r.eliminado) activos++;
        if (r.stock < 0) {
            snprintf(mensaje, LARGO_PROBLEMA, "ROTO: Repuesto #%d (%s) tiene stock negativo: %d",
                     r.id, r.codigo, r.stock);
            anotarProblema(reporte, mensaje);
        }
    }
    revisarHeader(RUTA_REPUESTOS, "repuestos.bin", sizeof(Repuesto), reporte, activos);
    reporte->repuestosVerificados = repuestosReales;

    // ordenes: referencias a vehiculos, mecanicos y repuestos
    activos = 0;
    for (int i = 0; i < ordenesReales; i++) {
        OrdenServicio o;
        if (!leerRegistroCrudo(RUTA_ORDENES, i, &o, sizeof(OrdenServicio))) return ERROR_ARCHIVO;
        if (!o.eliminado) activos++;
        bool abierta = strcmp(o.estado, "ABIERTA") == 0;

        // a) y b) vehiculo
        if (o.idVehiculo < 1 || o.idVehiculo > vehiculosReales) {
            snprintf(mensaje, LARGO_PROBLEMA, "ROTO: Orden #%d apunta al vehiculo ID %d (no existe en vehiculos.bin)",
                     o.id, o.idVehiculo);
            anotarProblema(reporte, mensaje);
        } else if (abierta) {
            Vehiculo v;
            if (!leerRegistroCrudo(RUTA_VEHICULOS, o.idVehiculo - 1, &v, sizeof(Vehiculo))) return ERROR_ARCHIVO;
            if (v.eliminado) {
                snprintf(mensaje, LARGO_PROBLEMA, "ROTO: Orden #%d esta ABIERTA y su vehiculo %s esta eliminado",
                         o.id, v.placa);
                anotarProblema(reporte, mensaje);
            }
        }

        // a) y b) mecanico (en cerradas/anuladas un mecanico de baja es valido)
        if (o.idMecanico < 1 || o.idMecanico > mecanicosReales) {
            snprintf(mensaje, LARGO_PROBLEMA, "ROTO: Orden #%d apunta al mecanico ID %d (no existe en mecanicos.bin)",
                     o.id, o.idMecanico);
            anotarProblema(reporte, mensaje);
        } else if (abierta) {
            Mecanico m;
            if (!leerRegistroCrudo(RUTA_MECANICOS, o.idMecanico - 1, &m, sizeof(Mecanico))) return ERROR_ARCHIVO;
            if (m.eliminado) {
                snprintf(mensaje, LARGO_PROBLEMA, "ROTO: Orden #%d esta ABIERTA y su mecanico %s esta dado de baja",
                         o.id, m.nombre);
                anotarProblema(reporte, mensaje);
            }
        }

        // c) lineas del detalle
        if (o.numLineas < 0 || o.numLineas > MAX_LINEAS_ORDEN) {
            snprintf(mensaje, LARGO_PROBLEMA, "ROTO: Orden #%d dice tener %d lineas (maximo %d)",
                     o.id, o.numLineas, MAX_LINEAS_ORDEN);
            anotarProblema(reporte, mensaje);
            continue;
        }
        for (int j = 0; j < o.numLineas; j++) {
            int idRepuesto = o.lineas[j].idRepuesto;
            if (idRepuesto < 1 || idRepuesto > repuestosReales) {
                snprintf(mensaje, LARGO_PROBLEMA, "ROTO: Orden #%d, linea %d apunta al repuesto ID %d (no existe en repuestos.bin)",
                         o.id, j + 1, idRepuesto);
                anotarProblema(reporte, mensaje);
            }
        }
    }
    revisarHeader(RUTA_ORDENES, "ordenes.bin", sizeof(OrdenServicio), reporte, activos);
    reporte->ordenesVerificadas = ordenesReales;

    return OK;
}

Resultado crearBackup(char* rutaCreada, int tamanoRuta) {
    time_t ahora = time(nullptr);
    tm* fecha = localtime(&ahora);
    char carpeta[60];
    snprintf(carpeta, sizeof(carpeta), "backup_%04d-%02d-%02d_%02d-%02d",
             fecha->tm_year + 1900, fecha->tm_mon + 1, fecha->tm_mday, fecha->tm_hour, fecha->tm_min);

    if (!crearCarpeta(carpeta)) return ERROR_ARCHIVO;

    const char* origenes[] = {RUTA_TALLER, RUTA_VEHICULOS, RUTA_MECANICOS, RUTA_REPUESTOS, RUTA_ORDENES};
    const char* nombres[] = {"taller.bin", "vehiculos.bin", "mecanicos.bin", "repuestos.bin", "ordenes.bin"};
    for (int i = 0; i < 5; i++) {
        char destino[120];
        snprintf(destino, sizeof(destino), "%s/%s", carpeta, nombres[i]);
        if (!copiarArchivo(origenes[i], destino)) return ERROR_ARCHIVO;
    }

    copiarTexto(rutaCreada, carpeta, tamanoRuta);
    return OK;
}
