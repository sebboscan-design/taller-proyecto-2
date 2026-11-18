#ifndef MENU_MANTENIMIENTO_HPP
#define MENU_MANTENIMIENTO_HPP

// menu_mantenimiento.hpp
// Menu de mantenimiento: integridad y backup.

// menuMantenimiento: muestra el submenu y repite hasta que elijan 0.
void menuMantenimiento();

// menuVerificarIntegridad: imprime cada problema encontrado y el recuadro de resumen.
void menuVerificarIntegridad();

// menuCrearBackup: crea el backup y dice en que carpeta quedo.
void menuCrearBackup();

#endif
