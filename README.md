# System for tracking Bluetooth device sniffing their packets.

## BlueZ.
BlueZ provides support for the core Bluetooth layers and protocols. It is flexible, efficient and uses a modular implementatation.

### BlueZ Raspberry Pi 3b+ installation.

1. Download last version at [paxina oficial](http://www.bluez.org). If you prefer also can use the following command:

```console
	wget http://www.kernel.org/pub/linux/bluetooth/bluez-5.37.tar.xz
```

2. Unzip the file:

```console
	tar xvf [nome do ficheiro .tar.xz]
```

3. Update linux's repositoy and install dependencies:

```console
	sudo apt-get update  
	sudo apt-get install -y libusb-dev libdbus-1-dev libglib2.0-dev libudev-dev libical-dev libreadline-dev
```

4. Run the configuration script:

```console
	./configure --enable-library
```

>4.1 De salir o error _configure: error: rst2man is required_: Instalalo co seguinte comando e exucatr configure de novo.
	
		sudo apt-get install python-docutils
	

5. Compile and install BlueZ:

```console
	make  
	sudo make install
```

6. Install missing libraries if needed:

```console
	sudo apt install libbluetooth3
```

##

