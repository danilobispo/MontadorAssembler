MOD_A: BEGIN
EXTERN MOD_B
PUBLIC X
EXTERN Y
EXTERN W
PUBLIC N
SECTION TEXT ;comentarios
LOAD N
LOAD Y ;faz esta operacao se L1 for verdadeiro
INPUT N ;faz esta operacao se L2 for verdadeiro
LOAD N ;faz esta opercaao se L1 for verdadeiro
INPUT N ;faz esta operacao se L2 for verdadeiro
INPUT X
SUB W
ADD Y
MULT W
STOP
SECTION DATA
N: SPACE
x: SPACE 3

END