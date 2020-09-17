# SimpleMenu by r0den

<div style="display: flex; align-items: center;margin-left: 30px;margin-bottom: 20px">
    <a href="https://github.com/ArduinoLibs/SimpleMenu/blob/master/README.md" style="font-size: 12px;line-height: 12px;">EN</a>
    <a href="https://github.com/ArduinoLibs/SimpleMenu/blob/master/README.ru.md" style="font-size: 12px; margin-left: 5px;line-height: 12px;">RU</a>
</div>

![version 1.0.0.0](https://img.shields.io/badge/version-1.0.0-blue "version 1.0.0.0")

### Welcome!
This library was made to give the ability for making menus and sub-menus very easily, by providing friendly functions for adding new menu-items and linking them to other menus, what allows creating powerful sub-menus.<br/>
The first idea of the library was to make it simple to create friendly menus with buttons and displays.

The library has the ability to talk to other libraries from the **[GyverLibs](https://github.com/AlexGyver/GyverLibs)** series <span style="font-style: italic;font-size: 12px; color: #aaa">(you maybe want to take a look on the count of libraries wrote almost from scratch to provide nicely functions to create powerful things)</span>, what gives you the option to tell the menu which buttons controls the menu and to which display you want to output the menu without working with buttons logics and display render settings.

Hopefully, I was able to tell you the basic of what this library can do, but that's surely not all, I am always trying to do my best to add integrity of other components for input and output, so in the future this library could do a lot more (I hope so)...

### Contributing
To help the library to grow up interested in contributing people can open issues on GitHub and create pull-requests.

### Downloads
In order to download and start using the library, you need to download the library files.<br />
To do it, scroll up until you see on the side-bar menu a link to the releases page, click on it and choose the version you want to download. I am recommend to download the most newer available version.

In order to install the library, copy the **SimpleMenu** folder to your Arduino libraries folder (By default it's at your documents folder > Arduino > libraries).
If you can't find the local user libraries folder, open the folder where you installed the Arduino IDE and find there the libraries folder (make sure you are entering the **libraries** folder and not the **libs**!).

I am also recommend to download the GyverLibs series libraries to create the option to control the menu with buttons and other supported libraries.

### Supported GyverLibs libraries
<div style="margin-left: 25px">
  <b>Input Devices</b>
  <ul style="margin-left: 25px">
  	<li><b>GyverButton</b> - I've never saw more powerful libraries to manage buttons. This library provides all you need to create functions of any difficulty.</li>
    <li><b>GyverEncoder</b> - Same as the GyverButton library, one of the most powerful libraries to manage encoders.</li>
  </ul>
  
  <b>Output Devices</b>
  <p style="margin-left: 25px">
  	Currently the library supports only the <b>GyverOLED</b> library.<br />
    If you want to make an output to other types of displays and control the output manually, see the <b>button2serial</b> example which outputting the whole menu to the serial port, changing the render function logic you can render the menu to whatever display (or other device) you want.
  </p>
</div>

### License
Copyright (c) r0den, davidsl4. All rights reserved. Licensed under the [MIT](https://github.com/ArduinoLibs/SimpleMenu/blob/master/LICENSE) license.