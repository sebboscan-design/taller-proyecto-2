#ifndef RESULTADOS_HPP
#define RESULTADOS_HPP

// resultados.hpp
// Lo que devuelven las funciones de logica que pueden fallar.
//
// Un bool solo diria si algo fallo. Este enum dice ademas POR QUE fallo, y
// como la logica no puede usar cout, esta es la forma de avisarle al menu
// que mensaje tiene que mostrar.

enum Resultado {
    OK,                               // salio bien
    ERROR_ARCHIVO,                    // no se pudo abrir o escribir un .bin
    ERROR_NO_EXISTE,                  // el ID, placa o codigo no esta registrado
    ERROR_ELIMINADO,                  // existe pero esta eliminado o dado de baja
    ERROR_DATO_INVALIDO,              // un dato esta fuera de lo permitido (precio, anio, fecha...)
    ERROR_DUPLICADO,                  // placa, cedula o codigo ya registrado
    ERROR_ESTADO_INVALIDO,            // la orden no esta en el estado que la operacion necesita
    ERROR_VEHICULO_NO_DISPONIBLE,     // al abrir orden: el vehiculo no existe o esta eliminado
    ERROR_MECANICO_NO_EXISTE,         // al abrir orden: no hay mecanico con ese ID
    ERROR_MECANICO_DE_BAJA,           // al abrir orden: el mecanico esta dado de baja
    ERROR_VEHICULO_CON_ORDEN_ABIERTA, // el vehiculo ya tiene una orden ABIERTA
    ERROR_MECANICO_SIN_CUPO,          // el mecanico ya tiene 3 ordenes ABIERTAS
    ERROR_KILOMETRAJE_MENOR,          // km de ingreso menor al ultimo registrado
    ERROR_STOCK_INSUFICIENTE,         // no hay unidades suficientes del repuesto
    ERROR_LIMITE_LINEAS,              // la orden ya tiene MAX_LINEAS_ORDEN repuestos distintos
    ERROR_REPUESTO_NO_ESTA,           // al quitar: ese repuesto no esta en la orden
    ERROR_TIENE_ORDENES,              // no se puede eliminar o dar de baja por sus ordenes
    ERROR_EN_USO,                     // el repuesto aparece en alguna orden
    ERROR_ORDEN_VACIA                 // cerrar sin repuestos y con mano de obra 0
};

#endif
