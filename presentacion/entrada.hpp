#ifndef ENTRADA_HPP
#define ENTRADA_HPP

// entrada.hpp
// Lectura de datos del usuario (viene del Proyecto 1).
// Todas leen lineas completas con getline, nunca con cin >>, para que el
// programa funcione igual escribiendo a mano o con ./taller < pruebas/entrada.txt.
// Las que leen datos devuelven false si el usuario escribe CANCELAR o si se
// acaba la entrada, y el menu que las llama se sale sin guardar nada.

// leerEntero: muestra el mensaje y lee un numero entero en *destino.
// Si escriben texto o un numero mal escrito, avisa y lo vuelve a pedir.
// Devuelve false si escriben CANCELAR o se acaba la entrada.
bool leerEntero(const char* mensaje, int* destino);

// leerFlotante: igual que leerEntero pero con decimales (precios, mano de obra).
bool leerFlotante(const char* mensaje, float* destino);

// leerTexto: lee una linea completa y la copia en destino sin pasarse de
// tamanoDestino (siempre con '\0' al final). Lee primero en un std::string,
// que crece solo, y despues copia al char[].
// Devuelve false si escriben CANCELAR o se acaba la entrada.
bool leerTexto(const char* mensaje, char* destino, int tamanoDestino);

// confirmar: pregunta S/N hasta que respondan una de las dos.
// Devuelve true si responden S (mayuscula o minuscula).
bool confirmar(const char* mensaje);

// leerOpcion: pide una opcion de menu entre minimo y maximo y la repite hasta
// que sea valida. Si se acaba la entrada devuelve minimo - 1 (para menus que
// empiezan en 0 es -1), y los menus usan eso para salir.
int leerOpcion(const char* mensaje, int minimo, int maximo);

#endif
