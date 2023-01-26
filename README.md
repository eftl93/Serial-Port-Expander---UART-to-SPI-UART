
# Signal Distribution Board
The original purpose of this board is to send strings of commands to a servo controller board and a high precision motor controller board, but the code can be modified to connect additional boards such as power modules, and other sensor boards.

I'm posting this entire project with more details in hackster.io: https://www.hackster.io/ederfernandotorres3/serial-port-expander-extra-spi-and-uart-ports-with-pic-mcu-697809

## Signal distribution board used on the robot
<img src="https://user-images.githubusercontent.com/86902176/210867012-98614071-db0a-4c9a-b8a4-59c4e95517c0.jpg" width="400">

## The I/O connectors of this board are as follows:
- Input power terminal: 7805 pin compatible regulator is used for voltage regulation.
- UART1 3-pin header: This is usually connected to the client sending commands such as a BeagleBone Black, Raspberry Pi, or other main MCU.
- UART2 3-pin header: In this project is used to send to a client that uses UART standard communication, some examples are: servo controllers, GPS module, Wi-Fi Modules, etc.
- 4 SPI connectors with JST 5-pin connector: This board is configured as an SPI Master controller as well. All 4 SPI connectors are connected to the MSSP1 peripheral of the PIC MCU. Every connector has GND, SDO, SDI, SCLK pins connected together. The SS pin on the connectors are connected to different MCU GPIO pins in order to control 4 different clients as per SPI standard.
- High Power LED terminal: This board contains a latching relay that switches 5V-regulated to two power-resistors, which are then connected to two block terminals. The main idea of these terminals is to power two high-power LEDs. In case that the LED(s) that need to be powered already contain built-in resistors (such as 5V LED strips), the onboard power-resistors can be bypassed by a wire (22 AWG solid wire is a reasonable jumper). Note: the 5V comes from the 7805 regulator, make sure to stay within its electrical and thermal ratings.
- 3 indicator LEDs: These onboard LEDs can be used to indicate different states of the board. I used to assist in the debugging process, but they can be used for other purposes.
- Pickit Connector: This is a 5-pin connector that aligns with the Pickit programmers from Microchip.

## Serial Peripheral Specifications
### UART
- Both UART1 and UART2 are setup to communicate with a baud rate of 115200 baud. This can be modified by changing the value in the registers: SPBRG1:SPBRGH1 (for UART1) and SPBRG2:SPBRGH2 (for UART2). These registers are set on the file: "uart.c" and inside the "uart_init()" function.

### SPI
- The SPI perfipheral is set run at a clock of 500 KHz, this can be modified by changing the value in the SSP1ADD register. The SSP1ADD register is set in the file "spi_master.c" inside the spi_master_init(void) function. The formula to calculate the SCLK is as follows: SCLK = (Fosc)/((SSP1ADD + 1)(4))
- The Fosc is the system's frequency, in this project, this board uses a 16 MHz crystal, and then the MCU uses a 4x phase lock loop to increase the frequency to 64 MHz, therefore: Fosc = 64 MHz. And the current value for SSP1ADD is 31. If we plug in the values we get: (64x10^6) / ((31 +1) (4)) =.5x10^6 or 0.5 MHz. In the equation, you can solve for the SSP1ADD variable and change the SCLK for the desired frequency.
