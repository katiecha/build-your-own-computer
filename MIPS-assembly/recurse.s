.text
main:
    addi $sp, $sp, -12 # opens frame
    sw $ra, 0($sp) # return address
    sw $s0, 4($sp) # register to store N
    sw $s1, 8($sp) # register to store result

    li $s0, 0 # set s0=0

    li $v0, 4 # syscall code for printing string
    la $a0, user_prompt
    syscall

    li $v0, 5 # syscall code for reading in integer
    syscall
    move $s0, $v0 # moves input into $s0
    
    move $a0, $s0 # moves input into arg register
    jal f # calls function
    move $s1, $v0 # returns result and moves into $s1

    li $v0, 1 # syscall code for printing integer (result in $s1)
    move $a0, $s1
    syscall

    li $v0, 4 # syscall code for printing newline
    la $a0, newline
    syscall

    lw $ra, 0($sp)
    lw $s0, 4($sp)
    lw $s1, 8($sp)
    addi $sp, $sp, 12 # closes frame
    li $v0, 0 # return EXIT_SUCCESS
    jr $ra

f: 
    addi $sp, $sp, -12 # opens frame for current recursive call
    sw $ra, 0($sp)
    sw $s0, 4($sp)

    li $v0, 2 # set $v0 equal to 2 for the base case
    # base case
    move $t2, $a0 # pass integer into function
    beqz $t2, func_exit # if N=0, exit

    # recursive call
    addi $a0, $a0, -1 # N-1
    sw $t2, 8($sp)
    jal f
    lw $t2, 8($sp)

    li $s3, 2
    mul $s2, $s3, $t2 # result = 2*N
    add $s2, $s2, $v0 # result = result + f(N-1)
    addi $s2, $s2, -1 # result = result - 1

    move $v0, $s2 # return result

func_exit:
    lw $ra, 0($sp)
    lw $s0, 4($sp)
    addi $sp, $sp, 12 # closes frame for current recursive call
    jr $ra

.data
    # a literal string for prompting the user
    user_prompt: .asciiz "Please enter the input: "
    newline: .asciiz "\n"