# Things to do: #

## Linux ##

    git clone --recurse-submodules https://github.com/Fortschrittarier/SensorsSimulation.git
    cd SensorsSimulation
    mkdir build
    cd build/
    cmake ..
    make
    ./bin/sensors ../sensor_config.yml
	make test

## Windows ##
In a *Terminal*

	git clone --recurse-submodules https://github.com/Fortschrittarier/SensorsSimulation.git
	cd SensorsSimulation
    mkdir build
    cd build/
    cmake ..
	
Open ```build\sensor_project.sln``` with *Visual Studio*

- Right click on project ```test_main``` and open ```Properties```
- Select ```C/C++ -> Code Generation```
- Set ```Runtime Library``` to ```Multithreaded (/MT)```
- Build solution as ```Release x64```
	
Back in *Terminal*

	bin\Release\sensors.exe ..\sensor_config.yml
	bin\Release\test_main.exe