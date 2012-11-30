

%define FIONREAD 0x4004667F

%define MSG_OOB                    0x0001
%define MSG_PEEK                   0x0002
%define MSG_DONTROUTE              0x0004
%define MSG_WAITALL                0x0008
%define MSG_INTERRUPT              0x0010
%define MSG_PARTIAL                0x8000

STRUC Playload
	.sizeData			resd 1
	.checksum			resd 1
	.data:
ends