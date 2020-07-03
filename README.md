# Things to do: #

## Linux ##

    git clone --recurse-submodules https://github.com/Fortschrittarier/SensorsSimulation.git
    cd SensorsSimulation
    mkdir build
    cd build/
    cmake ..
    make
    ./sensors ../sensor_config.yml

## Windows ##
In a *Developer Command Prompt*

	git clone --recurse-submodules https://github.com/Fortschrittarier/SensorsSimulation.git
	cd SensorsSimulation
    mkdir build
    cd build/
    cmake ..
	
Open SensorsSimulation.sln with *Visual Studio*
	* Right click on project ```test_main``` and open ```Properties```
	* Select ```C/C++ -> Code Generation```
	* Set ```Runtime Library``` to ```Multithreaded (/MT)```
	* Build solution as ```Release x64```
	
Back in *Developer Command Prompt*

	bin\Release\sensors.exe ..\sensor_config.yml
	bin\Release\test_main.exe