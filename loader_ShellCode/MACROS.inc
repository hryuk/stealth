%ifndef __MACROS_INC__
%define __MACROS_INC__
 
%define nline 0x0D,0x0A
 
%define ptr
%define sizeof(_x_)       _x_ %+ _size
%define reserve(_x_)    _x_ %+ _res
 
%idefine byte_size       1
%idefine word_size     (byte_size * 2)
%idefine dword_size      (word_size * 2)
%idefine qword_size      (dword_sized * 2)
%idefine tword_size        10
%idefine oword_size       (qword_size * 2)
 
%idefine byte_res       RESB
%idefine word_res       RESW
%idefine dword_res      RESD
%idefine qword_res      RESQ
%idefine tword_res      REST
%idefine oword_res      RESO
 
%define int8_t          byte
%define int16_t         word
%define int32_t         dword
%define int64_t        qword
%define int80_t        tword
%define int128_t       oword
 
%define int8_t_size        1
%define int16_t_size   (int8_t_size * 2)
%define int32_t_size   (int16_t_size * 2)
%define int64_t_size  (int32_t_size * 2)
%define int80_t_size  10
%define int128_t_size (int64_t_size * 2)
 
%define int8_t_res        RESB
%define int16_t_res     RESW
%define int32_t_res     RESD
%define int64_t_res     RESQ
%define int80_t_res     REST
%define int128_t_res    RESO
 
%define iax8        al
%define iax16     ax
%define iax32     eax
%define iax64        rax
%define iax128       xmm0
 
%define ibx8        bl
%define ibx16     bx
%define ibx32     ebx
%define ibx64        rbx
%define ibx128       xmm1
 
%define icx8        cl
%define icx16     cx
%define icx32     ecx
%define icx64        rcx
%define icx128       xmm3
 
%define idx8        dl
%define idx16     dx
%define idx32     edx
%define idx64        rdx
%define idx128       xmm4
 
%define ibp8        sp
%define ibp16     sp
%define ibp32     esp
%define ibp64        rsp
%define ibp128       rsp
 
%define isp8     sp
%define isp16     sp
%define isp32     esp
%define isp64        rsp
%define isp128       rsp
 
%macro undef 1-99
    %rep %0
 %deftok arg__ %1
    %undef $arg__
   %warning arg__
   
    %ifdef arg__
    ;   %undef %arg__
   %endif
     %rotate 1
  %endrep
%endmacro
 
;----------High-Level Call
;Usage:
;   ccall FuncName, param1, param2, param 3...
;   pcall FuncName, param1, param2, param 3...
;   stdcall FuncName, param1, param2, param 3...
;   fastcall FuncName, param1, param2, param 3...
%macro pcall 2-9 ;Pascal Call: push first value first, no cleanup
%define _j %1
%rep %0-1
    %rotate 1
    push %1
%endrep
    call _j
%endmacro
 
%macro ccall 2-9 ;C Call: push last value first, cleanup
%define _j %1
%assign __params %0-1
%rep %0-1
    %rotate -1
    push %1
%endrep
    call _j
    %assign __params __params * 4
    add esp, __params
%endmacro
 
%macro stdcall 2-9 ;Windows StdCall: push last value first, no cleanup
%define _j %1
%rep %0-1
    %rotate -1
    push %1
%endrep
    call _j
%endmacro
 
%macro fastcall 2-9 ;Borland FastCall: use registers, no cleanup
%define _j %1
%assign __pnum 1
%rep %0-4
    %rotate -1
    %if __pnum = 1
     mov eax, %1
    %elif __pnum = 2
     mov edx, %1
    %elif __pnum = 3
     mov ebx, %1
    %else
     push %1
    %endif
    %assign __pnum __pnum+1
%endrep
    call _j
%endmacro
 
;----------IF-THEN-ELSE Loops
;Usage:
;   IF Value, Cond, Value
;    ;if code here
;   ELSIF Value, Cond, Value
;    ;else-if code here
;   ELSE
;    ;else code here
;   ENDIF
;Where Value is a variable, register, or integer -- but not EAX or EBX!!
;Where Cond is the conditional -- a, ae, b, be, e, g, ge, le, ne, nz, z, etc..
%macro IF 3
 %push if
 %assign __curr 1
; %if %1 != eax
    mov eax, %1
; %endif
; %if %3 != ebx
    mov ebx, %3
; %endif
    cmp eax, ebx
    j%+2 %%if_code
    jmp %$loc(__curr)
%%if_code:
%endmacro
 
%macro ELSIF 3
  %ifctx if
    jmp %$end_if
%$loc(__curr):
  %assign __curr __curr+1
;  %if %1 != eax
    mov eax, %1
;  %endif
;  %if %3 != ebx
    mov ebx, %3
;  %endif
    cmp eax, ebx
    j%+2 %%elsif_code
    jmp %$loc(__curr)
%%elsif_code:
 %else
   %error "'ELSIF' can only be used following 'IF'"
 %endif
%endmacro
 
%macro ELSE 0
 %ifctx if
    jmp %$end_if
%$loc(__curr):
%assign __curr __curr+1
 %else
  %error "'ELSE' can only be used following an 'IF'"
 %endif
%endmacro
 
%macro ENDIF 0
%$loc(__curr):
%$end_if:
%pop
%endmacro
 
;----------SWITCH-CASE Blocks
;Usage:
;   SWITCH Variable
;   CASE Int
;        ;case1 code here
;        BREAK
;   CASE Int
;        ;case2 code here
;        BREAK
;   DEFAULT
;        ;default code here
;        BREAK
;   ENDSWITCH
;Where Variable is a variable of INT to DWORD value [eax is used to store]
;Where Int is an immediate INT to DWORD value [ebx is used to store]
%macro SWITCH 1
%push switch
%assign __curr 1
    mov eax, %1
    jmp %$loc(__curr)
%endmacro
 
%macro CASE 1
%ifctx switch
    %$loc(__curr):
    %assign __curr __curr+1
    mov ebx, %1
    cmp eax, ebx
    jne %$loc(__curr)
%else
    %error "'CASE' can only be used in a 'SWITCH' block"
%endif
%endmacro
 
%macro DEFAULT 0
%ifctx switch
    %$loc(__curr):
%else
    %error "'DEFAULT' can only be used in a 'SWITCH' block"
%endif
%endmacro
 
%macro BREAK 0
    jmp %$endswitch
%endmacro
 
%macro ENDSWITCH 0
    %ifctx switch
    %$endswitch:
    %pop
    %else
         %error "'ENDSWITCH' can only be used in an 'SWITCH' block"
    %endif
%endmacro
 
;----------DO-FOR and DO-WHILE Loops
;Usage:
;   DO
;    ;code to do here
;   FOR min, cond, max, step
;Where Min is a value or variable of INT to DWORD value [eax is used]
;Where Cond is condition -- a, ae, b, be, g, ge, l, le, n, ne, z
;Where Max is an INT to DWORD value [ebx is used to store]
;Where Step is an INT to DWORD value to be added to Min at each loop
;e.g. FOR 0, l, 100, 1 == for(x=0; x<100, x++) do code...;
;   DO
;    ;code to do here
;   WHILE variable, condition, value
;Where variable is a value or variable of INT to DWORD value [eax is used]
;Where condition is a conditional -- a, ae, b, be, g, ge, l, le, n, ne, z
;Where Value is an INT to DWORD value [ebx is used to store]
;e.g. WHILE Counter, l, 100 == while( counter < 100 ) do code...;
%macro DO 0
 %push do
    jmp %$init_loop
%$start_loop:
    push eax
%endmacro
 
%macro FOR 4
 %ifctx do
    pop eax
    add eax, %4
    cmp eax, %3
    j%-2 %%end_loop
    jmp %$start_loop
%$init_loop:
    mov eax, %1
    jmp %$start_loop
%%end_loop:
 %pop
 %else
  %error "'FOR' must be preceded by a 'DO'"
 %endif
%endmacro
 
%macro WHILE 3
 %ifctx do
    pop eax
    mov ebx, %3
    cmp eax, ebx
    j%+2 %%end_loop
    jmp %$start_loop
%$init_loop:
    mov eax, %1
    jmp %$start_loop
%%end_loop:
 %pop
 %else
  %error "'WHILE' must be preceded by a 'DO'"
 %endif
%endmacro
 
;----------Procedure Declarations
;Usage:
;PROC ProcName Parameter1, Parameter2, Parameter3...
;LOCALDD Dword_Variable [opt]
;LOCALDW Word_Variable [opt]
;LOCALDB Byte_Variable [opt]
;   ;procedure code here
;ENDP
;
;ENTRYPROC
    ;procedure code here
;ENDP
;
;Note that parameters are set up to EQU to ebp-x
;Note that locals are set up to EQU to ebp+x
;Note that neither LOCALS or Parameters are necessary, as in ENTRYPROC
 
%macro PROC 1-99
GLOBAL %1
%1:
%push local
 %assign _i 4
 ;%assign %[%2]_ 0
 %rep %0-1
 %assign _i _i+4
      
   %define curr_arg_name %2
   %ifndef proc_args__
        %define proc_args__ %[curr_arg_name]
   %else
      %define proc_args__ %[proc_args__],%[curr_arg_name]
    %endif
       
   ;%assign %[%2]_ _i
    %define %$%2 [ebp+%[_i]]
    ;%define %[curr_arg_name] [ebp+%[_i]]
  
 %rotate 1
 %endrep
    push ebp
    mov ebp, esp
  
 %assign __ll 0
 LOCALDD _EDX
 LOCALDD _ECX
%endmacro
%macro PROC__ 3-99
GLOBAL %00
;%1:
%push %00
 %assign _i 4
 ;%assign %[%2]_ 0
 %stacksize flat          ; tell NASM to use bp
    ;%arg      i:word, j_ptr:word
 %rep %0
 %assign _i _i+4
   
    
   %define curr_arg_name %1
   %defstr curr_arg_name_str %1
    
   %ifndef proc_args__
        %define proc_args__ %[curr_arg_name_str]
   %else
      %define proc_args__ %[proc_args__],%[curr_arg_name_str]
    %endif
       
    ;%define %[curr_arg_name] [ebp+%[_i]]
    %arg %1
 %rotate 1
 %endrep
   ;undef %[proc_args__]
  %warning %[curr_arg_name]
  %undef curr_arg_name
    push ebp
    mov ebp, esp
     
  
 ;%assign __ll 0
 ;LOCALDD _EDX
 ;LOCALDD _ECX
%endmacro
 
 
%macro ENDP 0
 
   ;%ifdef proc_args__
 ;   undef %[proc_args__]
    ;   %undef proc_args__
  ;%endif
 %ifndef __returned__
 	leave
 	%ifdef _CPROC_
 		ret
 	%undef _CPROC_
 	%else
 		ret _i-4
	%endif
 	%ifctx local
 		 %pop
 	%endif
 %else
 	%undef __returned__
 %endif
%endmacro
;//Macro return// como en c
%macro return 1
 	%define __returned__
	mov eax,%1
	mov dword [esp+0x1C],eax
	popad
	leave
	%ifdef _CPROC_
		ret
 		%undef _CPROC_
 	%else
    	ret _i-4
    %endif
%endmacro

 
%define endp ENDP
%define proc PROC
 
 
%macro ENTRYPROC 0
PROC main
%endmacro
 
%macro LOCALVAR 1
    ;sub esp, 4
    %assign %[%1]_ __ll
    %define %$%1 [ebp-%[%1]_]
%endmacro
%macro LOCALDB 1
  %assign __ll __ll+1
  LOCALVAR %1
%endmacro
%define localdb  LOCALDB
 
 
%macro LOCALDW 1
  %assign __ll __ll+2
  LOCALVAR %1
%endmacro
%define localdw LOCALDW
 
%macro LOCALDD 1
  %assign __ll __ll+4
  LOCALVAR %1
%endmacro
%define localdd LOCALDD
 
;Reserva una cantidad fija de bytes en la pila
%macro LOCALBUF 1-2
  %assign __ll __ll+%2
  LOCALVAR %1
%endmacro
%define localbuf LOCALBUF
 
;Fin de la declaracion de variables locales
%macro LOCALEND 0
 ;Reservamos memoria en la pila, procurando dejarla alineada a 4
    sub esp, __ll+(0x4-(__ll & 0x03))
%endmacro
%define ENDL LOCALEND
%define endl LOCALEND
 
;----------Data Declarations
;Usage:
;CHAR Name, String
;INT Name, Byte
;WORD Name, Word
;DWORD Name, Dword
;e.g.    CHAR  UserName, "Joe User"
;        INT   Timeout,  30
;        WORD  Logins
;        DWORD Password
;Note that not specifying a value causes the variable to default to 0
;Note that strings are automatically zero-terminated
%macro CHAR 1-2 0
%1: DB   %2,0
%endmacro
 
%macro INT 1-2 0
%1: DB   %2
%endmacro
 
%macro WORD 1-2 0
%1: DW   %2
%endmacro
 
%macro DWORD 1-2 0
%1: DD   %2
%endmacro
;Macro de importacion
 
%macro IMPORT 1-2
   %define symbolo %1
  %define %[symbolo] _%[symbolo]
   
    %defstr arg2_ %2
    %strlen arg2_len arg2_
   
    %if arg2_len
        %define %[symbolo] %[symbolo]@%2
    %endif
   
    %undef arg2_len
 %undef arg2_
     
    ;%endrep
    %undef symbolo
  ;extern %1
   
%endmacro
%define import IMPORT
 
%macro VCALL 2;puntero this y nombre
  mov ecx,%1
  mov eax,[ecx]
   mov eax,[eax+%2]
    push ecx
    call eax
    pop ecx
  
%endmacro
 
%macro ECALL 1
 extern %1
   mov %$_EDX,edx
  mov %$_ECX,ecx
  call %1
 mov edx,%$_EDX
  mov ecx,%$_ECX
%endmacro
%macro ECALL2 1
 ;extern %1
   mov %$_EDX,edx
  mov %$_ECX,ecx
  call %1
 mov edx,%$_EDX
  mov ecx,%$_ECX
%endmacro
 
 
%macro TCALL 1
    
%endmacro
%macro CCALL 1
  
%endmacro
%macro FCALL 1
  
%endmacro
%define ecall ECALL
%define ecall2 ECALL2
 
%macro class 1-99
%push CClass
   %define defclass %1
 ;%define father %2
  %assign _i 4
    %assign vpointer 0
  %assign sz 0
    %assign vtable 0
    ;%if father
 ;%endif
 %rep %0-1
 ;%assign _i _i+4
    %define father %2
  %if
    %define %[defclass].[%2] %[sz]+vpointer
    %endif
     %assign sz sz+sizeof(father)
 %rotate 1
 %endrep
%endmacro
%macro endclass 0
  ;asigno un tamaño a la clase
    %[defclass]_size equ    sz+vpointer
 %[defclass]_vtable equ  vtable
   
    %undef defclass
 %ifdef father
       %undef father
   %endif
  %pop
%endmacro
%macro static 1
     
%endmacro
 
%macro pclass 1
     
%endmacro
%macro endpclass 0
  
%endmacro
 
%macro miembro 1-2
 %[defclass].%[%2]_size  equ sizeof(%1)
  %define %[defclass].%[%2]       %[sz]+vpointer
   
    %ifdef %[defclass]_member
       %define %[defclass]_member %[defclass]_member, %[%2]
    %else
       %define %[defclass]_member %[%2]
    %endif
  %assign sz sz+sizeof(%1)
     
%endmacro
%macro virtual 1
   %assign vpointer 4
  %ifdef father
       %ifdef %[father].%[%1]
          %define %[defclass].%[%1] %[father].%[%1]
       %else
           %define %[defclass].%[%1] %[vtable]
     %endif
  %else
       %define %[defclass].%[%1] %[vtable]
 %endif
   
    %ifdef %[defclass]_proc
     %define %[defclass]_proc %[defclass]__proc, %[%1]
   %else
       %define %[defclass]__proc %[%1]
 %endif
  %assign vtable vtable+4
 ;GLOBAL %[defclass].%[%1]_proc
%endmacro
 
%define ends endstruc
 
%macro StackAlloc 1
  
    mov ecx,%1
  and cl,0xfc
 add ecx,4
   sub esp,ecx
  
%endmacro
 
%endif