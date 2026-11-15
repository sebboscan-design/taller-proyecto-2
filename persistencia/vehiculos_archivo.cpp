#include "vehiculos_archivo.hpp"
#include <fstream>

using namespace std;

bool guardarVehiculoNuevo(Vehiculo* vehiculo) {
    ArchivoHeader header;
    if (!leerHeader(RUTA_VEHICULOS, &header)) return false;

    time_t ahora = time(nullptr);
    vehiculo->id = header.proximoID;
    vehiculo->eliminado = false;
    vehiculo->fechaCreacion = ahora;
    vehiculo->fechaUltimaModificacion = ahora;

    fstream archivo(RUTA_VEHICULOS, ios::in | ios::out | ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(RUTA_VEHICULOS);
        return false;
    }

    // primero el registro y despues el header: si algo se corta en medio,
    // el header queda atrasado y la verificacion de integridad lo detecta
    archivo.seekp(posicionRegistro(vehiculo->id, sizeof(Vehiculo)));
    archivo.write((char*)vehiculo, sizeof(Vehiculo));

    header.cantidadRegistros++;
    header.registrosActivos++;
    header.proximoID++;
    archivo.seekp(0);
    archivo.write((char*)&header, sizeof(ArchivoHeader));

    bool bien = archivo.good();
    archivo.close();
    return bien;
}

ResultadoArchivo leerVehiculo(int id, Vehiculo* destino) {
    ArchivoHeader header;
    if (!leerHeader(RUTA_VEHICULOS, &header)) return ARCHIVO_ERROR;
    if (id < 1 || id > header.cantidadRegistros) return ARCHIVO_ID_NO_EXISTE;

    ifstream archivo(RUTA_VEHICULOS, ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(RUTA_VEHICULOS);
        return ARCHIVO_ERROR;
    }

    archivo.seekg(posicionRegistro(id, sizeof(Vehiculo)));
    archivo.read((char*)destino, sizeof(Vehiculo));
    bool bien = archivo.gcount() == sizeof(Vehiculo);
    archivo.close();
    return bien ? ARCHIVO_OK : ARCHIVO_ERROR;
}

bool sobrescribirVehiculo(Vehiculo* vehiculo) {
    ArchivoHeader header;
    if (!leerHeader(RUTA_VEHICULOS, &header)) return false;
    if (vehiculo->id < 1 || vehiculo->id > header.cantidadRegistros) return false;

    fstream archivo(RUTA_VEHICULOS, ios::in | ios::out | ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(RUTA_VEHICULOS);
        return false;
    }

    vehiculo->fechaUltimaModificacion = time(nullptr);
    archivo.seekp(posicionRegistro(vehiculo->id, sizeof(Vehiculo)));
    archivo.write((char*)vehiculo, sizeof(Vehiculo));
    bool bien = archivo.good();
    archivo.close();
    return bien;
}

bool marcarVehiculoEliminado(int id) {
    Vehiculo registro;
    if (leerVehiculo(id, &registro) != ARCHIVO_OK) return false;
    if (registro.eliminado) return true; // ya estaba, no se descuenta dos veces

    registro.eliminado = true;
    if (!sobrescribirVehiculo(&registro)) return false;

    ArchivoHeader header;
    if (!leerHeader(RUTA_VEHICULOS, &header)) return false;
    header.registrosActivos--;
    return actualizarHeader(RUTA_VEHICULOS, header);
}

int contarVehiculosActivos() {
    ArchivoHeader header;
    if (!leerHeader(RUTA_VEHICULOS, &header)) return -1;
    return header.registrosActivos;
}

int totalVehiculos() {
    ArchivoHeader header;
    if (!leerHeader(RUTA_VEHICULOS, &header)) return -1;
    return header.cantidadRegistros;
}
