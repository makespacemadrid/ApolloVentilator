**Tracking del proyecto**

- Todas las tareas definidas deberían aparecer en el **Project** correspondiente resumiendo el requerimiento u objetivo.  
- Una vez clarificado, la tarjeta se puede transformar en un issue (Botón derecho en la tarjeta), permitiendo asignarle 
  responsables, milestone (sprint) y creando una rama asociada. 

**Trabajo por ramas**  

- La rama debe ir asociada a un issue, eso nos permite tener contexto de la feature + discusión en un único punto (Issues).
- Permite que otras personas colaboren al poder seguir el trabajo de uno de los miembros.
- Para facilitar el flujo de trabajo a quien vaya a comenzar, que la rama master se encuentre en un punto estable.
  - Si hay dependencias entre tareas se puede hacer branch desde la rama de la tarea padre.
 
**Features, Bugs, Etc ...**  

- Utilizar un naming común para conocer lo que se está trabajando en una rama.  

> feature/gui-make-graph-faster  
> bugfix/remove-gtk  

**Pull Request**

- Intentemos que la historia del repositorio tenga sentido

> Formato de ejemplo:  
```markdown
Issue  
- Funcionamiento del bucle de draw en interfaz QT  

Objetivo  
- Visualizar correctamente una gráfica que pinte valores que se obtienen dinámicamente (puerto serie o tty emulado).

Estado  
- La gráfica se pinta correctamente pero a los 10 segundos aparece cthulhu y te pega

Changelog  
- Añadido control QTChart
- Link Evento Serie con metodo redraw
``` 

- Antes de hacer pull request se puede hacer squash (merge) de los commits en uno solo, editando el mensaje resultante.  
> git log // Contabilizas los commits que has producido en tu rama (ej. 5)  
> git rebase -i HEAD~5  
> // Cambias todos los commits que aparecen menos el primero (borrar pick y escribir squash o 's')
> También se puede hacer en la PR

- En la PR, si se utiliza la opción de Squash y --no-ff se puede mergear el commit sin añadir commits de merge al historico.

4. Documentación

- Una vez cerrada la issue, añadir la info de interes a la wiki o referenciar a la issue si no hay tiempo.
