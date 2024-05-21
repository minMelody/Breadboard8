top:
  LDA product
  ADD y
  STA product
  LDA x
  SUB decrement
  STA x
  JZ  end
  JMP top
end:
  LDA product
  OUT
  HLT

decrement: 1

  .org $d
product: 0
x: 5
y: 7
