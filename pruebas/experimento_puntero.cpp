// Experimento de la pregunta 1 del README_V2.
// Guarda en un .bin una orden "estilo Proyecto 1" (con DetalleOrden* detalles)
// y en otra ejecucion la vuelve a leer para ver que pasa con sus lineas.
//
// Compilar:  g++ pruebas/experimento_puntero.cpp -o experimento
// Correr:    ./experimento escribir     (primera ejecucion)
//            ./experimento leer         (segunda ejecucion, programa nuevo)

#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

struct DetalleOrden {
    int idRepuesto;
    int cantidad;
    float precioUnitario;
};

// la orden del Proyecto 1, con el detalle en un arreglo dinamico
struct OrdenP1 {
    int id;
    char estado[12];
    DetalleOrden* detalles; // esto es solo una direccion de memoria
    int numDetalles;
    int capacidadDetalles;
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Uso: experimento escribir | experimento leer\n";
        return 0;
    }

    if (strcmp(argv[1], "escribir") == 0) {
        OrdenP1 orden;
        orden.id = 1;
        strcpy(orden.estado, "ABIERTA");
        orden.capacidadDetalles = 2;
        orden.detalles = new DetalleOrden[2];
        orden.detalles[0] = {1, 3, 25.0f};
        orden.detalles[1] = {4, 1, 15.0f};
        orden.numDetalles = 2;

        ofstream archivo("experimento.bin", ios::binary);
        archivo.write((char*)&orden, sizeof(OrdenP1));
        archivo.close();

        cout << "Se escribio la orden con " << sizeof(OrdenP1) << " bytes.\n";
        cout << "El puntero 'detalles' guardado vale: " << (void*)orden.detalles << "\n";
        cout << "Las lineas NO se escribieron, solo esa direccion.\n";
        delete[] orden.detalles;
    } else {
        OrdenP1 orden;
        ifstream archivo("experimento.bin", ios::binary);
        archivo.read((char*)&orden, sizeof(OrdenP1));
        archivo.close();

        cout << "Orden #" << orden.id << " (" << orden.estado << ") con " << orden.numDetalles << " lineas.\n";
        cout << "El puntero leido vale: " << (void*)orden.detalles << "\n";
        cout << "Esa direccion era de la ejecucion anterior. Intentando leer la primera linea..." << endl; // endl para que se vea antes del posible cierre
        // aqui normalmente sale basura o el programa se cae (violacion de segmento)
        cout << "Repuesto: " << orden.detalles[0].idRepuesto << ", cantidad: " << orden.detalles[0].cantidad << "\n";
    }
    return 0;
}
