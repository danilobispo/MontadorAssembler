L1:	EQU 1
L2: EQU 1
LOAD N
SECTION TEXT ;comentarios
TESTE: MACRO &A, &B, &C
       LOAD &A
       INPUT &B
       INPUT &C
       ENDMACRO
IF L1
LOAD N ;faz esta opera��o se L1 for verdadeiro
IF L2
INPUT N ;faz esta opera��o se L2 for verdadeiro
TESTE X, Y, Z
IF L1
LOAD N ;faz esta opera��o se L1 for verdadeiro
IF L2
INPUT N ;faz esta opera��o se L2 for verdadeiro
INPUT X+L2

SECTION DATA
N: SPACE
x: SPACE 3