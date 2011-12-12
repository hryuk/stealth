
%include "MACROS.inc"
%include "PEB.inc"
%include "WIN32N.INC"

struc LoaderParam

ends

struc UNICODE_STRING
     .Length		resw 1;
     .MaximumLength	resw 1;
     .Buffer		resd 1;
ends