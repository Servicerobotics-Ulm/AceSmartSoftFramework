# ACE/SmartSoft Framework

This repository provides the **ACE/SmartSoft Framework** implementation. The **ACE/SmartSoft Framework** implements the [**SmartSoft Component-Developer API**](https://github.com/Servicerobotics-Ulm/SmartSoftComponentDeveloperAPIcpp) based on the [**The ADAPTIVE Communication Environment (ACE)**](http://www.cs.wustl.edu/~schmidt/ACE.html) middleware.

![CommunicationPatterns](http://www.servicerobotik-ulm.de/drupal/sites/default/files/inner-outer.png)

The ACE/SmartSoft Framework enables implementation of software components with clearly specified [communication patterns](http://robmosys.eu/wiki/modeling:metamodels:commpattern) as a basis for a service-oriented robotics component model which is fully compliant to the European [RobMoSys Component Metamodel](http://robmosys.eu/wiki/modeling:metamodels:component). Thereby, the ACE/SmartSoft Framework provides the required glue-code that links the component's internal [component developer API](https://github.com/Servicerobotics-Ulm/SmartSoftComponentDeveloperAPIcpp) with a specific communication middleware, in this case the [ADAPTIVE Communication Environment (ACE)](http://www.cs.wustl.edu/~schmidt/ACE.html). There also exist other implementations of the SmartSoft structures for middlewares like CORBA and DDS.

This repository is maintained by Servicerobotik Ulm. For more information see:

* Big picture: relation of repositories: https://wiki.servicerobotik-ulm.de/download
* SRRC Technical Wiki on SmartSoft and SmartMDSD Toolchain: https://wiki.servicerobotik-ulm.de

## Installation instructions

Installation requirements:

  * [**CMake**](https://cmake.org/) min. version **3.0**
  * A C++ compiler supporting the **C++ 11** standard

The **ACE/SmartSoft Framework** has two main dependencies, the [**The ADAPTIVE Communication Environment (ACE)**](http://www.cs.wustl.edu/~schmidt/ACE.html) middleware which can be comfortably installed using a bash scipt (see below) and the [**SmartSoft Component-Developer API**](https://github.com/Servicerobotics-Ulm/SmartSoftComponentDeveloperAPIcpp) (see instructions below).

Install the necessary build essentials:
```bash
sudo apt-get install build-essential pkg-config cmake
```

### Installing the ADAPTIVE Communication Environment (ACE)

The installation of the ACE middleware with **Ubuntu 18.04 and 20.04** can be done using the shipped Debian package from the default repos:

```bash
> sudo apt-get install libace-dev
```
Nothing further has to be done when using the packages!



With older or other Linux systems you can install the ACE middleware by using the shipped [**installation script**](https://github.com/Servicerobotics-Ulm/AceSmartSoftFramework/blob/master/INSTALL-ACE-6.0.2.sh). Download the script and execute it within a terminal as follows:

```bash
> wget https://raw.githubusercontent.com/Servicerobotics-Ulm/AceSmartSoftFramework/master/INSTALL-ACE-6.0.2.sh
> chmod +x INSTALL-ACE-6.0.2.sh
> sudo ./INSTALL-ACE-6.0.2.sh
```
The default location for the installed ACE library is (on Linux) **/opt/ACE_wrappers**. However, you can select any custom locaiton of your choice. In order for the ACE/SmartSoft framework to find your ACE library installation the environment variable named **ACE_ROOT** is used which needs the be specified after the above installation. The best way is to use the **~/.pfile** for this as follows:

```bash
> echo "export ACE_ROOT=/opt/ACE_wrappers" > ~/.profile
```
Don't forget to either restart your system to reload the **.profile** file or call `source ~/.profile` within your currently opened terminal.

### Compiling the SmartSoft Component-Developer API

For cloning and building the SmartSoft Component-Developer API github repository, please follow the instructions below. Please note that the default installation folder is assumed to be defined over the environment variable named **SMART_ROOT_ACE** and the best practice for the clone location is **$SMART_ROOT_ACE/repos**.

```
> export SMART_ROOT_ACE=~/SOFTWARE/smartsoft
> mkdir -p $SMART_ROOT_ACE/repos
> cd $SMART_ROOT_ACE/repos
> git clone https://github.com/Servicerobotics-Ulm/SmartSoftComponentDeveloperAPIcpp.git
> cd SmartSoftComponentDeveloperAPIcpp
> mkdir build
> cd build
> cmake ..
> make install
```

Now the ACE/SmartSoft Framework can be compiled as shown in the following section.

### Compiling the ACE/SmartSoft Framework

For compiling the ACE/SmartSoft Framework it is assumed that the ACE middleware has been installed as explained above and that CMake (at least version 3) and a C++ compiler supporting the standard 11 are preinstalled. At the moment, **Ubuntu 16.04 64bit** is the mostly tested and used operating system, however, Windows 7 and OSx have been also tested in the past.

First, checkout a copy of the ACE/SmartSoft github repository into any location of your choice, for example like this:

```
> mkdir ~/SOFTWARE/smartsoft/repos
> cd ~/SOFTWARE/smartsoft/repos
> git clone https://github.com/Servicerobotics-Ulm/AceSmartSoftFramework.git
```

Next, select a location where all the locally compiled binaries, libraries, headers and all other files should be installed. For instance, this can be the location **~/SOFTWARE/smartsoft**. This location should be specified using the environment variable called **SMART_ROOT_ACE** (use .profile as above to make this variable persistent). After that you can compile and install ACE/SmartSoft like this:

```
> cd ~/SOFTWARE/smartsoft
> echo "export SMART_ROOT_ACE=$PWD" > ~/.profile
> source ~/.profile
> cd $SMART_ROOT_ACE/repos/AceSmartSoftFramework
> mkdir build
> cd build
> cmake ..
> make install
```

That should fully compile and install the ACE/SmartSoft framework.

Please note, that components will need to find the libraries at startup using the **LD_LIBRARY_PATH** environment variable which can be set as follows (you only need to do this once, if not already done):

```
> echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SMART_ROOT_ACE/lib" > ~/.bashrc
> bash
```

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
