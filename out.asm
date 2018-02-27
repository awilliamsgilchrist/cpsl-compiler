.text
main:
la $gp, GA
li $t8, 42
sw $t8, 0($gp)
li $v0, 1
li $a0, 0
syscall
.data
STR0: .asciiz 
GA: