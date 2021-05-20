.text
main:
	addi $t0, $zero, 1
  addi $t1, $zero, 2
  addi $t2, $zero, 3
  
  lui	$t3, 0x1000
	lw	$t4, 0($t3)
  add $t3, $t4, $t2 # d_stall = 1

  lui	$t3, 0x1000
	lw	$t4, 0($t3)
  addi $t3, $t4, 100 # d_stall = 2

  lui	$t3, 0x1000
	lw	$t4, 0($t3)
  beq $t3, $t4, Label # d_stall = 4

  lui	$t3, 0x1000
	lw	$t4, 0($t3)
  add $t5 $t1, $t2
  beq $t3, $t4, Label # d_stall = 5

  add $t3, $t1, $t2
  beq $t3, $t4 Label # d_stall = 6

  addi $v0,$zero,10
  syscall         # exit()

Label:
  addi $v0,$zero,10
  syscall         # exit()
	
.data 0x10000000
	.word	1, 2, 0 