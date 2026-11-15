#include "ordenes_archivo.hpp"
#include <fstream>

using namespace std;

bool guardarOrdenNueva(OrdenServicio* orden) {
    ArchivoHeader header;
    if (!leerHeader(RUTA_ORDENES, &header)) return false;

    time_t ahora = time(nullptr);
    orden->id = header.proximoID;
    orden->eliminado = false;
    orden->fechaCreacion = ahora;
    orden->fechaUltimaModificacion = ahora;

    fstream archivo(RUTA_ORDENES, ios::in | ios::out | ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(RUTA_ORDENES);
        return false;
    }

    // primero el registro y despues el header: si algo se corta en medio,
    // el header queda atrasado y la verificacion de integridad lo detecta
    archivo.seekp(posicionRegistro(orden->id, sizeof(OrdenServicio)));
    archivo.write((char*)orden, sizeof(OrdenServicio));

    header.cantidadRegistros++;
    header.registrosActivos++;
    header.proximoID++;
    archivo.seekp(0);
    archivo.write((char*)&header, sizeof(ArchivoHeader));

    bool bien = archivo.good();
    archivo.close();
    return bien;
}

ResultadoArchivo leerOrden(int id, OrdenServicio* destino) {
    ArchivoHeader header;
    if (!leerHeader(RUTA_ORDENES, &header)) return ARCHIVO_ERROR;
    if (id < 1 || id > header.cantidadRegistros) return ARCHIVO_ID_NO_EXISTE;

    ifstream archivo(RUTA_ORDENES, ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(RUTA_ORDENES);
        return ARCHIVO_ERROR;
    }

    archivo.seekg(posicionRegistro(id, sizeof(OrdenServicio)));
    archivo.read((char*)destino, sizeof(OrdenServicio));
    bool bien = archivo.gcount() == sizeof(OrdenServicio);
    archivo.close();
    return bien ? ARCHIVO_OK : ARCHIVO_ERROR;
}

bool sobrescribirOrden(OrdenServicio* orden) {
    ArchivoHeader header;
    if (!leerHeader(RUTA_ORDENES, &header)) return false;
    if (orden->id < 1 || orden->id > header.cantidadRegistros) return false;

    fstream archivo(RUTA_ORDENES, ios::in | ios::out | ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(RUTA_ORDENES);
        return false;
    }

    orden->fechaUltimaModificacion = time(nullptr);
    archivo.seekp(posicionRegistro(orden->id, sizeof(OrdenServicio)));
    archivo.write((char*)orden, sizeof(OrdenServicio));
    bool bien = archivo.good();
    archivo.close();
    return bien;
}

bool marcarOrdenEliminada(int id) {
    OrdenServicio registro;
    if (leerOrden(id, &registro) != ARCHIVO_OK) return false;
    if (registro.eliminado) return true; // ya estaba, no se descuenta dos veces

    registro.eliminado = true;
    if (!sobrescribirOrden(&registro)) return false;

    ArchivoHeader header;
    if (!leerHeader(RUTA_ORDENES, &header)) return false;
    header.registrosActivos--;
    return actualizarHeader(RUTA_ORDENES, header);
}

int contarOrdenesActivas() {
    ArchivoHeader header;
    if (!leerHeader(RUTA_ORDENES, &header)) return -1;
    return header.registrosActivos;
}

int totalOrdenes() {
    ArchivoHeader header;
    if (!leerHeader(RUTA_ORDENES, &header)) return -1;
    return header.cantidadRegistros;
}
