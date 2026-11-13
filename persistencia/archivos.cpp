#include "archivos.h"
#include <fstream>
#include <filesystem>

using namespace std;

static char archivoConError[100] = "";

void anotarErrorArchivo(const char* ruta) {
    int i = 0;
    for (; ruta[i] != '\0' && i < 99; i++) archivoConError[i] = ruta[i];
    archivoConError[i] = '\0';
}

const char* ultimoArchivoConError() {
    return archivoConError;
}

bool prepararCarpetaDatos() {
    // filesystem solo se usa para crear carpetas; todo lo demas es fstream
    error_code error;
    filesystem::create_directories(RUTA_DATOS, error);
    return filesystem::is_directory(RUTA_DATOS);
}

bool inicializarArchivo(const char* ruta) {
    // si ya existe no se toca, para no borrar lo que habia
    ifstream existente(ruta, ios::binary);
    if (existente.is_open()) {
        existente.close();
        return true;
    }

    ofstream nuevo(ruta, ios::binary);
    if (!nuevo.is_open()) {
        anotarErrorArchivo(ruta);
        return false;
    }

    ArchivoHeader header;
    header.cantidadRegistros = 0;
    header.proximoID = 1;
    header.registrosActivos = 0;
    header.version = 1;
    nuevo.write((char*)&header, sizeof(ArchivoHeader));
    bool bien = nuevo.good();
    nuevo.close();
    return bien;
}

bool leerHeader(const char* ruta, ArchivoHeader* destino) {
    ifstream archivo(ruta, ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(ruta);
        return false;
    }

    archivo.seekg(0);
    archivo.read((char*)destino, sizeof(ArchivoHeader));
    bool bien = archivo.gcount() == sizeof(ArchivoHeader);
    archivo.close();
    return bien;
}

bool actualizarHeader(const char* ruta, ArchivoHeader header) {
    // in|out para no truncar el archivo: solo se pisan los bytes del header
    fstream archivo(ruta, ios::in | ios::out | ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(ruta);
        return false;
    }

    archivo.seekp(0);
    archivo.write((char*)&header, sizeof(ArchivoHeader));
    bool bien = archivo.good();
    archivo.close();
    return bien;
}

long posicionRegistro(int id, int tamanoRegistro) {
    return (long)sizeof(ArchivoHeader) + (long)(id - 1) * tamanoRegistro;
}

int registrosRealesEnArchivo(const char* ruta, int tamanoRegistro) {
    ifstream archivo(ruta, ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(ruta);
        return -1;
    }

    archivo.seekg(0, ios::end);
    long bytes = (long)archivo.tellg();
    archivo.close();

    long bytesRegistros = bytes - (long)sizeof(ArchivoHeader);
    if (bytesRegistros < 0) return 0;
    return (int)(bytesRegistros / tamanoRegistro);
}

bool leerRegistroCrudo(const char* ruta, int indice, void* destino, int tamanoRegistro) {
    ifstream archivo(ruta, ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(ruta);
        return false;
    }

    archivo.seekg((long)sizeof(ArchivoHeader) + (long)indice * tamanoRegistro);
    archivo.read((char*)destino, tamanoRegistro);
    bool bien = archivo.gcount() == tamanoRegistro;
    archivo.close();
    return bien;
}

bool crearCarpeta(const char* ruta) {
    error_code error;
    filesystem::create_directories(ruta, error);
    return filesystem::is_directory(ruta);
}

bool copiarArchivo(const char* origen, const char* destino) {
    ifstream entrada(origen, ios::binary);
    if (!entrada.is_open()) {
        anotarErrorArchivo(origen);
        return false;
    }
    ofstream salida(destino, ios::binary);
    if (!salida.is_open()) {
        anotarErrorArchivo(destino);
        return false;
    }

    char byte;
    while (entrada.get(byte)) {
        salida.put(byte);
    }
    bool bien = salida.good();
    entrada.close();
    salida.close();
    return bien;
}

bool existeTaller() {
    ifstream archivo(RUTA_TALLER, ios::binary);
    bool existe = archivo.is_open();
    archivo.close();
    return existe;
}

bool leerTaller(Taller* destino) {
    ifstream archivo(RUTA_TALLER, ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(RUTA_TALLER);
        return false;
    }

    archivo.read((char*)destino, sizeof(Taller));
    bool bien = archivo.gcount() == sizeof(Taller);
    archivo.close();
    return bien;
}

bool guardarTaller(Taller* taller) {
    ofstream archivo(RUTA_TALLER, ios::binary);
    if (!archivo.is_open()) {
        anotarErrorArchivo(RUTA_TALLER);
        return false;
    }

    time_t ahora = time(nullptr);
    taller->fechaCreacion = ahora;
    taller->fechaUltimaModificacion = ahora;
    archivo.write((char*)taller, sizeof(Taller));
    bool bien = archivo.good();
    archivo.close();
    return bien;
}
