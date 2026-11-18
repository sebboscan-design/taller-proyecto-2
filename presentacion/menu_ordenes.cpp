#include "menu_ordenes.hpp"
#include "entrada.hpp"
#include "comunes.hpp"
#include "../logica/ordenes.hpp"
#include "../logica/vehiculos.hpp"
#include "../logica/mecanicos.hpp"
#include "../logica/repuestos.hpp"
#include "../logica/utilidades.hpp"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

void menuOrdenes() {
    int opcion = -1;
    do {
        cout << "\n--- MENU ORDENES DE SERVICIO ---\n";
        cout << "1. Abrir orden\n";
        cout << "2. Agregar repuesto a orden\n";
        cout << "3. Quitar repuesto de orden\n";
        cout << "4. Cerrar orden\n";
        cout << "5. Anular orden\n";
        cout << "6. Ver detalle de orden\n";
        cout << "7. Listar ordenes\n";
        cout << "0. Volver\n";
        opcion = leerOpcion("Seleccione una opcion", 0, 7);

        switch (opcion) {
            case 1: menuAbrirOrden(); break;
            case 2: menuAgregarRepuestoOrden(); break;
            case 3: menuQuitarRepuestoOrden(); break;
            case 4: menuCerrarOrden(); break;
            case 5: menuAnularOrden(); break;
            case 6: menuVerDetalleOrden(); break;
            case 7: menuListarOrdenes(); break;
        }
    } while (opcion != 0 && opcion != -1);
}

// Pide el numero de orden y la lee. true si existe
static bool pedirOrden(const char* mensaje, OrdenServicio* orden) {
    int id;
    if (!leerEntero(mensaje, &id)) return false;
    Resultado res = obtenerOrdenPorID(id, orden);
    if (res == ERROR_NO_EXISTE) {
        cout << "No existe la orden #" << id << ".\n";
        return false;
    }
    if (res != OK) {
        mostrarErrorArchivo();
        return false;
    }
    return true;
}

// Mensaje para cuando la orden no esta en el estado que hace falta
static void avisarEstado(const OrdenServicio* orden) {
    cout << "ERROR: La orden #" << orden->id << " esta " << orden->estado << " y no puede modificarse.\n";
}

void menuAbrirOrden() {
    cout << "\n--- ABRIR ORDEN (Escriba CANCELAR para abortar) ---\n";
    char placa[10];
    if (!leerTexto("Placa del vehiculo: ", placa, 10)) return;
    convertirAMayusculas(placa);

    Vehiculo vehiculo;
    Resultado busqueda = obtenerVehiculoPorPlaca(placa, &vehiculo);
    if (busqueda == ERROR_NO_EXISTE) {
        cout << "ERROR: No hay un vehiculo activo con la placa '" << placa << "'.\n";
        return;
    }
    if (busqueda != OK) {
        mostrarErrorArchivo();
        return;
    }

    int idMecanico, kilometraje;
    char falla[200];
    if (!leerEntero("ID del mecanico: ", &idMecanico)) return;
    if (!leerEntero("Kilometraje de ingreso: ", &kilometraje)) return;
    if (!leerTexto("Descripcion de la falla: ", falla, 200)) return;

    if (!confirmar("Confirma la apertura de la orden? (S/N): ")) {
        cout << "Operacion cancelada.\n";
        return;
    }

    int idNueva = 0;
    Resultado res = abrirOrden(vehiculo.id, idMecanico, kilometraje, falla, &idNueva);
    char mecanico[130];
    describirMecanico(idMecanico, mecanico, 130);

    if (res == OK) {
        cout << "Orden #" << idNueva << " abierta para " << vehiculo.placa << " con " << mecanico << ".\n";
    } else if (res == ERROR_MECANICO_NO_EXISTE) {
        cout << "ERROR: No existe un mecanico con ID " << idMecanico << ".\n";
    } else if (res == ERROR_MECANICO_DE_BAJA) {
        cout << "ERROR: El mecanico '" << mecanico << "' esta dado de baja y no puede recibir ordenes nuevas.\n";
    } else if (res == ERROR_VEHICULO_CON_ORDEN_ABIERTA) {
        int abierta = 0;
        buscarOrdenAbiertaDeVehiculo(vehiculo.id, &abierta);
        cout << "ERROR: El vehiculo '" << vehiculo.placa << "' ya tiene la orden #" << abierta << " ABIERTA.\n";
    } else if (res == ERROR_MECANICO_SIN_CUPO) {
        cout << "ERROR: El mecanico '" << mecanico << "' ya tiene " << MAX_ORDENES_ABIERTAS_MECANICO
             << " ordenes abiertas.\n";
    } else if (res == ERROR_KILOMETRAJE_MENOR) {
        cout << "ERROR: El kilometraje " << kilometraje << " es menor al ultimo registrado para '"
             << vehiculo.placa << "' (" << vehiculo.kilometraje << " km).\n";
    } else if (res == ERROR_DATO_INVALIDO) {
        cout << "ERROR: La descripcion de la falla no puede estar vacia.\n";
    } else if (res == ERROR_VEHICULO_NO_DISPONIBLE) {
        cout << "ERROR: El vehiculo '" << vehiculo.placa << "' no esta disponible.\n";
    } else {
        mostrarErrorArchivo();
    }
}

void menuAgregarRepuestoOrden() {
    cout << "\n--- AGREGAR REPUESTO A ORDEN ---\n";
    OrdenServicio orden;
    if (!pedirOrden("Numero de orden: ", &orden)) return;
    if (string(orden.estado) != "ABIERTA") {
        avisarEstado(&orden);
        return;
    }

    char codigo[20];
    if (!leerTexto("Codigo del repuesto: ", codigo, 20)) return;
    convertirAMayusculas(codigo);
    Repuesto repuesto;
    Resultado busqueda = obtenerRepuestoPorCodigo(codigo, &repuesto);
    if (busqueda == ERROR_NO_EXISTE) {
        cout << "ERROR: No hay un repuesto activo con el codigo '" << codigo << "'.\n";
        return;
    }
    if (busqueda != OK) {
        mostrarErrorArchivo();
        return;
    }

    int cantidad;
    if (!leerEntero("Cantidad: ", &cantidad)) return;

    Resultado res = agregarRepuestoAOrden(orden.id, repuesto.id, cantidad);
    if (res == OK) {
        cout << cantidad << " x '" << repuesto.nombre << "' agregado(s) a la orden #" << orden.id << ".\n";
    } else if (res == ERROR_STOCK_INSUFICIENTE) {
        cout << "ERROR: Stock insuficiente de '" << repuesto.nombre << "'. Disponible: " << repuesto.stock
             << ", solicitado: " << cantidad << ".\n";
    } else if (res == ERROR_LIMITE_LINEAS) {
        cout << "ERROR: La orden #" << orden.id << " ya tiene " << MAX_LINEAS_ORDEN
             << " repuestos distintos (es el maximo). La linea no se agrego.\n";
    } else if (res == ERROR_DATO_INVALIDO) {
        cout << "ERROR: La cantidad debe ser mayor a 0 (se ingreso " << cantidad << ").\n";
    } else if (res == ERROR_ESTADO_INVALIDO) {
        avisarEstado(&orden);
    } else {
        mostrarErrorArchivo();
    }
}

void menuQuitarRepuestoOrden() {
    cout << "\n--- QUITAR REPUESTO DE ORDEN ---\n";
    OrdenServicio orden;
    if (!pedirOrden("Numero de orden: ", &orden)) return;
    if (string(orden.estado) != "ABIERTA") {
        avisarEstado(&orden);
        return;
    }

    char codigo[20];
    if (!leerTexto("Codigo del repuesto a quitar: ", codigo, 20)) return;
    convertirAMayusculas(codigo);
    Repuesto repuesto;
    Resultado busqueda = obtenerRepuestoPorCodigo(codigo, &repuesto);
    if (busqueda == ERROR_NO_EXISTE) {
        cout << "ERROR: No hay un repuesto activo con el codigo '" << codigo << "'.\n";
        return;
    }
    if (busqueda != OK) {
        mostrarErrorArchivo();
        return;
    }

    Resultado res = quitarRepuestoDeOrden(orden.id, repuesto.id);
    if (res == OK) {
        cout << "'" << repuesto.nombre << "' quitado de la orden #" << orden.id
             << ". Sus unidades volvieron al inventario.\n";
    } else if (res == ERROR_REPUESTO_NO_ESTA) {
        cout << "ERROR: La orden #" << orden.id << " no tiene '" << repuesto.nombre << "'.\n";
    } else if (res == ERROR_ESTADO_INVALIDO) {
        avisarEstado(&orden);
    } else {
        mostrarErrorArchivo();
    }
}

void menuCerrarOrden() {
    cout << "\n--- CERRAR ORDEN ---\n";
    OrdenServicio orden;
    if (!pedirOrden("Numero de orden: ", &orden)) return;
    if (string(orden.estado) != "ABIERTA") {
        cout << "ERROR: La orden #" << orden.id << " esta " << orden.estado << "; solo se cierran ordenes ABIERTAS.\n";
        return;
    }
    float manoObra;
    if (!leerFlotante("Costo de mano de obra: ", &manoObra)) return;

    string pregunta = "Confirma el cierre de la orden #" + to_string(orden.id) + "? (S/N): ";
    if (!confirmar(pregunta.c_str())) {
        cout << "Operacion cancelada.\n";
        return;
    }

    Resultado res = cerrarOrden(orden.id, manoObra);
    if (res == OK) {
        obtenerOrdenPorID(orden.id, &orden);
        cout << "Orden #" << orden.id << " cerrada. Total: " << fixed << setprecision(2)
             << calcularTotalOrden(&orden) << "\n";
    } else if (res == ERROR_ORDEN_VACIA) {
        cout << "ERROR: La orden #" << orden.id << " no tiene repuestos y la mano de obra es 0.\n";
    } else if (res == ERROR_DATO_INVALIDO) {
        cout << "ERROR: La mano de obra no puede ser negativa (se ingreso " << manoObra << ").\n";
    } else if (res == ERROR_ESTADO_INVALIDO) {
        avisarEstado(&orden);
    } else {
        mostrarErrorArchivo();
    }
}

void menuAnularOrden() {
    cout << "\n--- ANULAR ORDEN ---\n";
    OrdenServicio orden;
    if (!pedirOrden("Numero de orden: ", &orden)) return;
    if (string(orden.estado) == "ANULADA") {
        cout << "ERROR: La orden #" << orden.id << " ya esta ANULADA.\n";
        return;
    }

    string pregunta = "Seguro que desea anular la orden #" + to_string(orden.id) + " (" + orden.estado + ")? (S/N): ";
    if (!confirmar(pregunta.c_str())) {
        cout << "Operacion cancelada.\n";
        return;
    }

    Resultado res = anularOrden(orden.id);
    if (res == OK) {
        cout << "Orden #" << orden.id << " anulada. Las unidades de sus " << orden.numLineas
             << " linea(s) volvieron al inventario.\n";
    } else if (res == ERROR_ESTADO_INVALIDO) {
        cout << "ERROR: La orden #" << orden.id << " ya esta ANULADA.\n";
    } else {
        mostrarErrorArchivo();
    }
}

void menuVerDetalleOrden() {
    OrdenServicio orden;
    if (!pedirOrden("Numero de orden: ", &orden)) return;
    mostrarDetalleOrden(&orden);
}

void menuListarOrdenes() {
    cout << "\n--- LISTAR ORDENES ---\n";
    cout << "1. Todas\n";
    cout << "2. Por estado\n";
    cout << "3. Historial de un vehiculo\n";
    int op = leerOpcion("Seleccione una opcion", 1, 3);

    ListaOrdenes lista;
    Resultado res;
    if (op == 1) {
        res = listarOrdenes(&lista);
    } else if (op == 2) {
        cout << "1. ABIERTA\n2. CERRADA\n3. ANULADA\n";
        int e = leerOpcion("Estado", 1, 3);
        if (e < 1) return;
        const char* estados[] = {"ABIERTA", "CERRADA", "ANULADA"};
        res = listarOrdenesPorEstado(estados[e - 1], &lista);
    } else if (op == 3) {
        char placa[10];
        if (!leerTexto("Placa del vehiculo: ", placa, 10)) return;
        convertirAMayusculas(placa);
        Vehiculo vehiculo;
        Resultado busqueda = obtenerVehiculoPorPlaca(placa, &vehiculo);
        if (busqueda == ERROR_NO_EXISTE) {
            cout << "No hay un vehiculo activo con la placa '" << placa << "'.\n";
            return;
        }
        if (busqueda != OK) {
            mostrarErrorArchivo();
            return;
        }
        res = listarOrdenesPorVehiculo(vehiculo.id, &lista);
    } else {
        return;
    }

    if (res != OK) {
        mostrarErrorArchivo();
        return;
    }
    if (lista.cantidad == 0) {
        cout << "No hay ordenes para mostrar.\n";
        return;
    }
    cout << "\n" << left << setw(7) << "ORDEN" << setw(12) << "APERTURA" << setw(28) << "VEHICULO"
         << setw(24) << "MECANICO" << setw(10) << "ESTADO" << right << setw(10) << "TOTAL" << "\n";
    cout << string(91, '-') << "\n";
    for (int i = 0; i < lista.cantidad; i++) mostrarFilaOrden(&lista.items[i]);
    cout << "Total de ordenes: " << lista.cantidad << "\n";
    avisarSiHayMas(lista.hayMas, lista.cantidad);
}

void mostrarFilaOrden(const OrdenServicio* orden) {
    char vehiculo[90], mecanico[130];
    describirVehiculo(orden->idVehiculo, vehiculo, 90);
    describirMecanico(orden->idMecanico, mecanico, 130);
    cout << left << "#" << setw(6) << orden->id << setw(12) << orden->fechaApertura
         << setw(28) << string(vehiculo).substr(0, 27) << setw(24) << string(mecanico).substr(0, 23)
         << setw(10) << orden->estado << right << setw(10) << fixed << setprecision(2)
         << calcularTotalOrden(orden) << "\n" << left;
}

void mostrarDetalleOrden(const OrdenServicio* orden) {
    Vehiculo vehiculo;
    Resultado lecturaVehiculo = obtenerVehiculoPorID(orden->idVehiculo, &vehiculo);
    char mecanico[130];
    describirMecanico(orden->idMecanico, mecanico, 130);

    cout << fixed << setprecision(2);
    cout << "\n+----------------------------------------------------------------+\n";
    cout << "  ORDEN DE SERVICIO #" << orden->id << "\n";
    cout << "+----------------------------------------------------------------+\n";
    cout << "  Estado   : " << left << setw(18) << orden->estado << "Apertura: " << orden->fechaApertura << "\n";
    if (orden->fechaCierre[0] != '\0') {
        cout << "  " << setw(29) << " " << "Cierre  : " << orden->fechaCierre << "\n";
    }
    if (lecturaVehiculo == OK || lecturaVehiculo == ERROR_ELIMINADO) {
        cout << "  Vehiculo : " << vehiculo.placa << " - " << vehiculo.marca << " " << vehiculo.modelo
             << " " << vehiculo.anio << " (" << orden->kilometrajeIngreso << " km)\n";
        cout << "  Dueno    : " << vehiculo.nombrePropietario << "\n";
    } else {
        cout << "  Vehiculo : (ID " << orden->idVehiculo << " no encontrado)\n";
    }
    cout << "  Mecanico : " << mecanico << "\n";
    cout << "  Falla    : " << orden->descripcionFalla << "\n";
    cout << "+----------+---------------------------+------+---------+--------+\n";
    cout << "  " << left << setw(10) << "Codigo" << setw(28) << "Repuesto" << right << setw(5) << "Cant"
         << setw(10) << "P.Unit" << setw(9) << "Total" << "\n";
    cout << "+----------+---------------------------+------+---------+--------+\n";
    for (int i = 0; i < orden->numLineas; i++) {
        const DetalleOrden* linea = &orden->lineas[i];
        Repuesto repuesto;
        Resultado lectura = obtenerRepuestoPorID(linea->idRepuesto, &repuesto);
        bool hay = lectura == OK || lectura == ERROR_ELIMINADO;
        cout << "  " << left << setw(10) << (hay ? repuesto.codigo : "?")
             << setw(28) << (hay ? string(repuesto.nombre).substr(0, 27) : string("(no encontrado)"))
             << right << setw(5) << linea->cantidad << setw(10) << linea->precioUnitario
             << setw(9) << linea->cantidad * linea->precioUnitario << "\n";
    }
    cout << "+----------+---------------------------+------+---------+--------+\n";
    cout << right << setw(55) << "Repuestos" << setw(9) << calcularSubtotalRepuestos(orden) << "\n";
    cout << setw(55) << "Mano de obra" << setw(9) << orden->manoObra << "\n";
    cout << setw(55) << "TOTAL" << setw(9) << calcularTotalOrden(orden) << "\n";
    cout << "+----------------------------------------------------------------+\n" << left;
}
