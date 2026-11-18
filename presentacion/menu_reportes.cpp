#include "menu_reportes.hpp"
#include "menu_ordenes.hpp"
#include "entrada.hpp"
#include "comunes.hpp"
#include "../logica/reportes.hpp"
#include "../logica/utilidades.hpp"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

void menuReportes() {
    int opcion = -1;
    do {
        cout << "\n--- REPORTES ---\n";
        cout << "1. Historial de vehiculo\n";
        cout << "2. Repuestos mas utilizados\n";
        cout << "3. Facturacion por mecanico\n";
        cout << "0. Volver\n";
        opcion = leerOpcion("Seleccione una opcion", 0, 3);

        switch (opcion) {
            case 1: menuHistorialVehiculo(); break;
            case 2: menuRepuestosMasUtilizados(); break;
            case 3: menuFacturacionPorMecanico(); break;
        }
    } while (opcion != 0 && opcion != -1);
}

void menuHistorialVehiculo() {
    char placa[10];
    if (!leerTexto("Placa del vehiculo: ", placa, 10)) return;
    convertirAMayusculas(placa);

    Vehiculo vehiculo;
    ListaOrdenes ordenes;
    float facturado = 0;
    Resultado res = historialVehiculo(placa, &vehiculo, &ordenes, &facturado);
    if (res == ERROR_NO_EXISTE) {
        cout << "No hay un vehiculo activo con la placa '" << placa << "'.\n";
        return;
    }
    if (res != OK) {
        mostrarErrorArchivo();
        return;
    }

    cout << "\n=== HISTORIAL DE " << vehiculo.placa << " ===\n";
    cout << vehiculo.marca << " " << vehiculo.modelo << " " << vehiculo.anio << " | "
         << vehiculo.kilometraje << " km | Dueno: " << vehiculo.nombrePropietario << "\n\n";
    if (ordenes.cantidad == 0) {
        cout << "Este vehiculo no tiene ordenes.\n";
    } else {
        cout << left << setw(7) << "ORDEN" << setw(12) << "APERTURA" << setw(28) << "VEHICULO"
             << setw(24) << "MECANICO" << setw(10) << "ESTADO" << right << setw(10) << "TOTAL" << "\n";
        cout << string(91, '-') << "\n";
        for (int i = 0; i < ordenes.cantidad; i++) mostrarFilaOrden(&ordenes.items[i]);
        avisarSiHayMas(ordenes.hayMas, ordenes.cantidad);
    }
    cout << "Total facturado (solo ordenes CERRADAS): " << fixed << setprecision(2) << facturado << "\n";
}

void menuRepuestosMasUtilizados() {
    TopRepuestos top;
    if (repuestosMasUtilizados(&top) != OK) {
        mostrarErrorArchivo();
        return;
    }
    cout << "\n=== REPUESTOS MAS UTILIZADOS (ordenes CERRADAS) ===\n";
    if (top.cantidad == 0) {
        cout << "Todavia no hay repuestos usados en ordenes cerradas.\n";
        return;
    }
    cout << left << setw(5) << "#" << setw(12) << "CODIGO" << setw(32) << "NOMBRE" << right << setw(9) << "UNIDADES" << "\n";
    cout << string(58, '-') << "\n";
    for (int i = 0; i < top.cantidad; i++) {
        cout << left << setw(5) << i + 1 << setw(12) << top.filas[i].repuesto.codigo
             << setw(32) << string(top.filas[i].repuesto.nombre).substr(0, 31)
             << right << setw(9) << top.filas[i].unidades << "\n";
    }
}

void menuFacturacionPorMecanico() {
    ListaFacturacion lista;
    if (facturacionPorMecanico(&lista) != OK) {
        mostrarErrorArchivo();
        return;
    }
    cout << "\n=== FACTURACION POR MECANICO (ordenes CERRADAS) ===\n";
    if (lista.cantidad == 0) {
        cout << "No hay mecanicos registrados.\n";
        return;
    }
    cout << "+------------------------------+---------+-----------+-----------+\n";
    cout << "| " << left << setw(29) << "Mecanico" << "| " << setw(8) << "Ordenes"
         << "| " << setw(10) << "M. obra" << "| " << setw(10) << "Repuestos" << "|\n";
    cout << "+------------------------------+---------+-----------+-----------+\n";
    for (int i = 0; i < lista.cantidad; i++) {
        FilaFacturacion* f = &lista.filas[i];
        string nombre = f->mecanico.nombre;
        if (f->mecanico.eliminado) nombre += " (BAJA)";
        cout << "| " << left << setw(29) << nombre.substr(0, 28) << "| " << right << setw(7) << f->ordenesCerradas
             << " | " << setw(9) << fixed << setprecision(2) << f->manoObra
             << " | " << setw(9) << f->repuestos << " |\n";
    }
    cout << "+------------------------------+---------+-----------+-----------+\n" << left;
    avisarSiHayMas(lista.hayMas, lista.cantidad);
}
