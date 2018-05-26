## TAREA 3

### Integrantes:

- José Manuel Comber Nro. Alumno: 14632675
- Felipe Vandeputte Nro. Alumno: 1463225J

La principal decisión de diseño fue consolidar el disco en el momento en que se hace cada write, en vez de hacerlo cuando se hace *cz_close*. Para el bloque no se ocupó un struct, sino un simple arreglo de unsigned chars. Las estructuras restantes fueron bastante simples, y no hay mayores originalidades ahí.