/* sofia.i - swig wrapper for sofia code */
%module sofia
%{
int mainline(char *par, char *hdrPtr, char *dataPtr);
%}

int mainline(char *par, char *hdrPtr, char *dataPtr);
