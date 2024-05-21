  LDI 1
loop:
  ADD value
  STA value
  OUT
  JC $0
  JMP loop

  .org $e
value: 0
