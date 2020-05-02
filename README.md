# ApolloVentilator

Information: Checkout our [wiki](https://github.com/makespacemadrid/ApolloVentilator/wiki)


### Schematic
![Schematic](https://github.com/makespacemadrid/ApolloVentilator/raw/master/media/esquemaBasico.png)


### Prototype 1 @ MakeSpace Madrid
![Prototipe1] (https://github.com/makespacemadrid/ApolloVentilator/raw/master/media/prototype1.jpg)

### Prototype 2 @ Miguel's Workshop
![Prototipe2] (https://github.com/makespacemadrid/ApolloVentilator/raw/master/media/prototype2.jpg)

## Getting started

### Linux

```
$ apt-get install libcairo2-dev libgirepository1.0-dev python3-cairo-dev
```

### Mac

```
$ brew install pygobject3 gtk+3 libnotify
```

### App (Python 3)

From ```./gui```

Install system dependencies (Debian-based):

```
$ pip install -r requirements.txt
$ ./main.py
```

## Requirements:
2 ventilators mode
* Pressure Regulated Volume Controlled (PCV): set pressure delivered during period of inspiration. The voume measured is shown as data on display. 
* Volume Regulated Pressure controlled (VCV): The tidal volume is set and it adapts the pressure to the lowest. Here the important data are: tidal volume, upper alarm and lower current volume. Useful parameters: tidal volume, respiratory rate. There is a limit pressure and during inspiration period.

If patient stops breathing, there is a failsafe that set to mandatory breathing when it's detected.

During inspiration:

* Inspiration pressure: The plateau pressure is adjusted to volume hard limit (32 cmH2O by default, and could increate to 70cmH2O)
* Peak pressure: 2cmH2O greater than plateau.
* VCV pressure limit (15-40cm2 with increments of 5)
* Failsafe valve that opens always at 80cmH20 (or lower)

* PEEP: 5-10 with increments of 5. The circuit must be pressured.
* IE ratio: 1:1, 1:2, 1:3
* Respiratory rate: 10-30 bpm with increments of 2.
* Tidal volume: 40ml +-10, 350 +-50, 250-600 +-50,up to 8000. 
* Weight body and volume calculated based on 6ml/kg.


## Alarms

* Gas failure
* Electricity failure
* Machine switched off
* Inspiration pressure exceeded
* Inspiration pressure not achieved
* PEEP pressure not achieved
* Tidal volume not achieved
* Tidal volume exceeded
* Resistance: peak pressure - plateau pressure > 2cmH2O
* Compliance
* Volume Leakage

## Monitoring
### Settings
* Tidal volume
* Frequency
* PEEP
* FI02
* Ventilation Mode

### Current status
* Tidal volume
* Breathing rate
* PEEP
* Plateau pressure
* FI02

The normal healthy O2 consuption is 250mL/min, and could be increased until, for example 500 mL/min in a sepsis.

### Parallel measures 

* Presión arteria
* Frecuencia cardiaca
* ECG
* Nivel de sedación
* Posicion del tubo e inflado del neumotaponamiento
* Intercambio gaseoso (pulsiometria SO2), capnografía y gasometrías arteriales periodicas para obtener los **índices de oxigenación** (clave para evaluar la desintubación del paciente).
