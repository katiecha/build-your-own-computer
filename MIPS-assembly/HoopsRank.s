.text
.align 2

# ----------------------------------------main----------------------------------------
main:
    addi $sp, $sp, -36 # opens frame
    sw $ra, 0($sp) # return address
    sw $s0, 4($sp)
    sw $s1, 8($sp)
    sw $s2, 12($sp)
    sw $s3, 16($sp)
    sw $s4, 20($sp)
    sw $s5, 24($sp)
    sw $s6, 28($sp)
    sw $s7, 32($sp)
    li $s4, 0

    # ----------allocate space for dummy head node----------
    li $v0, 9 # syscall code for sbrk (malloc)
    li $a0, 72 # 64 bytes for name, 4 bytes for metric, 4 bytes for pointer
    syscall

    move $t6, $v0 # move address for space into $t6

# ----------------------------------------get input----------------------------------------
_get_input:
    # ----------allocate space----------
    li $v0, 9 # syscall code for sbrk (malloc)
    li $a0, 72 # 64 bytes for name, 4 bytes for metric, 4 bytes for pointer
    syscall

    move $t0, $v0 # move address for space into $t0

    # ----------prompting for name----------
    li $v0, 4 # syscall code for printing string
    la $a0, name_prompt # "Team name:"
    syscall # prints out "Team name:"
    
    # ----------putting string into address----------
    li $v0, 8 # syscall code for read string
    move $a0, $t0 # moves $t0 (name) into $a0
    li $a1, 64 # max of 64 chars
    syscall

    move $t2, $t0 # storing address of linked list

    # ----------replaces new line with space---------- 
    la $a1, done # load the address of "DONE" into $a1

_newline_to_space:
    lb $t8, 0($t2) # loads byte
    li $t9, 10 # loads 10 (newline) into $t9
    beq $t8, $t9, _is_newline # if equal, branch
    addi $t2, $t2, 1 # else, increment
    j _newline_to_space # loop

_is_newline:
    li $t9, 32 # load 32 (space) into $t9
    sb $t9, 0($t2) # replaces newline with space for output

    la $a1, done # load the address of "DONE" into $a1

    # ----------strcmp----------
    li $s7, 0 # boolean
    j _strcmp # calls strcmp(name, DONE)

_end_strcmp:
    move $t3, $v0 # move $v0 (result) into $t3 --> result is 0 if equal, 1 if greater than, -1
    beqz $t3, _end_loop_for_get_input # if equal, "DONE" entered, end input loop
    
    # ----------gets apgs----------
    li $v0, 4 # syscall code for printing string 
    la $a0, apgs_prompt
    syscall

    li $v0, 5 # syscall code for reading int (apgs)
    syscall
    move $s1, $v0 # moves input into $s1

    # ----------gets apgg----------
    li $v0, 4 # syscall code for printing string 
    la $a0, apgg_prompt
    syscall

    li $v0, 5 # syscall code for reading int (apgs)
    syscall
    move $s2, $v0 # moves input into $s2

    # ----------gets ard----------
    li $v0, 4 # syscall code for printing string 
    la $a0, ard_prompt
    syscall

    li $v0, 5 # syscall code for reading int (apgs)
    syscall
    move $s3, $v0 # moves input into $s3

    # ----------set metric----------
    move $a0, $s1 # apgs
    move $a1, $s2 # apgg
    move $a2, $s3 # ard
    j _metric_calc # calls metric calculator

# ----------------------------------------return----------------------------------------
_end_loop_for_get_input: # exits loop getting input
    lw $s0, 68($t6)
    move $a0, $s0
    j _loop_for_print

_come_back:
    lw $ra, 0($sp)
    lw $s0, 4($sp)
    lw $s1, 8($sp)
    lw $s2, 12($sp)
    lw $s3, 16($sp)
    lw $s4, 20($sp)
    lw $s5, 24($sp)
    lw $s6, 28($sp)
    lw $s7, 32($sp)
    addi $sp, $sp, 36 # closes frame
    li $v0, 0 # return EXIT_SUCCESS
    jr $ra

# HELPER FUNCTIONS
# ----------------------------------------sort----------------------------------------
_pre_sort:
    move $t5, $t6

_sort:
    move $t4, $t5
    lw $t5, 68($t5) # setting current to next node 

    beqz $t5, _insert_node # loading in next field equal to zero (base case | null)

    lw $s0, 64($t5) # s0 = first node metric
    lw $s1, 64($t0) # s1 = insert node metric

    li $s7, 1
    
    move $a0, $t0
    move $a1, $t5
    bgt $s1, $s0, _insert_node

    beq $s0, $s1, _strcmp
    
    j _sort

    # head node = $t6
    # last node = $t4
    # insert node = $t0
    # current node = $t5

_insert_node:
    # making insert node's next = next field of last ptr
    lw $t1, 68($t4)
    sw $t1, 68($t0)

    # making next field of last ptr = current node inserting
    sw $t0, 68($t4)
    j _get_input

# ----------------------------------------metric calculator----------------------------------------
_metric_calc:
    sub $t8, $a0, $a1 # result = (avg points scored - avg points given up)
    add $t8, $t8, $a2 # result = (avg points scored - avg points given up) + avg rebounding diff
    move $v0, $t8
    sw $v0, 64($t0) # stores result after team name in linked list
    j _pre_sort # loop

# ----------------------------------------string compare---------------------------------------- 
_strcmp:
    #strcmp: returns zero if equal, 1 if "greater than", -1 if "less than"
    move $s6, $a0 # word 1
    move $t1, $a1 # word 2

_loop_for_strcmp:
    lb $t2, 0($s6)
    lb $t3, 0($t1)

    bne $t2, $t3, _str_not_equal
    add $t2, $t2, $t3
    beq $t2, $zero, _str_equal

    addi $s6, $s6, 1
    addi $t1, $t1, 1

    j _loop_for_strcmp

_str_not_equal:
    sub $s5, $t2, $t3
    move $v0, $s5 # return - for "less than" or + for "greater than"
    beqz $s7, _end_strcmp
    bltz $v0, _insert_node
    j _sort

_str_equal:
    li $v0, 0 # return 0 for "equal to"
    j _end_loop_for_get_input

# ----------------------------------------print list----------------------------------------
_loop_for_print:
    beq $a0, $zero, _end_loop_for_print

    move $s4, $a0

    li $v0, 4 # syscall code for printing string (team name)
    la $a0, 0($s4)
    syscall

    li $v0, 1 # syscall code for printing int (metric)
    lw $a0, 64($s4)
    syscall

    li $v0, 4 # syscall code for printing string (\n)
    la $a0, newline
    syscall

    lw $s4, 68($s4)
    move $a0, $s4
    j _loop_for_print

_end_loop_for_print:
    j _come_back

.data
    # literal strings for prompting the user
name: 
    .align 2 
    .space 64 # space for a name of up to 63 chars + \n
name_prompt: 
    .align 2 
    .asciiz "Team name: "
apgs_prompt: 
    .align 2 
    .asciiz "Average number of points scored per game: "
apgg_prompt: 
    .align 2 
    .asciiz "Average number of points given up per game: "
ard_prompt: 
    .align 2 
    .asciiz "Average rebounding differential: "
newline: 
    .align 2 
    .asciiz "\n"
done: 
    .align 2 
    .asciiz "DONE "
