#!/bin/bash
#
#  Copyright (C) 2014-2016 Alex Lotz, Matthias Lutz, Dennis Stampfer
#
#        Servicerobotik Ulm
#        University of Applied Sciences
#        Prittwitzstr. 10
#        D-89075 Ulm
#        Germany
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# Dennis Stampfer 15.1.2016
# Compatibility for raspberry-pi / arm architecture. More specific: Raspbian 8.0/jessie
#


echo "Usage: sudo ./INSTALL-ACE-6.5.8.sh [INSTALL-DIR]";

ACE=ACE-6.5.8

if [ $# -eq 1 ]; then
	INSTALL_DIR=$1;
else
	INSTALL_DIR=/opt;
fi

# check if script is run as root!
if [ $(whoami) != "root" ]; then echo "ERROR: please run as root (sudo $0)!"; exit 1; fi


# check if running on raspberry pi / raspbian
if `grep --ignore-case raspbian /etc/os-release > /dev/null`; then
        echo "===> Raspberry PI detected."
        OS_RASPBIAN=true
fi



# go to $INSTALL_DIR to install ACE into
cd $INSTALL_DIR;

# download ACE to current directory
rm -f $ACE.tar.gz;
echo "===> wget http://download.dre.vanderbilt.edu/previous_versions/$ACE.tar.gz";
wget http://download.dre.vanderbilt.edu/previous_versions/$ACE.tar.gz
if [ "$?" -ne 0 ]; then echo "ERROR: download failed!"; exit 1; fi

# rm possible former installation
rm -rf ACE_wrappers;
rm -rf $ACE;

# extract archive
echo "===> tar -xzf $ACE.tar.gz";
tar -xzf $ACE.tar.gz
if [ "$?" -ne 0 ]; then echo "ERROR: extraction failed!"; exit 1; fi

#  rename top level directory and set the softlink
echo "===> mv ACE_wrappers/ $ACE";
mv ACE_wrappers/ $ACE
echo "===> ln -s $ACE ACE_wrappers";
ln -s $ACE ACE_wrappers

# set environment variables
cd ACE_wrappers;
export ACE_ROOT=$PWD;
export _ROOT=$ACE_ROOT/;
export LD_LIBRARY_PATH=$ACE_ROOT/lib:$LD_LIBRARY_PATH;
cd $ACE_ROOT;


# create config.h file
echo "===> create FILE $ACE_ROOT/ace/config.h"
true > $ACE_ROOT/ace/config.h

# RaspberryPi specific settings
if $OS_RASPBIAN; then
	cat > $ACE_ROOT/ace/config.h << EOL
// RaspberryPi specific settings
#if defined (__arm__) && defined (__linux__)
        #define ACE_GCC_HAS_TEMPLATE_INSTANTIATION_VISIBILITY_ATTRS 1
        #define ACE_DEFAULT_BASE_ADDR ((char *) 0x40000000)
#endif

EOL
fi

echo "#include <ace/config-linux.h>" >> $ACE_ROOT/ace/config.h
echo "" >> $ACE_ROOT/ace/config.h


# create platform_macros.GNU file
echo "===> create FILE $ACE_ROOT/include/makeinclude/platform_macros.GNU";
true > $ACE_ROOT/include/makeinclude/platform_macros.GNU

if $OS_RASPBIAN; then
	echo "no_hidden_visibility=1" >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
fi

echo "valgrind=1" >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo "inline=1" >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo "include \$(ACE_ROOT)/include/makeinclude/platform_linux.GNU" >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo "" >> $ACE_ROOT/include/makeinclude/platform_macros.GNU


# compile ace kernel
cd $ACE_ROOT/ace;
time make;
if [ "$?" -ne 0 ]; then echo "FAILED to compile $ACE_ROOT/ace!"; exit 1; fi


#set ace so lib dir
echo "$INSTALL_DIR/ACE_wrappers/lib" >> /etc/ld.so.conf.d/ace.conf
ldconfig

echo "";
echo "ACE successfully installed!!!";
echo "BYE!";

