# Proyecto 1 - Base de Datos 2 (CS2042)

## Tabla de Contenidos

- [Proyecto 1 - Base de Datos 2 (CS2042)](#proyecto-1---base-de-datos-2-cs2042)
  - [Tabla de Contenidos](#tabla-de-contenidos)
  - [Resumen](#resumen)
  - [Introducción](#introducción)
    - [Objetivos del proyecto](#objetivos-del-proyecto)
    - [Descripción del dominio de datos](#descripción-del-dominio-de-datos)
    - [Resultados esperados con indexación](#resultados-esperados-con-indexación)
  - [Técnicas utilizadas](#técnicas-utilizadas)
    - [Técnicas de indexación de archivos utilizadas](#técnicas-de-indexación-de-archivos-utilizadas)
    - [Inserción, eliminación y búsqueda](#inserción-eliminación-y-búsqueda)
  - [Implementación del Parser SQL](#implementación-del-parser-sql)
    - [¿Por qué usar Expresiones Regulares?](#¿por-qué-usar-expresiones-regulares?)
    - [Ejemplos de Secuencias SQL](#ejemplos-de-secuencias-sql)
    - [Justificación de uso](#justificación-de-uso)
  - [Autores](#autores)
  - [Referencias](#referencias)

## Resumen
El presente proyecto consta en la implementación de un sistema de gestión de base de datos utilizando 5 técnicas de organización de archivos en memoria secundaria: AVL File, Sequential File, ISAM Sparse Index, B+ Tree y Extendible Hash, todos escritos en C++. Además, se implementó un parser de SQL para la ejecución de consultas. Asimismo, se creó una API en C++ y una UI en Nextjs para consumir la API, ambos deployeados en la nube.

## Introducción
En el ámbito de las bases de datos, las técnicas de organización de archivos desempeñan un papel crucial en la optimización del rendimiento y la gestión eficiente de la memoria secundaria. Estas estructuras de datos están diseñadas para almacenar y recuperar grandes volúmenes de información de manera rápida y eficiente, mejorando significativamente la velocidad de consulta y la capacidad de escalabilidad del sistema. Además, permiten una mejor utilización de los recursos físicos y una gestión más efectiva del espacio en disco.

### Objetivos del proyecto
+ Implementar 5 técnicas de organización de archivos en memoria secundaria: AVL File, Sequential File, ISAM Sparse Index, B+ Tree y Extendible Hash.
+ Desarrollar un parser de SQL para la ejecución de consultas.
+ Implementar una interfaz gráfica funcional con herramientas de frontend, backend y cloud computing.
+ Realizar un análisis comparativo de las técnicas de organización de archivos implementadas.
### Descripción del dominio de datos

El dominio de datos utilizado en este proyecto corresponde a un conjunto de registros de aplicaciones móviles de la App Store. Cada registro contiene información detallada sobre una aplicación, incluyendo su nombre, categoría, precio, calificación, entre otros. Para este proyecto, se considera una base de datos de aplicaciones móviles con los siguientes campos:

- `id`: Identificador único de la aplicación.
- `track_name`: Nombre de la aplicación.
- `size_bytes`: Tamaño en bytes de la aplicación.
- `currency`: Moneda utilizada para el precio.
- `price`: Precio de la aplicación.
- `rating_count_tot`: Número total de calificaciones.
- `rating_count_ver`: Número de calificaciones de la versión actual.
- `user_rating`: Calificación promedio de los usuarios.
- `user_rating_ver`: Calificación promedio de la versión actual.
- `ver`: Versión de la aplicación.
- `cont_rating`: Clasificación por contenido.
- `prime_genre`: Género principal de la aplicación.
- `sup_devices.num`: Número de dispositivos compatibles.
- `ipadSc_urls.num`: Número de capturas de pantalla para visualización.
- `lang.num`: Número de idiomas soportados.
- `vpp_lic`: Licencia VPP.

### Resultados esperados con indexación

La implementación de las técnicas de organización de archivos en memoria secundaria permitirá mejorar la eficiencia y el rendimiento de las consultas SQL en la base de datos de aplicaciones móviles. Al utilizar índices y estructuras de datos optimizadas, se espera reducir el tiempo de acceso a los registros, mejorar la velocidad de búsqueda y recuperación de información, y optimizar el uso de la memoria secundaria. Además, se espera que la implementación de un parser SQL permita ejecutar consultas de manera eficiente y precisa, facilitando la interacción con la base de datos y mejorando la experiencia del usuario.

## Técnicas utilizadas
### Técnicas de indexación de archivos utilizadas
### Inserción, eliminación y búsqueda

## Implementación del Parser SQL

La implementación de un parser SQL en C++ utilizando expresiones regulares completas es una estrategia eficiente y efectiva para analizar y procesar consultas SQL. Las expresiones regulares, también conocidas como regex, son una poderosa herramienta para el manejo de cadenas de texto que permiten buscar, encontrar, y manipular texto de manera flexible y eficiente.

### ¿Por qué usar Expresiones Regulares?

Las expresiones regulares ofrecen una gran flexibilidad y precisión al momento de reconocer patrones en las cadenas de texto. En el caso de un parser SQL, las expresiones regulares permiten identificar de manera precisa los diferentes componentes de una consulta SQL, como los comandos `CREATE`, `SELECT`, `INSERT`, `DELETE`, entre otros, así como los parámetros y argumentos asociados a estos comandos.

Por ejemplo, podemos considerar la siguiente secuencia SQL:

```sql
CREATE TABLE apple_store FROM FILE “C:\appleStore.csv” USING AVL INDEX ("id")
```

Una expresión regular bien diseñada puede identificar de manera precisa el comando `CREATE TABLE`, el nombre de la tabla `apple_store`, la ruta del archivo `C:\appleStore.csv`, y el índice hash `id`.

### Ejemplos de Secuencias SQL

```sql
SELECT * FROM apple_store WHERE id = 342352359
SELECT * FROM apple_store WHERE id BETWEEN 30000000 AND 50000000
INSERT INTO apple_store VALUES (30000000, "YAPE", 1000000, "USD", 0.0, 1000000, 1000000, 5.0, 5.0, "1.0", "4+", "Social Networking", 37, 5, 1, 1)
DELETE FROM apple_store WHERE id = 342352359
```

### Justificación de uso

El uso de expresiones regulares en la implementación de un parser SQL es una práctica común en la industria del software debido a su eficiencia y precisión. Según Nievergelt (2015), las expresiones regulares son una herramienta fundamental en el análisis de texto y se utilizan ampliamente en la implementación de compiladores e intérpretes, incluyendo los parsers SQL.

## Análisis comparativo

[//]: # (Gráficas, considerar dos métricas: total de accesos a disco duro {read & write} y tiempo de ejecución en milisegundos) 

### Inserción
### Búsqueda
### Eliminación

### Resultados experimentales

[//]: # (Discusión de los resultados experimentales )

## Pruebas de uso y presentación
### Interfaz gráfica



[//]: # (Link al frontend en la nube)
[//]: # ([Google]&#40;https://www.google.com&#41;)

### Demostración de funcionalidades

[//]: # (Link de youtube a la grabación )
[//]: # ([Google]&#40;https://www.google.com&#41;)

## Autores

| **Benjamin Soto** | **Edgar Chambilla** | **Fabrizzio Vilchez** | **Ian Gonzales** | **Jeffrey Monja** |
|:------------:|:------------:|:------------:|:------------:|:------------:|
| ![Benjamin Soto](https://avatars.githubusercontent.com/u/104233590?v=4) | ![Edgar Chambilla](https://avatars.githubusercontent.com/u/39739752?v=4) | ![Fabrizzio Vilchez](https://avatars.githubusercontent.com/u/115495332?v=4) | ![Ian Gonzales](https://avatars.githubusercontent.com/u/122047216?v=4) | ![Jeffrey Monja](https://avatars.githubusercontent.com/u/104637993?v=4) |
| [https://github.com/SotoBenjamin](https://github.com/SotoBenjamin) | [https://github.com/Edgar5377](https://github.com/Edgar5377) | [https://github.com/Fabrizzio20k](https://github.com/Fabrizzio20k) | [https://github.com/mukanjy0](https://github.com/mukanjy0) | [https://github.com/jeffreymonjacastro](https://github.com/jeffreymonjacastro) |

## Referencias

- Nievergelt, Y. (2015). Logic, mathematics, and computer science: modern foundations with practical applications. Springer.