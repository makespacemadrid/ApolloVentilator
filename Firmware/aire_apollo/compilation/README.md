# Compilation of the Arduino code
This folder contains code from [sudar's Arduino-Makefile](https://github.com/sudar/Arduino-Makefile).

## Usage
Move to the directory `/Firmware/aire_apollo` and execute `make`.

## How it works?
Using gitlab-ci, a worker node downloads the latest available `reespirator-test` docker image and executes the makefile, this will create a build to at least ensure the code compiles.

## Docker
The docker image `reespirator-test` is created from the Dockerfile in the root of the repository.
Working directory defaults to `/app`, where is expected to be mounted the app code.
The default cmd is set to `make`.
