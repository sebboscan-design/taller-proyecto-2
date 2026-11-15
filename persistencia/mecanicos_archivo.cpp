#include "mecanicos_archivo.hpp"
#include <fstream>

using namespace std;

bool guardarMecanicoNuevo(Mecanico* mecanico) {
    ArchivoHeader header;
    if (!leerHeader(RUTA_MECANICOS, &header)) return false;

    time_t ahora = time(nullptr);
    mecanico->id = header.proximoID;
    mecanico->eliminado = false;
    mecanico->fechaCreacion = ahora;
    mecanico->fechaUltimaModificacion = ahora;

    fstream archivo(RUTA_MECANICOS, ios::in | ios::out | ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(RUTA_MECANICOS);
        return false;
    }

    // primero el registro y despues el header: si algo se corta en medio,
    // el header queda atrasado y la verificacion de integridad lo detecta
    archivo.seekp(posicionRegistro(mecanico->id, sizeof(Mecanico)));
    archivo.write((char*)mecanico, sizeof(Mecanico));

    header.cantidadRegistros++;
    header.registrosActivos++;
    header.proximoID++;
    archivo.seekp(0);
    archivo.write((char*)&header, sizeof(ArchivoHeader));

    bool bien = archivo.good();
    archivo.close();
    return bien;
}

ResultadoArchivo leerMecanico(int id, Mecanico* destino) {
    ArchivoHeader header;
    if (!leerHeader(RUTA_MECANICOS, &header)) return ARCHIVO_ERROR;
    if (id < 1 || id > header.cantidadRegistros) return ARCHIVO_ID_NO_EXISTE;

    ifstream archivo(RUTA_MECANICOS, ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(RUTA_MECANICOS);
        return ARCHIVO_ERROR;
    }

    archivo.seekg(posicionRegistro(id, sizeof(Mecanico)));
    archivo.read((char*)destino, sizeof(Mecanico));
    bool bien = archivo.gcount() == sizeof(Mecanico);
    archivo.close();
    return bien ? ARCHIVO_OK : ARCHIVO_ERROR;
}

bool sobrescribirMecanico(Mecanico* mecanico) {
    ArchivoHeader header;
    if (!leerHeader(RUTA_MECANICOS, &header)) return false;
    if (mecanico->id < 1 || mecanico->id > header.cantidadRegistros) return false;

    fstream archivo(RUTA_MECANICOS, ios::in | ios::out | ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(RUTA_MECANICOS);
        return false;
    }

    mecanico->fechaUltimaModificacion = time(nullptr);
    archivo.seekp(posicionRegistro(mecanico->id, sizeof(Mecanico)));
    archivo.write((char*)mecanico, sizeof(Mecanico));
    bool bien = archivo.good();
    archivo.close();
    return bien;
}

bool marcarMecanicoEliminado(int id) {
    Mecanico registro;
    if (leerMecanico(id, &registro) != ARCHIVO_OK) return false;
    if (registro.eliminado) return true; // ya estaba, no se descuenta dos veces

    registro.eliminado = true;
    if (!sobrescribirMecanico(&registro)) return false;

    ArchivoHeader header;
    if (!leerHeader(RUTA_MECANICOS, &header)) return false;
    header.registrosActivos--;
    return actualizarHeader(RUTA_MECANICOS, header);
}

int contarMecanicosActivos() {
    ArchivoHeader header;
    if (!leerHeader(RUTA_MECANICOS, &header)) return -1;
    return header.registrosActivos;
}

int totalMecanicos() {
    ArchivoHeader header;
    if (!leerHeader(RUTA_MECANICOS, &header)) return -1;
    return header.cantidadRegistros;
}
