
%include "MACROS.inc"
%include "PEB.inc"
%include "PE.inc"
%include "WIN32N.INC"

struc LoaderParam

ends
struc LIST_ENTRY
	.Flink	resd 1
	.Blink	resd 1
ends

struc UNICODE_STRING
     .Length		resw 1;
     .MaximumLength	resw 1;
     .Buffer		resd 1;
ends