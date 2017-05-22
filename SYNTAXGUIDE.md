# Syntax guide

Labels and comments are not currently supported but this might change in the future. 


## Addressing modes 

- Each instruction must be on a new line.   
- The instruction must be the first element on the line (though it can be preceded by white-space characters).   
- The instruction should be followed by at least one white-space character. 
- Numerical arguments must be given in hexadecimal (without the "0x" prefix).
- Numerical arguments must be preceded by the character '$'.
- 1-byte arguments should be written with 2 digits and 2-bytes arguments should be written with 4 digits.
- Commas and parantheses can be preceded and followed by whitespaces.

  
**The instructions used in the following examples are simply possible instructions. The addressing modes can be used with different instructions and the instructions can be used with different addressing modes.**  
   
  
**Absolute**   
LDX $0000   
(Instruction followed by 2 bytes with the character $ as prefix)  

**Absolute indexed with X**   
ADC $0000,X   
(Instruction followed by 2 bytes with the character $ as prefix, followed by a comma, followed by the charcter X)   

**Absolute indexed with Y**   
ADC $0000,Y   
(Instruction followed by 2 bytes with the character $ as prefix, followed by a comma, followed by the charcter Y)   

**Accumulator**   
ASL   
(Instruction followed by nothing or whitespace)   
(The syntax is the same as Implied but, for any instruction, only one of the two is a valid addressing mode)

**Immediate**   
LDA #$00   
(Instruction followed by 1 byte with the character # and the character $ as prefixes)   

**Implied**   
TXA    
(Instruction followed by nothing or whitespace)   

**Indirect**   
JMP ($0000)   
(Instruction followed by 2 bytes with the character $ as prefix, all within parantheses)   

**Relative**   
BPL $00      
(Instruction followed by 1 byte with the character $ as prefix)   
(The synthax is the same as Zero page but it is only used along with branching instructions)

**Zero page**   
LDY $00   
(Instruction followed by 1 byte with the character $ as prefix)    

**Zero page indexed with X**   
LDA $00,X   
(Instruction followed by 1 byte with the character $ as prefix, followed by a comma, followed by the charcter X)   

**Zero page indexed with Y**   
LDA $00,Y   
(Instruction followed by 1 byte with the character $ as prefix, followed by a comma, followed by the charcter Y)   

**Zero page indexed indirect with X**   
STA ($00,X)   
(Instruction followed by 1 byte with the character $ as prefix, followed by a comma, followed by the charcter X, all within parantheses)   

**Zero page indirect indexed with Y**   
LSR ($00),Y   
(Instruction followed by 1 byte with the character $ as prefix both within parantheses, followed by a comma, followed by the charcter Y)   




##Instructions   

Any of the following is considered a valid instruction :      

**ADC**    
Add with carry 

**AND**    
Bitwise AND with accumulator   

**ASL**   
Arithmetic shift left   

**BCC**  
Branch on carry clear   

**BCS**  
Branch on carry set   

**BEQ**  
Branch on equal (zero set)   

**BIT**  
Bit test   

**BMI**  
Branch on minus (negative set)   

**BNE**  
Branch on not equal (zero clear)   

**BPL**  
Branch on plus (negative clear)   

**BRK**  
Interrupt   

**BVC**  
Branch on overflow clear   

**BVS**  
Branch on overflow set    

**CLC**  
Clear carry   

**CLD**  
Clear decimal   

**CLI**  
Clear interrupt disable   

**CLV**  
Clear overflow   

**CMP**  
Compare with accumulator   

**CPX**  
Compare with X   

**CPY**  
Compare with Y   

**DEC**  
Decrement  

**DEX**  
Decrement X   

**DEY**  
Decrement Y   

**EOR**  
Bitwise XOR with accumulator   

**INC**  
Increment   

**INX**  
Increment X   

**INY**  
Increment Y   

**JMP**  
Jump   

**JSR**  
Jump to subroutine   

**LDA**  
Load into accumulator   

**LDY**  
Load into X   

**LDY**  
Load into Y   

**LSR**  
Logical shift right   

**NOP**  
No operation   

**ORA**  
Bitwise OR with accumulator   

**PHA**  
Push accumulator to stack   

**PHP**  
Push processor status to stack   

**PLA**  
Pull accumulator from stack   

**PLP**  
Pull processor status from stack   

**ROL**  
Rotate bits left   

**ROR**  
Rotate bits right   

**RTI**  
Return from interrupt   

**RTS**  
Return from subroutine   

**SBC**  
Subtract with carry   

**SEC**  
Set carry   

**SED**  
Set decimal   

**SEI**  
Set interrupt disable   

**STA**  
Store accumulator   

**STX**  
Store X   

**STY**  
Store Y   

**TAX**  
Transfer accumulator to X   

**TAY**  
Transfer accumulator to Y   

**TSX**  
Transfer stack pointer to X   

**TXA**  
Transfer X to accumulator   

**TXS**  
Transfer X to stack pointer   

**TYA**  

Transfer Y to accumulator   
