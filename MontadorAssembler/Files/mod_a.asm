;Co

Y: EXTERN	;c� � � � � s � 

MOD_B     : 
EXTERN  ;co
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
L3: COPY Aaaa, Beeech
SECTION DATA
VAL: CONST 0x5
