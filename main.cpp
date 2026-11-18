#include "logica/sistema.hpp"
#include "presentacion/entrada.hpp"
#include "presentacion/comunes.hpp"
#include "presentacion/menu_vehiculos.hpp"
#include "presentacion/menu_mecanicos.hpp"
#include "presentacion/menu_repuestos.hpp"
#include "presentacion/menu_ordenes.hpp"
#include "presentacion/menu_reportes.hpp"
#include "presentacion/menu_mantenimiento.hpp"
#include <iostream>
#include <cstring>

using namespace std;

// Primera vez que se corre: pide los datos del taller y crea taller.bin
static bool pedirDatosTaller() {
    cout << "Primer arranque: ingrese los datos del taller.\n";
    Taller datos;
    memset(&datos, 0, sizeof(Taller));
    if (!leerTexto("Nombre del taller: ", datos.nombre, 100)) return false;
    if (!leerTexto("RIF: ", datos.rif, 20)) return false;
    if (!leerTexto("Direccion: ", datos.direccion, 200)) return false;
    if (!leerTexto("Telefono: ", datos.telefono, 20)) return false;

    Resultado res = registrarTaller(datos);
    if (res == ERROR_DATO_INVALIDO) {
        cout << "ERROR: El nombre y el RIF del taller no pueden estar vacios.\n";
        return false;
    }
    if (res != OK) {
        mostrarErrorArchivo();
        return false;
    }
    cout << "Taller registrado.\n";
    return true;
}

void menuPrincipal(const Taller* taller) {
    int opcion = -1;
    do {
        cout << "\n+-------------------------------------------+\n";
        cout << "  SISTEMA DE GESTION DE TALLER\n";
        cout << "  Taller: " << taller->nombre << "\n";
        cout << "+-------------------------------------------+\n";
        cout << "1. Gestion de Vehiculos\n";
        cout << "2. Gestion de Mecanicos\n";
        cout << "3. Inventario de Repuestos\n";
        cout << "4. Ordenes de Servicio\n";
        cout << "5. Reportes\n";
        cout << "6. Mantenimiento\n";
        cout << "0. Salir\n";
        opcion = leerOpcion("Seleccione una opcion", 0, 6);

        switch (opcion) {
            case 1: menuVehiculos(); break;
            case 2: menuMecanicos(); break;
            case 3: menuRepuestos(); break;
            case 4: menuOrdenes(); break;
            case 5: menuReportes(); break;
            case 6: menuMantenimiento(); break;
            case 0:
                // los datos ya estan en disco, pero igual se confirma
                if (!confirmar("Desea salir del programa? (S/N): ")) opcion = 7;
                break;
        }
    } while (opcion != 0 && opcion != -1);
    cout << "Hasta luego.\n";
}

int main() {
    // al arrancar se crean datos/ y los .bin que falten
    if (prepararArchivos() != OK) {
        mostrarErrorArchivo();
        return 1;
    }
    if (!tallerRegistrado()) {
        if (!pedirDatosTaller()) return 1;
    }

    Taller taller;
    if (obtenerTaller(&taller) != OK) {
        mostrarErrorArchivo();
        return 1;
    }

    menuPrincipal(&taller);
    return 0;
}
