#include "menu_repuestos.hpp"
#include "entrada.hpp"
#include "comunes.hpp"
#include "../logica/repuestos.hpp"
#include "../logica/utilidades.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>

using namespace std;

void menuRepuestos() {
    int opcion = -1;
    do {
        cout << "\n--- MENU REPUESTOS ---\n";
        cout << "1. Registrar repuesto\n";
        cout << "2. Buscar repuesto\n";
        cout << "3. Entrada de mercancia\n";
        cout << "4. Cambiar precio\n";
        cout << "5. Eliminar repuesto\n";
        cout << "6. Listar repuestos\n";
        cout << "0. Volver\n";
        opcion = leerOpcion("Seleccione una opcion", 0, 6);

        switch (opcion) {
            case 1: menuRegistrarRepuesto(); break;
            case 2: menuBuscarRepuesto(); break;
            case 3: menuEntradaMercancia(); break;
            case 4: menuCambiarPrecio(); break;
            case 5: menuEliminarRepuesto(); break;
            case 6: menuListarRepuestos(); break;
        }
    } while (opcion != 0 && opcion != -1);
}

// Busca un repuesto activo por codigo y avisa si no esta. true si lo encontro
static bool pedirRepuestoPorCodigo(Repuesto* destino) {
    char codigo[20];
    if (!leerTexto("Codigo del repuesto: ", codigo, 20)) return false;
    convertirAMayusculas(codigo);

    Resultado res = obtenerRepuestoPorCodigo(codigo, destino);
    if (res == ERROR_NO_EXISTE) {
        cout << "No hay un repuesto activo con el codigo '" << codigo << "'.\n";
        return false;
    }
    if (res != OK) {
        mostrarErrorArchivo();
        return false;
    }
    return true;
}

void menuRegistrarRepuesto() {
    cout << "\n--- REGISTRAR REPUESTO (Escriba CANCELAR para abortar) ---\n";

    Repuesto datos;
    memset(&datos, 0, sizeof(Repuesto));
    if (!leerTexto("Codigo: ", datos.codigo, 20)) return;
    if (!leerTexto("Nombre: ", datos.nombre, 100)) return;
    if (!leerTexto("Marca: ", datos.marca, 50)) return;
    if (!leerFlotante("Precio: ", &datos.precio)) return;
    if (!leerEntero("Stock: ", &datos.stock)) return;
    if (!leerEntero("Stock minimo: ", &datos.stockMinimo)) return;

    if (!confirmar("Desea guardar este repuesto? (S/N): ")) {
        cout << "Registro cancelado.\n";
        return;
    }

    int idNuevo = 0;
    Resultado res = registrarRepuesto(datos, &idNuevo);
    convertirAMayusculas(datos.codigo);
    if (res == OK) {
        cout << "Repuesto '" << datos.nombre << "' (" << datos.codigo << ") registrado con ID " << idNuevo << ".\n";
    } else if (res == ERROR_DUPLICADO) {
        cout << "ERROR: El codigo '" << datos.codigo << "' ya esta registrado.\n";
    } else if (res == ERROR_DATO_INVALIDO) {
        cout << "ERROR: Datos no validos. Revise: precio > 0, stock y stock minimo >= 0, campos no vacios.\n";
    } else {
        mostrarErrorArchivo();
    }
}

void menuBuscarRepuesto() {
    char texto[100];
    if (!leerTexto("Codigo o parte del nombre: ", texto, 100)) return;

    ListaRepuestos lista;
    if (buscarRepuestos(texto, &lista) != OK) {
        mostrarErrorArchivo();
        return;
    }
    if (lista.cantidad == 0) {
        cout << "Ningun repuesto coincide con '" << texto << "'.\n";
        return;
    }
    for (int i = 0; i < lista.cantidad; i++) mostrarRepuesto(&lista.items[i]);
    avisarSiHayMas(lista.hayMas, lista.cantidad);
}

void menuEntradaMercancia() {
    cout << "\n--- ENTRADA DE MERCANCIA ---\n";
    Repuesto r;
    if (!pedirRepuestoPorCodigo(&r)) return;
    int cantidad;
    if (!leerEntero("Cantidad que llego: ", &cantidad)) return;

    Resultado res = registrarEntradaMercancia(r.id, cantidad);
    if (res == OK) {
        cout << "Entrada registrada. '" << r.nombre << "' ahora tiene " << r.stock + cantidad << " unidades.\n";
    } else if (res == ERROR_DATO_INVALIDO) {
        cout << "ERROR: La cantidad debe ser mayor a 0 (se ingreso " << cantidad << ").\n";
    } else {
        mostrarErrorArchivo();
    }
}

void menuCambiarPrecio() {
    cout << "\n--- CAMBIAR PRECIO ---\n";
    Repuesto r;
    if (!pedirRepuestoPorCodigo(&r)) return;
    cout << "Precio actual de '" << r.nombre << "': " << fixed << setprecision(2) << r.precio << "\n";
    float nuevoPrecio;
    if (!leerFlotante("Nuevo precio: ", &nuevoPrecio)) return;

    string pregunta = "Confirma el nuevo precio de '" + string(r.nombre) + "'? (S/N): ";
    if (!confirmar(pregunta.c_str())) {
        cout << "Cambio cancelado.\n";
        return;
    }

    Resultado res = cambiarPrecioRepuesto(r.id, nuevoPrecio);
    if (res == OK) {
        cout << "Precio de '" << r.nombre << "' actualizado a " << nuevoPrecio
             << ". Las ordenes ya registradas conservan su precio.\n";
    } else if (res == ERROR_DATO_INVALIDO) {
        cout << "ERROR: El precio debe ser mayor a 0 (se ingreso " << nuevoPrecio << ").\n";
    } else {
        mostrarErrorArchivo();
    }
}

void menuEliminarRepuesto() {
    cout << "\n--- ELIMINAR REPUESTO ---\n";
    Repuesto r;
    if (!pedirRepuestoPorCodigo(&r)) return;
    mostrarRepuesto(&r);

    if (!confirmar("Seguro que desea eliminar este repuesto? (S/N): ")) {
        cout << "Eliminacion cancelada.\n";
        return;
    }

    Resultado res = eliminarRepuesto(r.id);
    if (res == OK) {
        cout << "Repuesto '" << r.nombre << "' eliminado.\n";
    } else if (res == ERROR_EN_USO) {
        cout << "ERROR: El repuesto '" << r.nombre << "' aparece en el detalle de alguna orden y no puede eliminarse.\n";
    } else {
        mostrarErrorArchivo();
    }
}

void menuListarRepuestos() {
    ListaRepuestos lista;
    if (listarRepuestos(&lista) != OK) {
        mostrarErrorArchivo();
        return;
    }
    if (lista.cantidad == 0) {
        cout << "No hay repuestos registrados.\n";
        return;
    }
    cout << "\n--- INVENTARIO DE REPUESTOS ---\n";
    for (int i = 0; i < lista.cantidad; i++) mostrarRepuesto(&lista.items[i]);
    cout << "Total de repuestos: " << lista.cantidad << "\n";
    avisarSiHayMas(lista.hayMas, lista.cantidad);
}

void mostrarRepuesto(const Repuesto* r) {
    cout << "ID " << r->id << " | " << r->codigo << " | " << r->nombre << " | " << r->marca
         << " | Precio: " << fixed << setprecision(2) << r->precio
         << " | Stock: " << r->stock << " (min " << r->stockMinimo << ")";
    if (tieneBajoStock(r)) cout << " [BAJO STOCK]";
    cout << "\n";
}
