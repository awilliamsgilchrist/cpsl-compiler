.text
main:
la $gp, GA
li $v0, 5
syscall
sw $v0, 0($gp)
li $v0, 12
syscall
sw $v0, 4($gp)
li $v0, 1
lw $a0, 0($gp)
syscall
li $v0, 11
lw $a0, 4($gp)
syscall
li $v0, 1
li $a0, 1
syscall
li $v0, 1
li $a0, 0
syscall
.data
STR0: .asciiz 
GA: .align 2