#ifndef UTILIDADES_HPP
#define UTILIDADES_HPP

// utilidades.hpp
// Herramientas chicas que usan los modulos de logica (vienen del Proyecto 1).
// Ninguna usa cin ni cout ni toca archivos.

// copiarTexto: copia origen en destino sin pasarse del tamano de destino.
// Siempre deja el '\0' al final, aunque el texto no quepa completo.
void copiarTexto(char* destino, const char* origen, int tamanoDestino);

// convertirAMayusculas: cambia el texto a mayusculas en el mismo arreglo.
// Se usa para que "ab123cd" y "AB123CD" cuenten como la misma placa.
void convertirAMayusculas(char* texto);

// esTextoVacio: true si el texto no tiene nada o solo tiene espacios.
bool esTextoVacio(const char* texto);

// contieneSinMayusculas: true si subcadena aparece dentro de texto, sin
// importar mayusculas. Con subcadena vacia devuelve true (sirve como "todos").
bool contieneSinMayusculas(const char* texto, const char* subcadena);

// obtenerFechaActual: escribe la fecha de hoy como AAAA-MM-DD.
// destino debe tener al menos 11 caracteres.
void obtenerFechaActual(char* destino);

// esFechaValida: true si la fecha es AAAA-MM-DD y el dia existe en ese mes
// (revisa bisiestos).
bool esFechaValida(const char* fecha);

// esPlacaValida: true si tiene 6 o 7 caracteres, solo letras y numeros.
bool esPlacaValida(const char* placa);

// esCedulaValida: true si son solo numeros, o numeros con prefijo V-, E- o J-.
bool esCedulaValida(const char* cedula);

// esEspecialidadValida: true si es MOTOR, FRENOS, ELECTRICIDAD, SUSPENSION o GENERAL.
bool esEspecialidadValida(const char* especialidad);

#endif
