#ifndef COMUNES_HPP
#define COMUNES_HPP

// comunes.hpp
// Funciones que usan todos los menus para mostrar cosas.

// mostrarErrorArchivo: mensaje para cuando la logica devuelve ERROR_ARCHIVO.
// Dice cual archivo fallo (lo saca de archivoConError()).
void mostrarErrorArchivo();

// avisarSiHayMas: si hayMas es true, avisa que solo se mostraron los primeros
// "mostrados" resultados de una busqueda.
void avisarSiHayMas(bool hayMas, int mostrados);

// describirVehiculo / describirMecanico / describirRepuesto:
// escriben en destino un texto legible para un ID, porque el enunciado pide
// que los IDs nunca se muestren solos. Ejemplos:
//   vehiculo -> "AB123CD - Toyota Corolla"
//   mecanico -> "Jose Perez" o "Ana Villalobos (BAJA)"
//   repuesto -> "FRN-001 - Pastillas delanteras"
// Si el ID no existe escriben "(... ID n no encontrado)".
void describirVehiculo(int id, char* destino, int tamano);
void describirMecanico(int id, char* destino, int tamano);
void describirRepuesto(int id, char* destino, int tamano);

#endif
