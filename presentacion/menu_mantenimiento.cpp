#include "menu_mantenimiento.hpp"
#include "entrada.hpp"
#include "comunes.hpp"
#include "../logica/mantenimiento.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

void menuMantenimiento() {
    int opcion = -1;
    do {
        cout << "\n--- MANTENIMIENTO ---\n";
        cout << "1. Verificar integridad\n";
        cout << "2. Crear backup\n";
        cout << "0. Volver\n";
        opcion = leerOpcion("Seleccione una opcion", 0, 2);

        switch (opcion) {
            case 1: menuVerificarIntegridad(); break;
            case 2: menuCrearBackup(); break;
        }
    } while (opcion != 0 && opcion != -1);
}

void menuVerificarIntegridad() {
    ReporteIntegridad reporte;
    if (verificarIntegridad(&reporte) != OK) {
        mostrarErrorArchivo();
        return;
    }

    cout << "\n";
    int mostrados = reporte.problemas < MAX_PROBLEMAS ? reporte.problemas : MAX_PROBLEMAS;
    for (int i = 0; i < mostrados; i++) cout << reporte.mensajes[i] << "\n";
    if (reporte.problemas > MAX_PROBLEMAS) {
        cout << "(y " << reporte.problemas - MAX_PROBLEMAS << " problema(s) mas)\n";
    }

    cout << "+-------------------------------------------------------+\n";
    cout << "|            REPORTE DE INTEGRIDAD DEL SISTEMA          |\n";
    cout << "+-------------------------------------------------------+\n";
    cout << "| Vehiculos verificados : " << left << setw(30) << reporte.vehiculosVerificados << "|\n";
    cout << "| Mecanicos verificados : " << setw(30) << reporte.mecanicosVerificados << "|\n";
    cout << "| Repuestos verificados : " << setw(30) << reporte.repuestosVerificados << "|\n";
    cout << "| Ordenes verificadas   : " << setw(30) << reporte.ordenesVerificadas << "|\n";
    cout << "|                                                       |\n";
    cout << "| Problemas encontrados : " << setw(30) << reporte.problemas << "|\n";
    cout << "| Estado: " << setw(46) << (reporte.problemas == 0 ? "SISTEMA INTEGRO" : "SE ENCONTRARON PROBLEMAS") << "|\n";
    cout << "+-------------------------------------------------------+\n";
}

void menuCrearBackup() {
    char ruta[80];
    if (crearBackup(ruta, 80) != OK) {
        mostrarErrorArchivo();
        return;
    }
    cout << "Backup creado en la carpeta: " << ruta << "\n";
}
