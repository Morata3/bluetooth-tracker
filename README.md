#System for tracking and monitoring of wireless devices using Bluetooth

This system contain two differents aproches. One of them is based on passive scanning via Ubertooth. The other one uses the HCI interface to perform active scanning of nearby devices. The bluetooth classic device are only detected by active scanning.

## Ubertooth software.
[Build guide](https://ubertooth.readthedocs.io/en/latest/build_guide.html)

## BlueZ.
BlueZ provides support for the core Bluetooth layers and protocols. It is flexible, efficient and uses a modular implementatation.

### BlueZ Raspberry Pi 3b+ installation.

1. Download last version at [official page](http://www.bluez.org). If you prefer also can use the following command:

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

>4.1 If error _configure: error: rst2man is required_ came up you will have to install the following package and run ./configure again
	
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

