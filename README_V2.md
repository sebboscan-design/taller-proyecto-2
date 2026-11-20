# Sistema de gestión de taller - Proyecto 2

Proyecto 2 de Programación 2 (2026-C)

## De qué trata

Es el mismo taller del Proyecto 1 pero ahora los datos no se pierden al cerrar el programa. Todo se guarda en archivos binarios dentro de la carpeta `datos/` y el programa nunca carga un archivo completo: cuando necesita un registro calcula en qué byte está, salta ahí con `seekg`/`seekp`, lee o escribe solo ese registro y cierra el archivo.

`SistemaTaller` y los arreglos dinámicos ya no existen. Tampoco hay `new` ni `delete[]` en ninguna parte del programa.

## Integrantes

- Sebastián Boscán - C.I. 32.741.341
- Camila Velásquez - C.I. (cédula)

## Cómo está organizado

Ahora son tres capas:

- `persistencia/`: lee y escribe bytes en los `.bin`. No tiene cout ni reglas de negocio, por ejemplo no decide si un registro eliminado sirve o no, solo lo entrega.
- `logica/`: las reglas del taller (las mismas del Proyecto 1 más las nuevas). Usa la persistencia para leer y escribir.
- `presentacion/`: los menús. Le pide cosas a la lógica y muestra los resultados. No abre ningún archivo.

Aparte están `main.cpp` (arranque y menú principal) y `estructuras.hpp` con los structs.

Los headers usan la extensión `.hpp` (en vez de `.h`) para dejar claro que son de C++, y cada uno tiene documentado arriba de cada función qué hace, qué recibe y qué devuelve.

## Compilar y correr

Hace falta g++ con soporte de C++17 (por `filesystem`, que solo se usa para crear las carpetas `datos/` y del backup). Desde la carpeta del proyecto:

```
g++ -std=c++17 main.cpp logica/*.cpp persistencia/*.cpp presentacion/*.cpp -o taller
./taller
```

La primera vez que se corre crea `datos/` con los archivos vacíos y pide los datos del taller. Las siguientes veces solo los lee.

### Pruebas

Los casos de uso piden cerrar y volver a abrir el programa, así que cada ejecución tiene su archivo: `entrada_3a.txt`, `entrada_3b.txt`, etc., con su `salida_...` correspondiente. Se corren en orden empezando sin datos. Para correrlas todas de una vez:

```
bash pruebas/correr_pruebas.sh                                    (Git Bash / Linux)
powershell -ExecutionPolicy Bypass -File pruebas\correr_pruebas.ps1   (PowerShell)
```

Ojo: el script borra `datos/` antes de empezar. Al final `datos/` queda con los datos pre-poblados que pide la entrega.

| Ejecución | Qué hace |
|---|---|
| 1a | Crea el taller, registra 3 vehículos, 2 mecánicos y 4 repuestos |
| 1b | Vuelve a abrir y lista todo: siguen los mismos datos con los mismos IDs |
| 2 | Registra 3 vehículos más (quedan los IDs 1 a 6), elimina el 3, busca el 5 por ID mostrando el byte donde se leyó, busca el 3 y avisa que está eliminado |
| 3a | Abre la orden #1 y le agrega 3 repuestos distintos |
| 3b | Agrega un 4to repuesto y 2 unidades más de las pastillas (se acumulan), quita el líquido de frenos (su stock vuelve) y cierra con 40 de mano de obra |
| 3c | Vuelve a abrir y muestra la orden #1 con sus líneas, precios y total 208.00 |
| 4a | Registra 12 repuestos más, abre la orden #2 y le agrega 15 repuestos distintos. El 16 se rechaza por el límite |
| 4b | Vuelve a abrir y la orden #2 sigue con sus 15 líneas |
| 5a | Lista el stock y anula la orden #1 (CERRADA, 3 líneas) |
| 5b | Vuelve a abrir: el stock de esos 3 repuestos volvió a su valor de antes |
| 6 | Intenta dar de baja a Ana con la orden #2 abierta (rechazado), la cierra, la da de baja (aceptado), sus órdenes cerradas siguen mostrando su nombre y una orden nueva con ella se rechaza |
| 7a | Crea un backup, registra el vehículo PQ321RS y le abre la orden #3 |
| (manual) | Se reemplaza `datos/vehiculos.bin` por el del backup (el script lo hace solo) |
| 7b | La verificación de integridad detecta que la orden #3 apunta al vehículo ID 7, que ya no existe |
| 8 | Se vuelve a registrar el vehículo perdido (vuelve a tomar el ID 7), se registran 2 vehículos y 2 mecánicos más, se completan 10 órdenes entre los 3 estados y 4 mecánicos, y se sacan los 3 reportes. Al final la integridad da 0 problemas |

`experimento_puntero.cpp` es el experimento de la pregunta 1 de análisis.

## Tamaños y posición de los registros

Medidos con `sizeof` en una compu de 64 bits (donde `time_t` ocupa 8 bytes):

| Estructura | sizeof |
|---|---|
| ArchivoHeader | 16 |
| Taller | 360 |
| Vehiculo | 288 |
| Mecanico | 176 |
| Repuesto | 208 |
| DetalleOrden | 12 |
| OrdenServicio | 464 |

Algunos tamaños son un poco más grandes que la suma de sus campos porque el compilador mete bytes de relleno para alinear los `int` y los `time_t`.

La posición de un registro con índice físico `i` (0, 1, 2...) es:

```
posicion = sizeof(ArchivoHeader) + i * sizeof(Estructura)
```

Por ejemplo, el vehículo con ID 5 está en `16 + 4 * 288 = 1168`, que es lo que muestra el mensaje de depuración del caso 2.

`taller.bin` no tiene header, es solo un `Taller` de 360 bytes.

## Relación entre ID e índice físico

Los IDs empiezan en 1 y se dan en el orden en que se agregan, los registros siempre se agregan al final y nunca se mueven ni se borran físicamente (el borrado es lógico). Entonces el primer registro (índice 0) tiene ID 1, el segundo ID 2, y así:

```
indice = id - 1
```

Por eso leer por ID no recorre nada: se calcula `16 + (id - 1) * sizeof(...)` y se salta directo. Esto lo hace `posicionRegistro()` en `persistencia/archivos.cpp`.

Esto dejaría de servir si cambiara cualquiera de esas condiciones. Por ejemplo, si se borraran registros físicamente y se corrieran los de atrás para tapar el hueco (como en el Proyecto 1), o si se reutilizaran los IDs de los eliminados, o si los IDs empezaran en otro número o se saltaran. En cualquiera de esos casos el ID ya no diría en qué posición está el registro y habría que recorrer el archivo para encontrarlo.

## Diseño del detalle de la orden

### Lo que hicimos

Cada orden lleva adentro un arreglo fijo de 15 líneas y un contador:

```cpp
const int MAX_LINEAS_ORDEN = 15;

struct DetalleOrden {        // 12 bytes
    int idRepuesto;
    int cantidad;
    float precioUnitario;    // precio del repuesto cuando se agrego
};

struct OrdenServicio {       // 464 bytes
    ...
    DetalleOrden lineas[MAX_LINEAS_ORDEN]; // 15 * 12 = 180 bytes
    int numLineas;                         // cuantas lineas valen
    ...
};
```

Así todas las órdenes miden exactamente lo mismo, se siguen ubicando por cálculo, y con un solo `read` se trae la orden completa con todas sus líneas sin tocar las demás órdenes. Tampoco hace falta un archivo extra.

### Cuánto ocupa

Como el arreglo es fijo, **una orden ocupa 464 bytes en disco tenga 1 línea o 15**. De esos, el detalle son 184 bytes (180 del arreglo y 4 de `numLineas`).

- Con 1 línea se usan 12 de los 180 bytes del arreglo y se desperdician 168 (más o menos el 36% de la orden).
- Con 15 líneas no se desperdicia nada.

Ese es el precio de que todo mida lo mismo. Con 10 órdenes el archivo pesa 4656 bytes, así que el desperdicio no es un problema para un taller.

### Alternativa que descartamos

Pensamos en un archivo aparte `detalles.bin` donde cada línea fuera un registro propio con el ID de su orden (`idOrden, idRepuesto, cantidad, precio, eliminado`). Así no se desperdicia espacio porque cada orden usa solo las líneas que tiene.

La descartamos porque las líneas de una orden quedan regadas por todo el archivo (se van guardando a medida que se agregan, mezcladas con las de otras órdenes). Para ver las líneas de una orden habría que recorrer todo `detalles.bin`, y eso va en contra del requisito 4 y de la idea de acceso aleatorio. Además serían más escrituras por operación (más puntos donde el programa se puede cortar), un header más, y más cosas que meter en el backup y en la integridad.

También pensamos en guardar en `detalles.bin` un bloque fijo de 15 líneas por orden en la misma posición que la orden. Eso sí permite acceso directo, pero ocupa lo mismo que nuestro diseño y obliga a escribir en dos archivos cada vez, así que no ganábamos nada.

### Quitar una línea

Se busca la línea del repuesto dentro de `lineas[]`, se corren una posición hacia atrás todas las que vienen después para tapar el hueco (igual que eliminar en el Proyecto 1), se resta 1 a `numLineas` y la última posición se pone en cero. Después se sobrescribe la orden completa en su misma posición y se devuelven las unidades al repuesto.

Agregar un repuesto que ya estaba suma a su línea y mantiene el precio que tenía. Si no estaba y ya hay 15 líneas, se rechaza con `ERROR_LIMITE_LINEAS`.

## Operaciones que escriben en varios archivos

Todas validan todo antes de la primera escritura. Si algo falla, no se escribe nada. Elegimos el orden de las escrituras con una regla: **si el programa se corta en medio, el stock nunca puede quedar más alto de lo real**. Un stock más bajo de lo real se arregla contando el almacén; uno más alto hace que el sistema venda repuestos que no existen.

| Operación | Primero escribe | Después escribe |
|---|---|---|
| Abrir orden | la orden en `ordenes.bin` | el km del vehículo en `vehiculos.bin` |
| Agregar repuesto | el stock (baja) en `repuestos.bin` | la línea en `ordenes.bin` |
| Quitar repuesto | la orden sin la línea | el stock (sube) en `repuestos.bin` |
| Cerrar orden | solo la orden | - |
| Anular orden | la orden como ANULADA | el stock de cada línea |

Y dentro de un mismo archivo, al crear un registro se escribe primero el registro y después el header. Si se corta en medio, el header queda atrasado y `verificarIntegridad()` lo detecta.

## Firmas que diseñamos

Las reglas que seguimos en todas:

- Ninguna función devuelve un puntero a un registro, porque ya no hay un arreglo en memoria al que apuntar. La que lee algo recibe un `destino` donde deja una **copia** del registro.
- La persistencia avisa con `bool` (salió bien o no) o con `ResultadoArchivo` cuando hay dos formas de fallar: `ARCHIVO_ID_NO_EXISTE` o `ARCHIVO_ERROR` (no se pudo abrir el archivo). Cuando falla al abrir un archivo guarda cuál fue, y así el mensaje puede decir qué archivo falló.
- La lógica devuelve `Resultado` (el enum de `logica/resultados.hpp`), que dice exactamente por qué falló algo: `ERROR_NO_EXISTE`, `ERROR_ELIMINADO`, `ERROR_STOCK_INSUFICIENTE`, `ERROR_LIMITE_LINEAS`, etc.
- Cuando el registro existe pero está eliminado, las funciones `obtener...PorID` devuelven `ERROR_ELIMINADO` pero igual llenan `destino`. Así el menú puede decir "fue eliminado" en vez de "no existe", y las órdenes viejas pueden mostrar el nombre de un mecánico dado de baja.
- Las búsquedas de varios llenan una lista de tamaño fijo (`ListaVehiculos`, `ListaOrdenes`...) con máximo `MAX_RESULTADOS = 20` y un `bool hayMas` para que el menú avise si quedaron más. Como son arreglos fijos no hay `new` ni `delete[]`.
- Los IDs nuevos se devuelven por un parámetro `int*` (por ejemplo `idNuevo`), porque el return ya se usa para el `Resultado`.

### Persistencia

Del header (sirven para cualquier archivo):

```cpp
bool inicializarArchivo(const char* ruta);           // lo crea si no existe; true si quedo listo
bool leerHeader(const char* ruta, ArchivoHeader* destino);
bool actualizarHeader(const char* ruta, ArchivoHeader header);   // solo escribe en la posicion 0
long posicionRegistro(int id, int tamanoRegistro);
```

Las cinco operaciones, iguales para las cuatro entidades (aquí las de vehículos; las otras son `guardarMecanicoNuevo`, `leerRepuesto`, `sobrescribirOrden`, etc.):

```cpp
bool guardarVehiculoNuevo(Vehiculo* vehiculo);       // le pone el ID y las fechas; el ID queda en vehiculo->id
ResultadoArchivo leerVehiculo(int id, Vehiculo* destino);   // por calculo, aunque este eliminado
bool sobrescribirVehiculo(Vehiculo* vehiculo);       // en su misma posicion, actualiza fechaUltimaModificacion
bool marcarVehiculoEliminado(int id);                // eliminado = true y registrosActivos--
int contarVehiculosActivos();                        // desde el header; -1 si falla el archivo
int totalVehiculos();                                // total historico, para recorrer los IDs del 1 al total
```

`guardar...Nuevo` recibe un puntero para que quien llama vea el ID que se le asignó. `leer...` devuelve `ResultadoArchivo` porque puede fallar por dos motivos distintos. Las que cuentan devuelven -1 cuando falla el archivo porque un conteo nunca es negativo.

Y para `taller.bin`: `existeTaller()`, `leerTaller(Taller*)` y `guardarTaller(Taller*)`.

### Lógica: vehículos

```cpp
Resultado registrarVehiculo(Vehiculo datos, int* idNuevo);
Resultado obtenerVehiculoPorID(int id, Vehiculo* destino);
Resultado obtenerVehiculoPorPlaca(const char* placa, Vehiculo* destino);
Resultado buscarVehiculosPorPropietario(const char* texto, ListaVehiculos* resultado);
Resultado listarVehiculos(ListaVehiculos* resultado);
Resultado actualizarVehiculo(int id, Vehiculo datos);
Resultado eliminarVehiculo(int id, int* ordenesEncontradas);
long posicionVehiculoEnArchivo(int id);   // para el mensaje de depuracion del caso 2
```

`registrarVehiculo` puede fallar por placa repetida (solo cuenta entre los activos) o por datos malos. `eliminarVehiculo` devuelve `ERROR_TIENE_ORDENES` y cuántas tiene en `ordenesEncontradas`, así el mensaje puede decir "tiene 3 órdenes".

### Lógica: mecánicos

```cpp
Resultado registrarMecanico(Mecanico datos, int* idNuevo);
Resultado obtenerMecanicoPorID(int id, Mecanico* destino);
Resultado listarMecanicos(ListaMecanicos* resultado);   // activos y de baja, con sus ordenes abiertas
Resultado darDeBajaMecanico(int id, int* ordenesAbiertas);
```

`darDeBajaMecanico` es nueva: devuelve `ERROR_TIENE_ORDENES` si tiene órdenes ABIERTAS, y `ERROR_ELIMINADO` si ya estaba de baja. `ListaMecanicos` trae al lado de cada mecánico cuántas órdenes abiertas tiene.

### Lógica: repuestos

```cpp
Resultado registrarRepuesto(Repuesto datos, int* idNuevo);
Resultado obtenerRepuestoPorID(int id, Repuesto* destino);
Resultado obtenerRepuestoPorCodigo(const char* codigo, Repuesto* destino);
Resultado buscarRepuestos(const char* texto, ListaRepuestos* resultado);
Resultado listarRepuestos(ListaRepuestos* resultado);
Resultado registrarEntradaMercancia(int idRepuesto, int cantidad);
Resultado cambiarPrecioRepuesto(int idRepuesto, float nuevoPrecio);
Resultado eliminarRepuesto(int idRepuesto);
bool tieneBajoStock(const Repuesto* repuesto);
```

`eliminarRepuesto` devuelve `ERROR_EN_USO` si el repuesto sale en alguna orden. `tieneBajoStock` devuelve bool porque solo compara dos números y no puede fallar.

### Lógica: órdenes

```cpp
Resultado abrirOrden(int idVehiculo, int idMecanico, int kilometrajeIngreso,
                     const char* descripcionFalla, int* idNuevaOrden);
Resultado agregarRepuestoAOrden(int idOrden, int idRepuesto, int cantidad);
Resultado quitarRepuestoDeOrden(int idOrden, int idRepuesto);
Resultado cerrarOrden(int idOrden, float manoObra);
Resultado anularOrden(int idOrden);
float calcularSubtotalRepuestos(const OrdenServicio* orden);
float calcularTotalOrden(const OrdenServicio* orden);
Resultado obtenerOrdenPorID(int id, OrdenServicio* destino);
Resultado listarOrdenes(ListaOrdenes* resultado);
Resultado listarOrdenesPorEstado(const char* estado, ListaOrdenes* resultado);
Resultado listarOrdenesPorVehiculo(int idVehiculo, ListaOrdenes* resultado);
Resultado buscarOrdenAbiertaDeVehiculo(int idVehiculo, int* idOrden);
int contarOrdenesDeVehiculo(int idVehiculo);
int contarOrdenesAbiertasMecanico(int idMecanico);
int contarOrdenesConRepuesto(int idRepuesto);
```

`abrirOrden` es la que más motivos de fallo tiene, por eso hay códigos separados: `ERROR_VEHICULO_NO_DISPONIBLE`, `ERROR_MECANICO_NO_EXISTE`, `ERROR_MECANICO_DE_BAJA`, `ERROR_VEHICULO_CON_ORDEN_ABIERTA`, `ERROR_MECANICO_SIN_CUPO` y `ERROR_KILOMETRAJE_MENOR`. Las de agregar, quitar y cerrar devuelven `ERROR_ESTADO_INVALIDO` si la orden no está ABIERTA, y el menú muestra en qué estado está. Los cálculos de total reciben la orden ya leída y devuelven un `float` porque no pueden fallar. Las de contar devuelven -1 si falla el archivo.

### Mantenimiento y reportes

```cpp
Resultado verificarIntegridad(ReporteIntegridad* reporte);
Resultado crearBackup(char* rutaCreada, int tamanoRuta);
Resultado historialVehiculo(const char* placa, Vehiculo* vehiculo, ListaOrdenes* ordenes, float* totalFacturado);
Resultado repuestosMasUtilizados(TopRepuestos* top);
Resultado facturacionPorMecanico(ListaFacturacion* resultado);
```

`verificarIntegridad` llena un struct con los conteos y los mensajes de cada problema ("ROTO: ..."), y la presentación los imprime. Lee los archivos registro por registro sin confiar en el header, porque justamente uno de los chequeos es ver si el header coincide con lo que de verdad hay.

Los reportes también leen un registro a la vez. El de repuestos más usados no guarda todos los repuestos en memoria: por cada repuesto cuenta sus unidades en las órdenes cerradas y lo mete en el top de 10 solo si le toca.

## Preguntas de análisis

**1. Escriban con write una OrdenServicio del Proyecto 1 tal cual (con su DetalleOrden\*), cierren el programa, vuelvan a ejecutarlo y lean ese registro. ¿Qué observaron al intentar acceder a sus líneas? ¿Por qué ocurre?**

Lo probamos con `pruebas/experimento_puntero.cpp`. La primera ejecución escribe la orden y la segunda la lee. El id, el estado y `numDetalles` salen bien, pero el puntero `detalles` trae una dirección de la ejecución anterior y al intentar leer la primera línea el programa se cae (violación de segmento). Otras veces podría mostrar basura.

Pasa porque `write` copia los bytes del struct tal cual, y dentro del struct no están las líneas, solo el puntero, que es un número con la dirección de memoria donde estaban. Esa memoria era del programa anterior y se liberó cuando terminó. El programa nuevo tiene otra memoria, así que esa dirección no apunta a nada válido. O sea, las líneas nunca llegaron al archivo. Por eso un struct que se guarda en disco no puede tener punteros.

**2. Al agregar un repuesto a una orden se escriben el detalle y el stock. Si el programa se interrumpe entre esas dos escrituras, ¿qué inconsistencia queda según el orden que eligieron? ¿Qué orden de escritura deja el daño más fácil de detectar o corregir, y por qué?**

Nosotros escribimos primero el stock (lo bajamos) y después la línea en la orden. Si se corta entre las dos, el repuesto queda con menos stock pero la orden no tiene la línea, o sea, hay unidades que "desaparecieron" del inventario sin estar en ninguna orden.

Si fuera al revés (primero la línea y después el stock), la orden tendría el repuesto pero el stock no se habría descontado, y el sistema diría que hay más unidades de las que hay de verdad. Eso es peor, porque se podrían agregar a otras órdenes repuestos que ya no están en el almacén.

Por eso elegimos bajar el stock primero: el daño es que el inventario queda por debajo, y eso se detecta y se corrige contando lo que hay físicamente y registrando una entrada de mercancía. Un inventario inflado es más difícil de notar porque el sistema no se queja hasta que alguien va a buscar el repuesto y no está. Con la misma idea, al quitar una línea o anular una orden primero se modifica la orden y después se devuelve el stock.

**3. En el Proyecto 1 no se podía eliminar un mecánico con órdenes. ¿Qué habría pasado con el historial si se hubiera permitido la eliminación física? ¿Por qué la baja lógica lo resuelve?**

Las órdenes solo guardan el `idMecanico`, no el nombre. Si el mecánico se borrara físicamente, sus órdenes quedarían apuntando a un ID que ya no existe y no se podría saber quién hizo el trabajo. Y en el Proyecto 1, al borrar se corrían los de atrás en el arreglo, así que podía pasar algo peor: que una búsqueda devolviera a otro mecánico y las órdenes viejas mostraran el nombre equivocado. También se perderían los datos para la facturación por mecánico.

Con la baja lógica el registro sigue en el archivo en su misma posición, solo se marca `eliminado = true`. Las órdenes cerradas y anuladas lo siguen encontrando por su ID y muestran su nombre (con "(BAJA)" al lado), y los reportes lo incluyen. Lo único que cambia es que la lógica no lo deja recibir órdenes nuevas.
