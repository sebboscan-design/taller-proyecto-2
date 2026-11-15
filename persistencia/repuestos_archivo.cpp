#include "repuestos_archivo.hpp"
#include <fstream>

using namespace std;

bool guardarRepuestoNuevo(Repuesto* repuesto) {
    ArchivoHeader header;
    if (!leerHeader(RUTA_REPUESTOS, &header)) return false;

    time_t ahora = time(nullptr);
    repuesto->id = header.proximoID;
    repuesto->eliminado = false;
    repuesto->fechaCreacion = ahora;
    repuesto->fechaUltimaModificacion = ahora;

    fstream archivo(RUTA_REPUESTOS, ios::in | ios::out | ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(RUTA_REPUESTOS);
        return false;
    }

    // primero el registro y despues el header: si algo se corta en medio,
    // el header queda atrasado y la verificacion de integridad lo detecta
    archivo.seekp(posicionRegistro(repuesto->id, sizeof(Repuesto)));
    archivo.write((char*)repuesto, sizeof(Repuesto));

    header.cantidadRegistros++;
    header.registrosActivos++;
    header.proximoID++;
    archivo.seekp(0);
    archivo.write((char*)&header, sizeof(ArchivoHeader));

    bool bien = archivo.good();
    archivo.close();
    return bien;
}

ResultadoArchivo leerRepuesto(int id, Repuesto* destino) {
    ArchivoHeader header;
    if (!leerHeader(RUTA_REPUESTOS, &header)) return ARCHIVO_ERROR;
    if (id < 1 || id > header.cantidadRegistros) return ARCHIVO_ID_NO_EXISTE;

    ifstream archivo(RUTA_REPUESTOS, ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(RUTA_REPUESTOS);
        return ARCHIVO_ERROR;
    }

    archivo.seekg(posicionRegistro(id, sizeof(Repuesto)));
    archivo.read((char*)destino, sizeof(Repuesto));
    bool bien = archivo.gcount() == sizeof(Repuesto);
    archivo.close();
    return bien ? ARCHIVO_OK : ARCHIVO_ERROR;
}

bool sobrescribirRepuesto(Repuesto* repuesto) {
    ArchivoHeader header;
    if (!leerHeader(RUTA_REPUESTOS, &header)) return false;
    if (repuesto->id < 1 || repuesto->id > header.cantidadRegistros) return false;

    fstream archivo(RUTA_REPUESTOS, ios::in | ios::out | ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(RUTA_REPUESTOS);
        return false;
    }

    repuesto->fechaUltimaModificacion = time(nullptr);
    archivo.seekp(posicionRegistro(repuesto->id, sizeof(Repuesto)));
    archivo.write((char*)repuesto, sizeof(Repuesto));
    bool bien = archivo.good();
    archivo.close();
    return bien;
}

bool marcarRepuestoEliminado(int id) {
    Repuesto registro;
    if (leerRepuesto(id, &registro) != ARCHIVO_OK) return false;
    if (registro.eliminado) return true; // ya estaba, no se descuenta dos veces

    registro.eliminado = true;
    if (!sobrescribirRepuesto(&registro)) return false;

    ArchivoHeader header;
    if (!leerHeader(RUTA_REPUESTOS, &header)) return false;
    header.registrosActivos--;
    return actualizarHeader(RUTA_REPUESTOS, header);
}

int contarRepuestosActivos() {
    ArchivoHeader header;
    if (!leerHeader(RUTA_REPUESTOS, &header)) return -1;
    return header.registrosActivos;
}

int totalRepuestos() {
    ArchivoHeader header;
    if (!leerHeader(RUTA_REPUESTOS, &header)) return -1;
    return header.cantidadRegistros;
}
