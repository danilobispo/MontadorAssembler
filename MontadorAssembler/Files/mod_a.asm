;Codigo fonte do modulo A

Y: EXTERN	;Esse � um coment�rio separado por tab
MOD_B: 
EXTERN  ;Esse � um coment�rio separado por dois espa�os
PUBLIC VAL
PUBLIC L1
SECTION TEXT
INPUT Y
ROT2: 
		LOAD VAL
ADD Y; loko demais
STORE Y+2
JMPP MOD_B
L1: STOP
SECTION DATA
VAL: CONST 0x5
