/* sofia.i - swig wrapper for sofia code */
%module sofia
%{
int mainline(char *argv1);
%}

int mainline(char *argv1);
