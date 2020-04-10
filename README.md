# Nitrox Analyzer
> An arduino-based oxygen analyzer for EANx/Nitrox diving gases.

[![PlatformIO][build-with-platformio]][platformio-url]
[![KiCad][made-with-kicad]][kicad-url]
![Nitrox][dive-nitrox]

<!---
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]
-->

<!-- TABLE OF CONTENTS -->
## Table of Contents

* [About the Project](#about-the-project)
  * [Built With](#built-with)
* [Hardware](#hardware)
* [Roadmap](#roadmap)
* [License](#license)
* [Contact](#contact)
* [Acknowledgements](#acknowledgements)


<!-- ABOUT THE PROJECT -->
## About The Project

[![Nitrox Analyzer][product-screenshot]]

There are many plans and schematics for Nitrox analyzers available online.
This project is an attempt to build a compact and neat device, inspired by the main characteristics of these designs:

* Arduino based
* Bright OLED display
* Simple user interface using a rotary encoder
* Automatic MOD calculation for most common O<sub>2</sub> partial pressures
* Li-Ion battery, rechargeable using a micro-USB phone charger
* Sound feedback
* Custom PCB mounted for increased reliability and more polished look

### Built With

* [PlatformIO][platformio-url]
  * Arduino Framework
  * U8g2
  * ClickEncoder
  * TimerOne
* [KiCad](https://kicad-pcb.org/) for PCB design


<!-- HARDWARE -->
## Hardware

This analyzer is based on classical and cheap components, they can be sourced for <20€ (oxygen cell excluded):


* **Arduino Pro Mini 3.3V/8MHz:** this microcontroller board has been chosen for its availability in 3.3V version and therefore the possibility to run on a single Li-ion battery.
* **ADS1115 breakout board:** this provides 16-bit ADC resolution over I²C, with PGA up to 16x
* **Rotary encoder with switch**
* **OLED 128x64 I²C display**
* A few small and passive components
* Optional:
  * Li-ion battery (18650)
  * TP4056 micro-usb battery charger with protection
  * Buzzer
  * Case
* You must also source an oxygen cell (e.g. used in a CCR)

Because the Pro Mini does not integrate a USB interface, you have to use a **USB to UART** breakout adapter (typically FTDI ft232) in order to program the chip. This can also be sourced for ~2€. Make sure to set the jumper on the 3.3V position.

The analyzer can be assembled by hand-wiring the components, or by soldering them on a PCB.
A detailled BOM for the PCB is available in the project `pcb` folder.


<!-- ROADMAP -->
## Roadmap

See the [open issues](https://github.com/plut0nium/eanx_analyzer/issues) for a list of proposed features (and known issues).


<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` for more information.


<!-- CONTACT -->
## Contact

_Charles Fourneau_

Project Link: [https://github.com/plut0nium/eanx_analyzer](https://github.com/plut0nium/eanx_analyzer)


<!-- ACKNOWLEDGEMENTS -->
## Acknowledgements
* [U8g2](https://github.com/olikraus/u8g2/)
* [ClickEncoder by Dennis](https://github.com/soligen2010/encoder)
* [TimerOne](https://github.com/PaulStoffregen/TimerOne)
* [Adafruit_ADS1X15](https://github.com/adafruit/Adafruit_ADS1X15)
* and many others...

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[product-screenshot]: doc
[dive-nitrox]: https://img.shields.io/badge/Dive-Nitrox-green

[build-with-platformio]: https://img.shields.io/badge/build%20with-PlatformIO-orange
[platformio-url]: https://platformio.org
[made-with-kicad]: https://img.shields.io/badge/made%20with-KiCad-blue
[kicad-url]: https://www.kicad-pcb.org/
[issues-url]: https://github.com/plut0nium/eanx_analyzer/issues
[license-shield]: https://img.shields.io/github/license/plut0nium/eanx_analyzer.svg?style=flat-square
[license-url]: https://github.com/plut0nium/eanx_analyzer/blob/master/LICENSE
