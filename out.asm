.text
main:
la $gp, GA
li $t8, 42
sw $t8, 0($gp)
li $v0, 1
lw $a0, 0($gp)
syscall
.data
STR0: .asciiz 
GA: