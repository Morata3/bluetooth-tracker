# System for tracking Bluetooth device sniffing their packets.

## BlueZ.
BlueZ provides support for the core Bluetooth layers and protocols. It is flexible, efficient and uses a modular implementatation.

### Instalación en Raspberry Pi 3b+.

1. Download last version at [paxina oficial](http://www.bluez.org). If you prefer can use the following command:

	wget http://www.kernel.org/pub/linux/bluetooth/bluez-5.37.tar.xz


2. Descomprimimos o ficheiro:

	tar xvf [nome do ficheiro .tar.xz]

3. Actualizamos o repositorio e instalamos as dependencias necesarias:

	sudo apt-get update  
	sudo apt-get install -y libusb-dev libdbus-1-dev libglib2.0-dev libudev-dev libical-dev libreadline-dev

4. Configuramos o script para poder compilar

	./configure --enable-library

>4.1 De salir o error _configure: error: rst2man is required_: Instalalo co seguinte comando e exucatr configure de novo.
	
		sudo apt-get install python-docutils
	

5. Compilamos e instalamos bluez:

	make  
	sudo make install

6. De faltar algunha librería instalala, por exemplo:

	sudo apt install libbluetooth3


