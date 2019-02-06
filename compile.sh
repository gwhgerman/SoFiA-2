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

# Compile source files
gcc --std=c99 --pedantic -Wall -O3 -c src/common.c
gcc --std=c99 --pedantic -Wall -O3 -c src/statistics_dbl.c
gcc --std=c99 --pedantic -Wall -O3 -c src/statistics_flt.c
gcc --std=c99 --pedantic -Wall -O3 -c src/Path.c
gcc --std=c99 --pedantic -Wall -O3 -c src/Array.c
gcc --std=c99 --pedantic -Wall -O3 -c src/LinkerPar.c
gcc --std=c99 --pedantic -Wall -O3 -c src/Parameter.c
gcc --std=c99 --pedantic -Wall -O3 -c src/SourceCatalog.c
gcc --std=c99 --pedantic -Wall -O3 -c src/DataCube.c
gcc --std=c99 --pedantic -Wall -O3 -o sofia common.o statistics_flt.o statistics_dbl.o Path.o Array.o LinkerPar.o Parameter.o DataCube.o SourceCatalog.o sofia.c -lm

# Remove object files
rm -rf *.o
