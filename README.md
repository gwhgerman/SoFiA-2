# SoFiA 2

This is version 2 of the HI Source Finding Application (SoFiA). SoFiA 2 is a reimplementation of the original SoFiA code in plain C. It is intended for use in HI data analysis pipelines and will be developed and maintained in parallel to SoFiA 1.x. As SoFiA 2 is currently under active development and **no stable version** is available at this point in time, we recommend that users **continue to use SoFiA 1.x** (https://github.com/SoFiA-Admin/SoFiA) for processing their data.

## Improvements in SoFiA 2

* Due to the use of C rather than Python, SoFiA 2 is somewhat faster than SoFiA 1.x.
* SoFiA 2 requires significantly less memory than SoFiA 1.x.
* SoFiA 2 currently does not have any external dependencies and should compile and run on any machine with a Linux or Unix operating system and the GCC compiler installed.

## Installation

If you are keen to test SoFiA 2 on your computer, please download and extract the source code into a directory of your choice. Then execute the `compile.sh` script to compile the software using the GCC compiler. Please ensure that you follow the instructions printed at the end of the compilation process to finalise the installation. Note that SoFiA 2 is still experimental and there is no documentation available. We do not recommend to use SoFiA 2 in production mode at this point in time.

## Feedback

Should you nevertheless decide to test SoFiA 2 on your own data, we would welcome any feedback on how well SoFiA 2 works for you and what improvements could be made. If you have a GitHub account, you can directly create a new issue (https://github.com/SoFiA-Admin/SoFiA-2/issues/new) on GitHub for questions, feature requests or bug reports. Alternatively, please feel free to directly contact the lead programmer, Tobias Westmeier, via e-mail at tobias.westmeier (at) uwa.edu.au to provide feedback on your experience with SoFiA 2. Note that the main purpose of SoFiA 2 is to facilitate processing of HI data from SKA precursor surveys, and we will not be able to accommodate requests for additional features beyond this primary scope.

## Copyright and licence

Copyright (C) 2019 Tobias Westmeier

SoFiA 2 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License  along with this program. If not, see http://www.gnu.org/licenses/.
