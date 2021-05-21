.text

main:
  lui$t0, 0x1000
  addi$s0, $0, 4
  sw$s0, 4($t0)
  lw$s1, 0($t0)
  lw$s1, 24($t0)
  sw$s1, 32($t0)
  add $t0, $t0, $s1
  addi$v0, $0, 10
  syscall

.data 0x10000000
  .word 1, 2, 5, 9, 7, 0, 64, 0, 8, 5, 1, 2