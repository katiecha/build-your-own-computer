.text
main:
    li $t0, 5 # set t0=5
    li $t1, 6 # set t1=6
    li $t2, 0 # set t2=0 - register to store N
    li $t3, 1 # set t3=0 - register to act as counter
    li $t4, 1 # set t4=1 - register to act as "number"
    li $t5, 0 # set t5=0 - register to store "mod values"
    
    li $v0, 4 # syscall code for printing string
    la $a0, user_prompt
    syscall

    li $v0, 5 # syscall code for reading in integer
    syscall
    move $t2, $v0 # moves input into $t2

_while_loop:
    bgt $t3, $t2, _end_loop # while(counter<=N)

    div $t5, $t4, $t0 # divide number by 5 and store into $t5
    mfhi $t5 # move from Hi - div puts remainder in Hi bits
    beqz $t5, _divisible # branch if equal to 0

    div $t5, $t4, $t1 # divide number by 6 and store into $t5
    mfhi $t5 # move from Hi - div puts remainder in Hi bits
    beqz $t5, _divisible # branch if equal to 0

    addi $t4, $t4, 1 # increment "number"
    j _while_loop # go back to beginning of while loop

_divisible:
    li $v0, 1 # syscall code for printing integer
    move $a0, $t4
    syscall

    li $v0, 4 # syscall code for printing newline
    la $a0, newline
    syscall

    addi $t3, $t3, 1 # increment counter
    addi $t4, $t4, 1 # increment "number"

    j _while_loop # go back to beginning of while loop

_end_loop:
    li $v0, 0 # return EXIT_SUCCESS
    jr $ra

.data
    # a literal string for prompting the user
    user_prompt: .asciiz "Please enter the input: "
    newline: .asciiz "\n"
