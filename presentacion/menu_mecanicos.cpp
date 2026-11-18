#include "menu_mecanicos.hpp"
#include "entrada.hpp"
#include "comunes.hpp"
#include "../logica/mecanicos.hpp"
#include "../logica/ordenes.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>

using namespace std;

void menuMecanicos() {
    int opcion = -1;
    do {
        cout << "\n--- MENU MECANICOS ---\n";
        cout << "1. Registrar mecanico\n";
        cout << "2. Buscar mecanico por ID\n";
        cout << "3. Listar mecanicos\n";
        cout << "4. Dar de baja mecanico\n";
        cout << "0. Volver\n";
        opcion = leerOpcion("Seleccione una opcion", 0, 4);

        switch (opcion) {
            case 1: menuRegistrarMecanico(); break;
            case 2: menuBuscarMecanico(); break;
            case 3: menuListarMecanicos(); break;
            case 4: menuDarDeBajaMecanico(); break;
        }
    } while (opcion != 0 && opcion != -1);
}

void menuRegistrarMecanico() {
    cout << "\n--- REGISTRAR MECANICO (Escriba CANCELAR para abortar) ---\n";

    Mecanico datos;
    memset(&datos, 0, sizeof(Mecanico));
    if (!leerTexto("Nombre: ", datos.nombre, 100)) return;
    if (!leerTexto("Cedula: ", datos.cedula, 20)) return;
    if (!leerTexto("Especialidad (MOTOR, FRENOS, ELECTRICIDAD, SUSPENSION, GENERAL): ", datos.especialidad, 20)) return;
    if (!leerTexto("Fecha de ingreso (AAAA-MM-DD): ", datos.fechaIngreso, 11)) return;

    if (!confirmar("Desea guardar este mecanico? (S/N): ")) {
        cout << "Registro cancelado.\n";
        return;
    }

    int idNuevo = 0;
    Resultado res = registrarMecanico(datos, &idNuevo);
    if (res == OK) {
        cout << "Mecanico '" << datos.nombre << "' registrado con ID " << idNuevo << ".\n";
    } else if (res == ERROR_DUPLICADO) {
        cout << "ERROR: La cedula '" << datos.cedula << "' ya esta registrada.\n";
    } else if (res == ERROR_DATO_INVALIDO) {
        cout << "ERROR: Datos no validos. Revise: nombre no vacio, cedula valida, especialidad permitida "
             << "y fecha AAAA-MM-DD.\n";
    } else {
        mostrarErrorArchivo();
    }
}

void menuBuscarMecanico() {
    int id;
    if (!leerEntero("ID del mecanico: ", &id)) return;

    Mecanico m;
    Resultado res = obtenerMecanicoPorID(id, &m);
    if (res == OK || res == ERROR_ELIMINADO) {
        cout << "----------------------------------------\n";
        cout << "ID " << m.id << " | " << m.nombre << " | " << m.cedula << " | " << m.especialidad
             << " | Ingreso: " << m.fechaIngreso << "\n";
        cout << "Estado: " << (res == OK ? "ACTIVO" : "DADO DE BAJA") << "\n";
        int abiertas = contarOrdenesAbiertasMecanico(id);
        if (abiertas >= 0) cout << "Ordenes abiertas: " << abiertas << "\n";
    } else if (res == ERROR_NO_EXISTE) {
        cout << "No existe un mecanico con ID " << id << ".\n";
    } else {
        mostrarErrorArchivo();
    }
}

void menuListarMecanicos() {
    ListaMecanicos lista;
    if (listarMecanicos(&lista) != OK) {
        mostrarErrorArchivo();
        return;
    }
    if (lista.cantidad == 0) {
        cout << "No hay mecanicos registrados.\n";
        return;
    }

    cout << "\n" << left << setw(5) << "ID" << setw(24) << "NOMBRE" << setw(14) << "CEDULA"
         << setw(14) << "ESPECIALIDAD" << setw(10) << "ESTADO" << "ABIERTAS\n";
    cout << string(75, '-') << "\n";
    for (int i = 0; i < lista.cantidad; i++) {
        Mecanico* m = &lista.items[i];
        cout << left << setw(5) << m->id << setw(24) << m->nombre << setw(14) << m->cedula
             << setw(14) << m->especialidad << setw(10) << (m->eliminado ? "BAJA" : "ACTIVO")
             << lista.ordenesAbiertas[i] << "\n";
    }
    cout << "Total de mecanicos: " << lista.cantidad << "\n";
    avisarSiHayMas(lista.hayMas, lista.cantidad);
}

void menuDarDeBajaMecanico() {
    cout << "\n--- DAR DE BAJA MECANICO ---\n";
    int id;
    if (!leerEntero("ID del mecanico: ", &id)) return;

    Mecanico m;
    Resultado busqueda = obtenerMecanicoPorID(id, &m);
    if (busqueda == ERROR_NO_EXISTE) {
        cout << "No existe un mecanico con ID " << id << ".\n";
        return;
    }
    if (busqueda == ERROR_ELIMINADO) {
        cout << "El mecanico '" << m.nombre << "' ya estaba dado de baja.\n";
        return;
    }
    if (busqueda != OK) {
        mostrarErrorArchivo();
        return;
    }

    string pregunta = "Seguro que desea dar de baja a '" + string(m.nombre) + "'? (S/N): ";
    if (!confirmar(pregunta.c_str())) {
        cout << "Operacion cancelada.\n";
        return;
    }

    int abiertas = 0;
    Resultado res = darDeBajaMecanico(id, &abiertas);
    if (res == OK) {
        cout << "Mecanico '" << m.nombre << "' dado de baja. Sus ordenes cerradas y anuladas siguen en el historial.\n";
    } else if (res == ERROR_TIENE_ORDENES) {
        cout << "ERROR: El mecanico '" << m.nombre << "' tiene " << abiertas
             << " orden(es) ABIERTA(S) y no puede darse de baja.\n";
    } else {
        mostrarErrorArchivo();
    }
}
