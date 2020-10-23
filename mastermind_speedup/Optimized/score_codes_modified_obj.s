# -----------------------------------------------------------------------------
# ISAAC LEHMAN
#
# A 64-bit function that scores a guess code against the secret code (6 pegs!)
# The function has signature:
#
#       void scoreCodes( Code secretCode,  CodePtr guessCode)
#
# Note that the parameters have already been passed in rdi, rsi.
#   rdi = secretCode
#   rsi = guessCode (this is the actual tha we manipulate and "returned")
#       - We have pointer access
#
# Other Variables:
#    rdi = i
#    rax = j
#    rdx, edx, dil = temp variable
#    rsp + 0x8 + i = secretCode[i] (temporary stack version)
#    rsp + 0x10 + i = guessCode[i] (temporary stack version)
#
# Note:
#   - 0x10 = 16
# -----------------------------------------------------------------------------
        .text
.globl  scoreCodes
scoreCodes:                      
    sub    $0x28,%rsp                       # Allocates memory on stack (local vars)
    mov    %rdi,0x8(%rsp)                   # Put secret code on the stack at %rsp + 8
    mov    (%rsi),%rdx                      # Put guess code into temp register
    mov    %rdx,0x10(%rsp)                  # Put guess code (%rax) on the stack at %rsp + 16 -> Code tempCode = (*guessCode); 
    movb   $0x0,0x7(%rsi)                   # Set numBlack = 0 of guessCode
    movb   $0x0,0x6(%rsi)                   # Set numWhite = 0 of guessCode
    jmp    count_blacks_unrolled_start      # jump to start of count blacks


# COUNT BLACKS SECTION
count_blacks_unrolled_start:                # for i = 0, 0x10 = 16, 0x8 = 8
    movzb  0x10(%rsp),%edx                  # temp for guessCode[i]
    cmp    %dl,0x8(%rsp)                    # does guessCode[i] == secretCode[i]
    jne    count_blacks_unrolled_1          # if not equal jump to next spot in loop: i = 1  
    incb   0x7(%rsi)                        # Add 1 to numBlacks
    movb   $0x7,0x10(%rsp)                  # guessCode[i] = BOGUS_GUESS = 7
    movb   $0x0,0x8(%rsp)                   # secretCode[i] = BOGUS_SECRET = 0   
    jmp    count_blacks_unrolled_1          # jump to next spot in loop: i = 1

count_blacks_unrolled_1:                    # for i = 1, 0x11 = 17, 0x9 = 9
    movzb  0x11(%rsp),%edx                  # temp for guessCode[i]
    cmp    %dl,0x9(%rsp)                    # does guessCode[i] == secretCode[i]
    jne    count_blacks_unrolled_2          # if not equal jump to next spot in loop: i = 2   
    incb   0x7(%rsi)                        # Add 1 to numBlacks
    movb   $0x7,0x11(%rsp)                  # guessCode[i] = BOGUS_GUESS = 7
    movb   $0x0,0x9(%rsp)                   # secretCode[i] = BOGUS_SECRET = 0   
    jmp    count_blacks_unrolled_2          # jump to next spot in loop: i = 2

count_blacks_unrolled_2:                    # for i = 2, 0x12 = 18, 0xa = 10
    movzb  0x12(%rsp),%edx                  # temp for guessCode[i]
    cmp    %dl,0xa(%rsp)                    # does guessCode[i] == secretCode[i]
    jne    count_blacks_unrolled_3          # if not equal jump to next spot in loop: i = 3 
    incb   0x7(%rsi)                        # Add 1 to numBlacks
    movb   $0x7,0x12(%rsp)                  # guessCode[i] = BOGUS_GUESS = 7
    movb   $0x0,0xa(%rsp)                   # secretCode[i] = BOGUS_SECRET = 0
    jmp    count_blacks_unrolled_3          # jump to next spot in loop: i = 3

count_blacks_unrolled_3:                    # for i = 3, 0x13 = 19, 0xb = 11 
    movzb  0x13(%rsp),%edx                  # temp for guessCode[i]
    cmp    %dl,0xb(%rsp)                    # does guessCode[i] == secretCode[i]
    jne    count_blacks_unrolled_4          # if not equal jump to next spot in loop: i = 4
    incb   0x7(%rsi)                        # Add 1 to numBlacks
    movb   $0x7,0x13(%rsp)                  # guessCode[i] = BOGUS_GUESS = 7
    movb   $0x0,0xb(%rsp)                   # secretCode[i] = BOGUS_SECRET = 0
    jmp    count_blacks_unrolled_4          # jump to next spot in loop: i = 4

count_blacks_unrolled_4:                    # for i = 4, 0x14 = 20, 0xc = 12 
    movzb  0x14(%rsp),%edx                  # temp for guessCode[i]
    cmp    %dl,0xc(%rsp)                    # does guessCode[i] == secretCode[i]
    jne    count_blacks_unrolled_5          # if not equal jump to next spot in loop: i = 5
    incb   0x7(%rsi)                        # Add 1 to numBlacks
    movb   $0x7,0x14(%rsp)                  # guessCode[i] = BOGUS_GUESS = 7
    movb   $0x0,0xc(%rsp)                   # secretCode[i] = BOGUS_SECRET = 0
    jmp    count_blacks_unrolled_5          # jump to next spot in loop: i = 5

count_blacks_unrolled_5:                    # for i = 5, 0x15 = 21, 0xd = 13 
    movzb  0x15(%rsp),%edx                  # temp for guessCode[i]
    cmp    %dl,0xd(%rsp)                    # does guessCode[i] == secretCode[i]
    jne    count_whites_loop_setup          # if not equal jump out of loop
    incb   0x7(%rsi)                        # Add 1 to numBlacks
    movb   $0x7,0x15(%rsp)                  # guessCode[i] = BOGUS_GUESS = 7
    movb   $0x0,0xd(%rsp)                   # secretCode[i] = BOGUS_SECRET = 0
    jmp    count_whites_loop_setup          # jump out of loop


# COUNT WHITES SECTION
count_whites_loop_setup:
    xor    %rdi,%rdi                        # set i = 0
    xor    %rax,%rax                        # set j = 0
    jmp count_whites_inner_loop             # jump to inner part of loop
    
    # outer loop 
count_whites_outer_loop:
    inc    %rdi                             # i++
    cmp    $0x5,%rdi                        # does i == 5 (IS END OF LOOP?)
    jg     done                             # if i > 5 (you are done with the function now!)
    xor    %rax,%rax                        # set j = 0
    jmp    count_whites_inner_loop          # jump to the inner loop over j

    # inner loop 
count_whites_inner_loop:
    cmp    $0x5,%rax                        # does j == 5?
    jg     count_whites_outer_loop          # jump back to outer loop
    movzb  0x10(%rsp, %rax),%edx            # temp for guessCode[i]
    cmp    %dl,0x8(%rsp, %rdi)              # guessCode[j] == secretCode[i]
    je     count_whites_true                # Jump to true case
    inc    %rax                             # j++
    jmp    count_whites_inner_loop          # go to top of inner loop

    # what happens when true
count_whites_true:
    incb   0x6(%rsi)                        # Add 1 to numWhite
    movb   $0x7,0x10(%rsp,%rax)             # guessCode[j]  = BOGUS_GUESS = 7
    movb   $0x0,0x8(%rsp,%rdi)              # secretCode[i] = BOGUS_SECRET = 0
    jmp    count_whites_outer_loop          # jump back to outer loop (break)

done:
    add    $0x28,%rsp                       # free memory on stack
    retq                                    # end of function – return to caller
