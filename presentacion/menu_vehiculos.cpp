#include "menu_vehiculos.hpp"
#include "entrada.hpp"
#include "comunes.hpp"
#include "../logica/vehiculos.hpp"
#include "../logica/utilidades.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>

using namespace std;

void menuVehiculos() {
    int opcion = -1;
    do {
        cout << "\n--- MENU VEHICULOS ---\n";
        cout << "1. Registrar vehiculo\n";
        cout << "2. Buscar vehiculo\n";
        cout << "3. Actualizar vehiculo\n";
        cout << "4. Eliminar vehiculo\n";
        cout << "5. Listar vehiculos\n";
        cout << "0. Volver\n";
        opcion = leerOpcion("Seleccione una opcion", 0, 5);

        switch (opcion) {
            case 1: menuRegistrarVehiculo(); break;
            case 2: menuBuscarVehiculo(); break;
            case 3: menuActualizarVehiculo(); break;
            case 4: menuEliminarVehiculo(); break;
            case 5: menuListarVehiculos(); break;
        }
    } while (opcion != 0 && opcion != -1); // -1: se acabo la entrada
}

void menuRegistrarVehiculo() {
    cout << "\n--- REGISTRAR VEHICULO (Escriba CANCELAR para abortar) ---\n";

    Vehiculo datos;
    memset(&datos, 0, sizeof(Vehiculo)); // asi no se guardan bytes basura en el archivo
    if (!leerTexto("Placa: ", datos.placa, 10)) return;
    if (!leerTexto("Marca: ", datos.marca, 50)) return;
    if (!leerTexto("Modelo: ", datos.modelo, 50)) return;
    if (!leerEntero("Anio: ", &datos.anio)) return;
    if (!leerEntero("Kilometraje: ", &datos.kilometraje)) return;
    if (!leerTexto("Propietario: ", datos.nombrePropietario, 100)) return;
    if (!leerTexto("Cedula del propietario: ", datos.cedulaPropietario, 20)) return;
    if (!leerTexto("Telefono del propietario: ", datos.telefonoPropietario, 20)) return;

    if (!confirmar("Desea guardar este vehiculo? (S/N): ")) {
        cout << "Registro cancelado.\n";
        return;
    }

    int idNuevo = 0;
    Resultado res = registrarVehiculo(datos, &idNuevo);
    convertirAMayusculas(datos.placa);
    if (res == OK) {
        cout << "Vehiculo " << datos.placa << " registrado con ID " << idNuevo << ".\n";
    } else if (res == ERROR_DUPLICADO) {
        cout << "ERROR: La placa '" << datos.placa << "' ya esta registrada.\n";
    } else if (res == ERROR_DATO_INVALIDO) {
        cout << "ERROR: Datos no validos. Revise: placa de 6-7 letras/numeros, anio entre 1950 y 2027, "
             << "kilometraje >= 0, cedula valida y campos no vacios.\n";
    } else {
        mostrarErrorArchivo();
    }
}

void menuBuscarVehiculo() {
    cout << "\n--- BUSCAR VEHICULO ---\n";
    cout << "1. Por ID\n";
    cout << "2. Por placa exacta\n";
    cout << "3. Por propietario (parcial)\n";
    int op = leerOpcion("Seleccione opcion", 1, 3);

    if (op == 1) {
        int id;
        if (!leerEntero("ID del vehiculo: ", &id)) return;

        Vehiculo v;
        Resultado res = obtenerVehiculoPorID(id, &v);
        if (res == OK || res == ERROR_ELIMINADO) {
            // mensaje de depuracion del caso de uso 2: se leyo por calculo, sin recorrer
            cout << "[DEPURACION] Vehiculo ID " << id << " leido en el byte "
                 << posicionVehiculoEnArchivo(id) << " de datos/vehiculos.bin ("
                 << sizeof(ArchivoHeader) << " + (" << id << " - 1) x " << sizeof(Vehiculo) << ")\n";
        }
        if (res == OK) {
            mostrarVehiculo(&v);
        } else if (res == ERROR_ELIMINADO) {
            cout << "El vehiculo ID " << id << " (" << v.placa << ") fue ELIMINADO.\n";
        } else if (res == ERROR_NO_EXISTE) {
            cout << "No existe un vehiculo con ID " << id << ".\n";
        } else {
            mostrarErrorArchivo();
        }
    } else if (op == 2) {
        char placa[10];
        if (!leerTexto("Placa: ", placa, 10)) return;
        convertirAMayusculas(placa);

        Vehiculo v;
        Resultado res = obtenerVehiculoPorPlaca(placa, &v);
        if (res == OK) {
            mostrarVehiculo(&v);
        } else if (res == ERROR_NO_EXISTE) {
            cout << "No hay un vehiculo activo con la placa '" << placa << "'.\n";
        } else {
            mostrarErrorArchivo();
        }
    } else if (op == 3) {
        char texto[100];
        if (!leerTexto("Nombre o parte del propietario: ", texto, 100)) return;

        ListaVehiculos lista;
        Resultado res = buscarVehiculosPorPropietario(texto, &lista);
        if (res != OK) {
            mostrarErrorArchivo();
            return;
        }
        if (lista.cantidad == 0) {
            cout << "Ningun propietario contiene '" << texto << "'.\n";
            return;
        }
        for (int i = 0; i < lista.cantidad; i++) mostrarVehiculo(&lista.items[i]);
        avisarSiHayMas(lista.hayMas, lista.cantidad);
    }
}

void menuActualizarVehiculo() {
    cout << "\n--- ACTUALIZAR VEHICULO ---\n";
    char placa[10];
    if (!leerTexto("Placa del vehiculo: ", placa, 10)) return;
    convertirAMayusculas(placa);

    Vehiculo actual;
    Resultado busqueda = obtenerVehiculoPorPlaca(placa, &actual);
    if (busqueda == ERROR_NO_EXISTE) {
        cout << "No hay un vehiculo activo con la placa '" << placa << "'.\n";
        return;
    }
    if (busqueda != OK) {
        mostrarErrorArchivo();
        return;
    }
    mostrarVehiculo(&actual);

    cout << "Ingrese los datos nuevos (la placa y el kilometraje no se cambian aqui):\n";
    Vehiculo datos = actual;
    if (!leerTexto("Marca: ", datos.marca, 50)) return;
    if (!leerTexto("Modelo: ", datos.modelo, 50)) return;
    if (!leerEntero("Anio: ", &datos.anio)) return;
    if (!leerTexto("Propietario: ", datos.nombrePropietario, 100)) return;
    if (!leerTexto("Cedula del propietario: ", datos.cedulaPropietario, 20)) return;
    if (!leerTexto("Telefono del propietario: ", datos.telefonoPropietario, 20)) return;

    if (!confirmar("Desea guardar los cambios? (S/N): ")) {
        cout << "Actualizacion cancelada.\n";
        return;
    }

    Resultado res = actualizarVehiculo(actual.id, datos);
    if (res == OK) {
        cout << "Vehiculo " << actual.placa << " actualizado.\n";
    } else if (res == ERROR_DATO_INVALIDO) {
        cout << "ERROR: Datos no validos (anio entre 1950 y 2027, cedula valida, campos no vacios).\n";
    } else {
        mostrarErrorArchivo();
    }
}

void menuEliminarVehiculo() {
    cout << "\n--- ELIMINAR VEHICULO ---\n";
    char placa[10];
    if (!leerTexto("Placa del vehiculo: ", placa, 10)) return;
    convertirAMayusculas(placa);

    Vehiculo actual;
    Resultado busqueda = obtenerVehiculoPorPlaca(placa, &actual);
    if (busqueda == ERROR_NO_EXISTE) {
        cout << "No hay un vehiculo activo con la placa '" << placa << "'.\n";
        return;
    }
    if (busqueda != OK) {
        mostrarErrorArchivo();
        return;
    }
    mostrarVehiculo(&actual);

    if (!confirmar("Seguro que desea eliminar este vehiculo? (S/N): ")) {
        cout << "Eliminacion cancelada.\n";
        return;
    }

    int ordenes = 0;
    Resultado res = eliminarVehiculo(actual.id, &ordenes);
    if (res == OK) {
        cout << "Vehiculo " << actual.placa << " eliminado.\n";
    } else if (res == ERROR_TIENE_ORDENES) {
        cout << "ERROR: El vehiculo '" << actual.placa << "' tiene " << ordenes
             << " orden(es) registrada(s) y no puede eliminarse.\n";
    } else {
        mostrarErrorArchivo();
    }
}

void menuListarVehiculos() {
    ListaVehiculos lista;
    if (listarVehiculos(&lista) != OK) {
        mostrarErrorArchivo();
        return;
    }
    if (lista.cantidad == 0) {
        cout << "No hay vehiculos registrados.\n";
        return;
    }

    cout << "\n" << left << setw(5) << "ID" << setw(10) << "PLACA" << setw(14) << "MARCA"
         << setw(14) << "MODELO" << setw(6) << "ANIO" << setw(10) << "KM" << "PROPIETARIO\n";
    cout << string(78, '-') << "\n";
    for (int i = 0; i < lista.cantidad; i++) {
        Vehiculo* v = &lista.items[i];
        cout << left << setw(5) << v->id << setw(10) << v->placa << setw(14) << v->marca
             << setw(14) << v->modelo << setw(6) << v->anio << setw(10) << v->kilometraje
             << v->nombrePropietario << "\n";
    }
    cout << "Total de vehiculos: " << lista.cantidad << "\n";
    avisarSiHayMas(lista.hayMas, lista.cantidad);
}

void mostrarVehiculo(const Vehiculo* v) {
    cout << "----------------------------------------\n";
    cout << "ID " << v->id << " | " << v->placa << " | " << v->marca << " " << v->modelo
         << " " << v->anio << " | " << v->kilometraje << " km\n";
    cout << "Propietario: " << v->nombrePropietario << " (" << v->cedulaPropietario
         << ", tel. " << v->telefonoPropietario << ")\n";
}
