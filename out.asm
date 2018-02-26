.text
li $v0, 4
la $a0, STR0
syscall
.data
STR0: .asciiz "Hello World!"
