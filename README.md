# ApolloVentilator


ApolloVentillator es un proyecto colaborativo para la construcción de equipos de respiración asistida libres que surge a raíz de la crisis del COVID-19. Su misión es apoyar a los equipos sanitarios aportando respiradores en momentos de escasez de estos dispositivos.

Los objetivos de este proyecto son:

- Durabilidad. Debe permitir ser usado como apoyo a la respiración del paciente de manera continuada durante al menos 2 semanas.
- Seguridad. Debe cumplir los requisitos de calidad necesarios para poder ser utilizado de forma segura.
- Replicabilidad. Debe poder ser replicados con facilidad.Los materiales deben ser fáciles de conseguir y su montaje debe ser sencillo y rápido.

Al ser un proyecto en desarrollo continuo gran parte de la información se puede encontrar en nuestra [wiki](https://github.com/makespacemadrid/ApolloVentilator/wiki).

El código se libera bajo la GPL 3. [Ver [LICENSE](LICENSE)] 


## ¿Como colaborar?

Actualmente nos encontramos agregando información en la wiki y trabajando en un prototipo.

Puedes encontrar las notas de colaboración en [COLLABORATION](man/COLLABORATION.md).  


## Requerimientos

### Software

#### Linux

```
$ apt-get install libcairo2-dev libgirepository1.0-dev python3-cairo-dev
```

#### Mac

```
$ brew install pygobject3 gtk+3 libnotify
```

### Firmware
> TO DO: Requerimientos para cargar firmware (Dispositivo usado, entorno, etc...) 

### Hardware
> TO DO Kicad, FreeCAD

## Puntos de entrada

#### Software

La aplicación está escrita en python3 utilizando el framework Qt5.

Para lanzar la aplicación es necesario contar con las dependencias indicadas en el fichero
`gui/requirements.txt`.


```shell script
$ cd gui
$ pip install -r requirements.txt
$ ./main.py
```

#### Firmware

> TO DO: Carga del firmware en dispositivo y funcionamiento

## Modos de funcionamiento

- Firmware: [Ver man/firmware.md](man/firmware.md)

- Software y Monitorización: [Ver man/software.md](man/software.md)