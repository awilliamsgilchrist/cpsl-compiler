.text
main:
la $gp, GA
li $v0, 4
la $a0, STR1
syscall
li $v0, 1
li $a0, 4
syscall
li $v0, 11
li $a0, 10
syscall
li $v0, 4
la $a0, STR2
syscall
li $v0, 1
li $a0, 4
syscall
li $v0, 11
li $a0, 10
syscall
li $v0, 4
la $a0, STR3
syscall
li $v0, 1
li $a0, 1
syscall
li $v0, 11
li $a0, 10
syscall
li $v0, 4
la $a0, STR4
syscall
li $v0, 1
li $a0, 0
syscall
li $v0, 11
li $a0, 10
syscall
li $v0, 4
la $a0, STR5
syscall
li $v0, 5
syscall
move $t0, $v0
sw $t0, 0($gp)
li $v0, 4
la $a0, STR6
syscall
li $v0, 5
syscall
move $t0, $v0
sw $t0, 4($gp)
li $v0, 11
li $a0, 10
syscall
lw $t8, 0($gp)
lw $t7, 4($gp)
add $t8, $t8, $t7
sw $t8, 8($gp)
li $v0, 4
la $a0, STR7
syscall
li $v0, 1
lw $a0, 8($gp)
syscall
li $v0, 11
li $a0, 10
syscall
lw $t7, 0($gp)
lw $t8, 4($gp)
sub $t7, $t7, $t8
sw $t7, 12($gp)
li $v0, 4
la $a0, STR8
syscall
li $v0, 1
lw $a0, 12($gp)
syscall
li $v0, 11
li $a0, 10
syscall
lw $t8, 0($gp)
lw $t7, 4($gp)
mult $t8, $t7
mflo $t8
sw $t8, 16($gp)
li $v0, 4
la $a0, STR9
syscall
li $v0, 1
lw $a0, 16($gp)
syscall
li $v0, 11
li $a0, 10
syscall
lw $t7, 0($gp)
lw $t8, 4($gp)
div $t7, $t8
mflo $t7
sw $t7, 20($gp)
li $v0, 4
la $a0, STR10
syscall
li $v0, 1
lw $a0, 20($gp)
syscall
li $v0, 11
li $a0, 10
syscall
lw $t8, 2($gp)
lw $t7, 4($gp)
div $t8, $t7
mfhi $t8
sw $t8, 24($gp)
li $v0, 4
la $a0, STR11
syscall
li $v0, 1
lw $a0, 24($gp)
syscall
li $v0, 11
li $a0, 10
syscall
lw $t7, 0($gp)
lw $t8, 4($gp)
beq $t7, $t8, a6
li $t7, 0
j a7
a6: li $t7, 1
a7: 
sw $t7, 28($gp)
li $v0, 4
la $a0, STR12
syscall
li $v0, 1
lw $a0, 28($gp)
syscall
li $v0, 11
li $a0, 10
syscall
lw $t8, 0($gp)
lw $t7, 4($gp)
sub $t8, $t8, $t7
bltz $t8, a8
li $t8, 0
j a9
a8: li $t8, 1
a9: 
sw $t8, 32($gp)
li $v0, 4
la $a0, STR13
syscall
li $v0, 1
lw $a0, 32($gp)
syscall
li $v0, 11
li $a0, 10
syscall
lw $t7, 0($gp)
lw $t8, 4($gp)
sub $t7, $t7, $t8
bltz $t7, a10
li $t7, 1
j a11
a10: li $t7, 0
a11: 
sw $t7, 36($gp)
li $v0, 4
la $a0, STR14
syscall
li $v0, 1
lw $a0, 36($gp)
syscall
li $v0, 11
li $a0, 10
syscall
lw $t8, 1993721092($gp)
lw $t7, 4($gp)
bne $t8, $t7, a12
li $t8, 0
j a13
a12: li $t8, 1
a13: 
sw $t8, 40($gp)
li $v0, 4
la $a0, STR15
syscall
li $v0, 1
lw $a0, 40($gp)
syscall
li $v0, 11
li $a0, 10
syscall
lw $t7, 0($gp)
lw $t8, 4($gp)
sub $t7, $t7, $t8
blez $t7, a14
li $t7, 0
j a15
a14: li $t7, 1
a15: 
sw $t7, 44($gp)
li $v0, 4
la $a0, STR16
syscall
li $v0, 1
lw $a0, 44($gp)
syscall
li $v0, 11
li $a0, 10
syscall
lw $t8, 0($gp)
lw $t7, 4($gp)
sub $t8, $t8, $t7
blez $t8, a16
li $t8, 1
j a17
a16: li $t8, 0
a17: 
sw $t8, 48($gp)
li $v0, 4
la $a0, STR17
syscall
li $v0, 1
lw $a0, 48($gp)
syscall
li $v0, 11
li $a0, 10
syscall
.data
STR0: .asciiz 
STR1: .asciiz "2+2="
STR2: .asciiz "2*2="
STR3: .asciiz "2/2="
STR4: .asciiz "2-2="
STR5: .asciiz "Enter A:"
STR6: .asciiz "\nEnter b:"
STR7: .asciiz "a+b="
STR8: .asciiz "a-b="
STR9: .asciiz "a*b="
STR10: .asciiz "a/b="
STR11: .asciiz "a%b="
STR12: .asciiz "a=b="
STR13: .asciiz "a<b="
STR14: .asciiz "a>b="
STR15: .asciiz "a<>b="
STR16: .asciiz "a<=b="
STR17: .asciiz "a>=b="
GA: .align 2
