.text
main:
la $gp, GA
li $v0, 4
la $a0, STR1
syscall
.data
STR0: .asciiz 
STR1: .asciiz "Hello World!"
GA:
