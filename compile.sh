#!/bin/sh
### ____________________________________________________________________ ###
###                                                                      ###
### SoFiA 2.0.0-beta (compile.sh) - Source Finding Application           ###
### Copyright (C) 2019 Tobias Westmeier                                  ###
### ____________________________________________________________________ ###
###                                                                      ###
### Address:  Tobias Westmeier                                           ###
###           ICRAR M468                                                 ###
###           The University of Western Australia                        ###
###           35 Stirling Highway                                        ###
###           Crawley WA 6009                                            ###
###           Australia                                                  ###
###                                                                      ###
### E-mail:   tobias.westmeier [at] uwa.edu.au                           ###
### ____________________________________________________________________ ###
###                                                                      ###
### This program is free software: you can redistribute it and/or modify ###
### it under the terms of the GNU General Public License as published by ###
### the Free Software Foundation, either version 3 of the License, or    ###
### (at your option) any later version.                                  ###
###                                                                      ###
### This program is distributed in the hope that it will be useful,      ###
### but WITHOUT ANY WARRANTY; without even the implied warranty of       ###
### MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         ###
### GNU General Public License for more details.                         ###
###                                                                      ###
### You should have received a copy of the GNU General Public License    ###
### along with this program. If not, see http://www.gnu.org/licenses/.   ###
### ____________________________________________________________________ ###
###                                                                      ###

echo "\033[36m_______________________________________________________________________\033[0m"
echo
echo " \033[1mInstalling SoFiA\033[0m"
echo "\033[36m_______________________________________________________________________\033[0m"
echo

# Compile source files
echo "  Compiling src/common.c"
gcc --std=c99 --pedantic -Wall -O3    -o src/common.o         -c src/common.c
echo "  Compiling src/statistics_dbl.c"
gcc --std=c99 --pedantic -Wall -O3    -o src/statistics_dbl.o -c src/statistics_dbl.c
echo "  Compiling src/statistics_flt.c"
gcc --std=c99 --pedantic -Wall -O3    -o src/statistics_flt.o -c src/statistics_flt.c
echo "  Compiling src/Stack.c"
gcc --std=c99 --pedantic -Wall -O3    -o src/Stack.o          -c src/Stack.c
echo "  Compiling src/Path.c"
gcc --std=c99 --pedantic -Wall -O3    -o src/Path.o           -c src/Path.c
echo "  Compiling src/Array.c"
gcc --std=c99 --pedantic -Wall -O3    -o src/Array.o          -c src/Array.c
echo "  Compiling src/Map.c"
gcc --std=c99 --pedantic -Wall -O3    -o src/Map.o            -c src/Map.c
echo "  Compiling src/Matrix.c"
gcc --std=c99 --pedantic -Wall -O3    -o src/Matrix.o         -c src/Matrix.c
echo "  Compiling src/LinkerPar.c"
gcc --std=c99 --pedantic -Wall -O3    -o src/LinkerPar.o      -c src/LinkerPar.c
echo "  Compiling src/Parameter.c"
gcc --std=c99 --pedantic -Wall -O3    -o src/Parameter.o      -c src/Parameter.c
echo "  Compiling src/Source.c"
gcc --std=c99 --pedantic -Wall -O3    -o src/Source.o         -c src/Source.c
echo "  Compiling src/Catalog.c"
gcc --std=c99 --pedantic -Wall -O3    -o src/Catalog.o        -c src/Catalog.c
echo "  Compiling src/DataCube.c"
gcc --std=c99 --pedantic -Wall -O3    -o src/DataCube.o       -c src/DataCube.c
echo "  Compiling sofia.c"
gcc --std=c99 --pedantic -Wall -O3    -o sofia  src/common.o  src/statistics_flt.o  src/statistics_dbl.o  src/Stack.o  src/Path.o  src/Array.o  src/Map.o  src/Matrix.o  src/LinkerPar.o  src/Parameter.o  src/DataCube.o  src/Source.o  src/Catalog.o sofia.c -lm

# Remove object files
rm -rf src/*.o

# Print instructions
echo "\033[36m_______________________________________________________________________\033[0m"
echo
echo " \033[1mInstallation complete\033[0m"
echo "\033[36m_______________________________________________________________________\033[0m"
echo
echo "  Please check above for any error messages produced by the compiler"
echo "  before proceeding with the instructions below."
echo
echo "  If no error messages have occured, please add the following line to"
echo "  your \033[1;36m.bashrc\033[0m or \033[1;36m.cshrc\033[0m file to complete the installation process:"
echo
echo "  \033[1;36mBASH:\033[0m"
echo "    \033[1mexport SOFIA2_PATH=\"$PWD\"\033[0m"
echo
echo "  \033[1;36mCSH:\033[0m"
echo "    \033[1msetenv SOFIA2_PATH \"$PWD\"\033[0m"
echo
echo "  In order to make SoFiA available across the entire system, you can"
echo "  either create a \033[1;36msymbolic link\033[0m in /usr/bin, e.g.:"
echo
echo "    \033[1msudo ln -s \033[3m<sofia_path>\033[23m /usr/bin/sofia\033[0m"
echo
echo "  where \033[3m<sofia_path>\033[23m is the full path name of the 'sofia' executable,"
echo "  or alternatively create an alias to the 'sofia' executable."
echo
