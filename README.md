# ACE/SmartSoft Framework

This repository provides the **ACE/SmartSoft Framework** implementation. The **ACE/SmartSoft Framework** implements the [**SmartSoft Component-Developer API**](https://github.com/Servicerobotics-Ulm/SmartSoftComponentDeveloperAPIcpp) based on the [**The ADAPTIVE Communication Environment (ACE)**](http://www.cs.wustl.edu/~schmidt/ACE.html) middleware.

## Installation instructions

Installation requirements:

  * [**CMake**](https://cmake.org/) min. version **3.0**
  * A C++ compiler supporting the **C++ 11** standard

The **ACE/SmartSoft Framework** has two main dependencies, the [**The ADAPTIVE Communication Environment (ACE)**](http://www.cs.wustl.edu/~schmidt/ACE.html) middleware which can be comfortably installed using a bash scipt (see below) and the [**SmartSoft Component-Developer API**](https://github.com/Servicerobotics-Ulm/SmartSoftComponentDeveloperAPIcpp) which is automatically downloaded by the CMake script.

### Installing the ADAPTIVE Communication Environment (ACE)

For installing the ACE middleware you can use the shipped [**installation script**](https://github.com/Servicerobotics-Ulm/AceSmartSoftFramework/blob/master/INSTALL-ACE-6.0.2.sh). Download the script and execute it within a terminal as follows:

```bash
> wget https://github.com/Servicerobotics-Ulm/AceSmartSoftFramework/blob/master/INSTALL-ACE-6.0.2.sh
> chmod +x INSTALL-ACE-6.0.2.sh
> sudo ./INSTALL-ACE-6.0.2.sh
```
The default location for the installed ACE library is (on Linux) **/opt/ACE_wrappers**. However, you can select any custom locaiton of your choice. In order for the ACE/SmartSoft framework to find your ACE library installation the environment variable named **ACE_ROOT** is used which needs the be specified after the above installation. The best way is to use the **~/.pfile** for this as follows:

```bash
> echo "export ACE_ROOT=/opt/ACE_wrappers" > ~/.profile
```
Don't forget to either restart your system to reload the **.profile** file or call `source ~/.profile` within your currently opened terminal.

### Compiling the ACE/SmartSoft Framework

For compiling the ACE/SmartSoft Framework it is assumed that the ACE middleware has been installed as explained above and that CMake (at least version 3) and a C++ compiler supporting the standard 11 are preinstalled. At the moment, **Ubuntu 16.04 64bit** is the mostly tested and used operating system, however, Windows 7 and OSx have been also tested in the past.

First, checkout a copy of the ACE/SmartSoft github repository into any location of your choice, for example like this:

```
> mkdir ~/SOFTWARE/smartsoft/src
> cd ~/SOFTWARE/smartsoft/src
> git clone https://github.com/Servicerobotics-Ulm/AceSmartSoftFramework.git
```

Next, select a location where all the locally compiled binaries, libraries, headers and all other files should be installed. For instance, this can be the location **~/SOFTWARE/smartsoft**. This location should be specified using the environment variable called **SMART_ROOT_ACE** (use again .profile as above for this). After that you can compile and install ACE/SmartSoft like this:

```
> cd ~/SOFTWARE/smartsoft
> echo "export SMART_ROOT_ACE=$PWD" > ~/.profile
> source ~/.profile
> cd src
> mkdir build
> cd build
> cmake ..
> make install
```

That should fully compile and install the ACE/SmartSoft framework.

### Trying out the shipped example components

For trying out the examples, first start the naming-service deamon like this:

```
> cd $SMART_ROOT_ACE
> ./startSmartSoftNamingService
```

After that execute the example-component binaries (each in a separate terminal) like this:

```
> cd $SMART_ROOT_ACE
> ./bin/exampleComponent10
```

Enjoy!
