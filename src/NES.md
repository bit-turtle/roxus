# Progress on NES emulator

|      | 0        | 1        | 2       | 3 | 4        | 5        | 6        | 7 | 8        | 9        | A        | B | C        | D        | E        | F |
|------|----------|----------|---------|---|----------|----------|----------|---|----------|----------|----------|---|----------|----------|----------|---|
| 0x00 | BRK_impl | ORA_xind |         |   |          | ORA_zpg  | ASL_zpg  |   | PHP_impl | ORA_imm  | ASL_acc  |   |          | ORA_abs  | ASL_abs  |   |
| 0x10 | BPL_rel  | ORA_indy |         |   |          | ORA_zpgx | ASL_zpgx |   | CLC_impl | ORA_absy |          |   |          | ORA_absx | ASL_absx |   |
| 0x20 | JSR_abs  | AND_xind |         |   | BIT_zpg  | AND_zpg  | ROL_zpg  |   | PLP_impl | AND_imm  | ROL_acc  |   | BIT_abs  | AND_abs  | ROL_abs  |   |
| 0x30 | BMI_rel  | AND_indy |         |   |          | AND_zpgx | ROL_zpgx |   | SEC_impl | AND_absy |          |   |          | AND_absx | ROL_absx |   |
| 0x40 | RTI_impl | EOR_xind |         |   |          | EOR_zpg  | LSR_zpg  |   | PHA_impl | EOR_imm  | LSR_acc  |   | JMP_abs  | EOR_abs  | LSR_abs  |   |
| 0x50 | BVC_rel  | EOR_indy |         |   |          | EOR_zpgx | LSR_zpgx |   | CLI_impl | EOR_absy |          |   |          | EOR_absx | LSR_absx |   |
| 0x60 | RTS_impl | ADC_xind |         |   |          | ADC_zpg  | ROR_zpg  |   | PLA_impl | ADC_imm  | ROR_acc  |   | JMP_ind  | ADC_abs  | ROR_abs  |   |
| 0x70 | BVS_rel  | ADC_indy |         |   |          | ADC_zpgx | ROR_zpgx |   | SEI_impl | ADC_absy |          |   |          | ADC_absx | ROR_absx |   |
| 0x80 |          | STA_xind |         |   | STY_zpg  | STA_zpg  | STX_zpg  |   | DEY_impl |          | TXA_impl |   | STY_abs  | STA_abs  | STX_abs  |   |
| 0x90 | BCC_rel  | STA_indy |         |   | STY_zpgx | STA_zpgx | STX_zpgy |   | TYA_impl | STA_absy | TXS_impl |   |          | STA_absx |          |   |
| 0xA0 | LDY_imm  | LDA_xind | LDX_imm |   | LDY_zpg  | LDA_zpg  | LDX_zpg  |   | TAY_impl | LDA_imm  | TAX_impl |   | LDY_abs  | LDA_abs  | LDX_abs  |   |
| 0xB0 | BCS_rel  | LDA_indy |         |   | LDY_zpgx | LDA_zpgx | LDX_zpgy |   | CLV_impl | LDA_absy | TSX_impl |   | LDY_absx | LDA_absx | LDX_absy |   |
| 0xC0 | CPY_imm  | CMP_xind |         |   | CPY_zpg  | CMP_zpg  | DEC_zpg  |   | INY_impl | CMP_imm  | DEX_impl |   | CPY_abs  | CMP_abs  | DEC_abs  |   |
| 0xD0 | BNE_rel  | CMP_indy |         |   |          | CMP_zpgx | DEC_zpgx |   | CLD_impl | CMP_absy |          |   |          | CMP_absx | DEC_absx |   |
| 0xE0 | CPX_imm  | SBC_xind |         |   | CPX_zpg  | SBC_zpg  | INC_zpg  |   | INX_impl | SBC_imm  | NOP_impl |   | CPX_abs  | SBC_abs  | INC_abs  |   |
| 0xF0 | BEQ_rel  | SBC_indy |         |   |          | SBC_zpgx | INC_zpgx |   | SED_impl | SBC_absy |          |   |          | SBC_absx | INC_absx |   |

## Unimplemented Instructions
* 0xfa? (NesGame)
* 0x07? (Mario)
* 0x04? (NesGame)
* 0xe6 *ROR_abs*

