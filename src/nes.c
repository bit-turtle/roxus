#include <roxus/nes.h>
#include "efi/types.h"
#include <roxus/roxus.h>
#include <roxus/image.h>
#include <stdlib.h>
#include <roxus/string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define INES_MAGIC "NES\x1a"

#define VBLANK_CYCLES 2270

efi_status_t print_number(uint16_t number) {
  efi_char_t number_buffer[16];
  return print(itoa(number, number_buffer, 10));
}

efi_status_t print_hex(uint16_t number) {
  efi_char_t number_buffer[16];
  return print(itoa(number, number_buffer, 16));
}

struct ines_header {
  uint8_t magic[4];
  uint8_t prg_rom_size;
  uint8_t chr_rom_size;
  uint8_t flags[10];
};

enum console_type : uint8_t {
  NES = 0,
  VS_UNISYSTEM = 1,
  PLAYCHOICE_10 = 2,
  EXTENDED = 3,
  INVALID_CONSOLE_TYPE
};

enum flags : uint8_t {
  C = 0b00000001,
  Z = 0b00000010,
  I = 0b00000100,
  D = 0b00001000,
  B = 0b00010000,
  UNUSED_FLAG = 0b00100000,
  V = 0b01000000,
  N = 0b10000000,
};

struct nes_state {
  uint32_t cycles;
  // CPU
  uint8_t a, x, y;
  enum flags p;
  uint8_t s;
  uint16_t pc;
  uint8_t ram[0x800];
  uint8_t ppu[8];
  uint8_t apu_io[0x18];
  uint8_t test[8];
  // Mapper
  uint16_t mapper;
  uint8_t submapper;
  uint8_t prg_size;
  uint8_t* prg_rom;
  uint8_t chr_size;
  uint8_t* chr_rom;
};

uint8_t get_byte(struct nes_state* state, uint16_t addr) {
  if (addr < 0x2000)
    return state->ram[addr&0x7ff];
  else if (addr < 0x4000)
    return state->ppu[addr&7];
  else if (addr < 0x4018)
    return state->apu_io[addr-0x4000];
  else if (addr < 0x4020)
    return state->test[addr-0x4018];
  else switch(state->mapper) {
    case 0:
      if (addr < 0x8000)
        break;
      else if (state->prg_size == 1)
        return state->prg_rom[addr&0x3fff];
      else
        return state->prg_rom[addr&0x7fff];
      break;
    default:
      print(u"Error: Read from unsupported mapper\n\r");
  }
  return 0;
}

void set_byte(struct nes_state* state, uint16_t addr, uint8_t value) {
  if (addr < 0x2000)
    state->ram[addr&0x7ff] = value;
  else if (addr < 0x4000)
    state->ppu[addr&7] = value;
  else if (addr < 0x4018)
    state->apu_io[addr-0x4000] = value;
  else if (addr < 0x4020)
    state->test[addr-0x4018] = value;
  else switch(state->mapper) {
    case 0:
      // Can't write to mapper 0
      break;
    default:
      print(u"Error: Write to unsupported mapper\n\r");
  }
}

void set_flag(struct nes_state* state, enum flags flag, bool value) {
  state->p &= ~flag;
  if (value) state->p |= flag;
}

void update_zn(struct nes_state* state, uint8_t value) {
  set_flag(state, Z, value == 0);
  set_flag(state, N, value & 0x80);
}

void update_v(struct nes_state* state, uint8_t original, uint8_t operand) {
  set_flag(state, V, (original&0x80) == (operand&0x80) && (original&0x80) != (state->a&0x80) );
}

void push(struct nes_state* state, uint8_t value) {
  state->ram[0x100+state->s] = value;
  state->s--;
}

uint8_t pop(struct nes_state* state) {
  state->s++;
  return state->ram[0x100+state->s];
}

enum addr : uint16_t {
  NMI_VECTOR = 0xfffa,
  RESET_VECTOR = 0xfffc,
  IRQ_VECTOR = 0xfffe,
};

uint16_t get_addr(struct nes_state* state, enum addr addr) {
  return (get_byte(state, addr+1)<<8 | get_byte(state, addr));
}

/* Instruction addressing postfix
  impl: Implied
  ind: Indirect
  xind: Indirect X-Indexed
  indy: Indirect Y-Indexed
  zpg: Zero Page
  zpgx: Zero Page X-Indexed
  zpgy: Zero Page Y-Indexed
  ind: Indirect
  imm: Immediate
  absx: Absolute X-Indexed
  absy: Absolute Y-Indexed
  acc: Accumulator
  rel: Relative
*/

enum inst : uint8_t {
  // 0
  BRK_impl = 0x00,
  ORA_xind = 0x01,
  ORA_zpg = 0x05,
  ASL_zpg = 0x06,
  PHP_impl = 0x08,
  ORA_imm = 0x09,
  ASL_acc = 0x0a,
  ORA_abs = 0x0d,
  ASL_abs = 0x0e,
  // 1
  BPL_rel = 0x10,
  ORA_indy = 0x11,
  ORA_zpgx = 0x15,
  ASL_zpgx = 0x16,
  CLC_impl = 0x18,
  ORA_absy = 0x19,
  ORA_absx = 0x1d,
  ASL_absx = 0x1e,
  // 2
  JSR_abs = 0x20,
  AND_xind = 0x21,
  BIT_zpg = 0x24,
  AND_zpg = 0x25,
  ROL_zpg = 0x26,
  PLP_impl = 0x28,
  AND_imm = 0x29,
  ROL_acc = 0x2a,
  BIT_abs = 0x2c,
  AND_abs = 0x2d,
  ROL_abs = 0x2e,
  // 3
  BMI_rel = 0x30,
  AND_indy = 0x31,
  AND_zpgx = 0x35,
  ROL_zpgx = 0x36,
  SEC_impl = 0x38,
  AND_absy = 0x39,
  AND_absx = 0x3d,
  ROL_absx = 0x3e,
  // 4
  RTI_impl = 0x40,
  EOR_xind = 0x41,
  EOR_zpg = 0x45,
  LSR_zpg = 0x46,
  PHA_impl = 0x48,
  EOR_imm = 0x49,
  LSR_acc = 0x4a,
  JMP_abs = 0x4c,
  EOR_abs = 0x4d,
  LSR_abs = 0x4e,
  // 5
  BVC_rel = 0x50,
  EOR_indy = 0x51,
  EOR_zpgx = 0x55,
  LSR_zpgx = 0x56,
  CLI_impl = 0x58,
  EOR_absy = 0x59,
  EOR_absx = 0x5d,
  LSR_absx = 0x5e,
  // 6
  RTS_impl = 0x60,
  ADC_xind = 0x61,
  ADC_zpg = 0x65,
  ROR_zpg = 0x66,
  PLA_impl = 0x68,
  ADC_imm = 0x69,
  ROR_acc = 0x6a,
  JMP_ind = 0x6c,
  ADC_abs = 0x6d,
  ROR_abs = 0x6e,
  // 7
  BVS_rel = 0x70,
  ADC_indy = 0x71,
  ADC_zpgx = 0x75,
  ROR_zpgx = 0x76,
  SEI_impl = 0x78,
  ADC_absy = 0x79,
  ADC_absx = 0x7d,
  ROR_absx = 0x7e,
  // 8
  STA_xind = 0x81,
  STY_zpg = 0x84,
  STA_zpg = 0x85,
  STX_zpg = 0x86,
  DEY_impl = 0x88,
  TXA_impl = 0x8a,
  STY_abs = 0x8c,
  STA_abs = 0x8d,
  STX_abs = 0x8e,
  // 9
  BCC_rel = 0x90,
  STA_indy = 0x91,
  STY_zpgx = 0x94,
  STA_zpgx = 0x95,
  STX_zpgy = 0x96,
  TYA_impl = 0x98,
  STA_absy = 0x99,
  TXS_impl = 0x9a,
  STA_absx = 0x9d,
  // a
  LDY_imm = 0xa0,
  LDA_xind = 0xa1,
  LDX_imm = 0xa2,
  LDY_zpg = 0xa4,
  LDA_zpg = 0xa5,
  LDX_zpg = 0xa6,
  TAY_impl = 0xa8,
  LDA_imm = 0xa9,
  TAX_impl = 0xaa,
  LDY_abs = 0xac,
  LDA_abs = 0xad,
  LDX_abs = 0xae,
  // b
  BCS_rel = 0xb0,
  LDA_indy = 0xb1,
  LDY_zpgx = 0xb4,
  LDA_zpgx = 0xb5,
  LDX_zpgy = 0xb6,
  CLV_impl = 0xb8,
  LDA_absy = 0xb9,
  TSX_impl = 0xba,
  LDY_absx = 0xbc,
  LDA_absx = 0xbd,
  LDX_absy = 0xbe,
  // c
  CPY_imm = 0xc0,
  CMP_xind = 0xc1,
  CPY_zpg = 0xc4,
  CMP_zpg = 0xc5,
  DEC_zpg = 0xc6,
  INY_impl = 0xc8,
  CMP_imm = 0xc9,
  DEX_impl = 0xca,
  CPY_abs = 0xcc,
  CMP_abs = 0xcd,
  DEC_abs = 0xce,
  // d
  BNE_rel = 0xd0,
  CMP_indy = 0xd1,
  CMP_zpgx = 0xd5,
  DEC_zpgx = 0xd6,
  CLD_impl = 0xd8,
  CMP_absy = 0xd9,
  CMP_absx = 0xdd,
  DEC_absx = 0xde,
  // e
  CPX_imm = 0xe0,
  SBC_xind = 0xe1,
  CPX_zpg = 0xe4,
  SBC_zpg = 0xe5,
  INC_zpg = 0xe6,
  INX_impl = 0xe8,
  SBC_imm = 0xe9,
  NOP_impl = 0xea,
  CPX_abs = 0xec,
  SBC_abs = 0xed,
  INC_abs = 0xee,
  // f
  BEQ_rel = 0xf0,
  SBC_indy = 0xf1,
  SBC_zpgx = 0xf5,
  INC_zpgx = 0xf6,
  SED_impl = 0xf8,
  SBC_absy = 0xf9,
  SBC_absx = 0xfd,
  INC_absx = 0xfe
};

void cycle(struct nes_state* state, uint32_t cycles) {
  state->cycles += cycles;
}

void push_addr(struct nes_state* state, uint16_t addr) {
  push(state, (addr>>8)&0xf);
  push(state, addr&0xf);
}

void pop_addr(struct nes_state* state) {
  state->pc = pop(state)<<8;
  state->pc |= pop(state);
}

void push_status(struct nes_state* state) {
  set_flag(state, UNUSED_FLAG, true);
  push(state, state->p);
}

void pop_status(struct nes_state* state) {
  state->p = pop(state) & ~B;
}

uint16_t read_vector(struct nes_state* state, uint16_t addr) {
  return get_byte(state, addr+1)<<8 | get_byte(state, addr);
}

void interrupt(struct nes_state* state, uint16_t addr) {
  set_flag(state, B, false);
  push_addr(state, state->pc);
  push_status(state);
  state->pc = read_vector(state, addr);
  cycle(state, 7);
}

void reset(struct nes_state* state) {
  interrupt(state, RESET_VECTOR);
  set_flag(state, I, true);
  state->s = 0xff;
  // PPU initial state
  state->ppu[0] = 0x00;
  state->ppu[1] = 0x00;
  state->ppu[2] = 0b10100000;
  state->ppu[3] = 0x00;
  state->ppu[4] = 0x00;
  state->ppu[5] = 0x00;
  state->ppu[6] = 0x00;
  state->ppu[7] = 0x00;
}

void nmi(struct nes_state* state) {
  interrupt(state, NMI_VECTOR);
}

void irq(struct nes_state* state) {
  if ( !(state->p&I) )
    interrupt(state, IRQ_VECTOR);
}

uint16_t addr_abs(struct nes_state* state) {
  state->pc+=2;
  return (get_byte(state, state->pc-1)<<8) | get_byte(state, state->pc-2);
}

uint16_t addr_absx(struct nes_state* state) {
  uint16_t addr = addr_abs(state);
  uint16_t offset = addr+state->x;
  // Page boundary cycle
  if ((addr&0xff00) != (offset&0xff00))
    cycle(state, 1);
  return offset;
}

uint16_t addr_absy(struct nes_state* state) {
  uint16_t addr = addr_abs(state);
  uint16_t offset = addr+state->y;
  // Page boundary cycle
  if ((addr&0xff00) != (offset&0xff00))
    cycle(state, 1);
  return offset;
}

uint16_t addr_ind(struct nes_state* state) {
  state->pc+=2;
  uint16_t addr = (get_byte(state, state->pc-1)<<8) | get_byte(state, state->pc-2);
  // With NMOS 0xff bug simulation
  return (get_byte(state, ((addr&0xff) != 0xff) ? addr+1 : addr&0xff00)<<8) | get_byte(state, addr);
}

uint16_t addr_zpg(struct nes_state* state) {
  state->pc++;
  return get_byte(state, state->pc-1);
}

uint16_t addr_zpgx(struct nes_state* state) {
  uint16_t addr = addr_zpg(state);
  uint16_t offset = addr+state->x;
  // Page boundary cycle
  if ((addr&0xff00) != (offset&0xff00))
    cycle(state, 1);
  return offset;
}

uint16_t addr_zpgy(struct nes_state* state) {
  uint16_t addr = addr_zpg(state);
  uint16_t offset = addr+state->y;
  // Page boundary cycle
  if ((addr&0xff00) != (offset&0xff00))
    cycle(state, 1);
  return offset;
}

uint16_t addr_indy(struct nes_state* state) {
  uint16_t addr = addr_ind(state);
  uint16_t offset = addr+state->y;
  return offset & 0xff;
}

uint16_t addr_xind(struct nes_state* state) {
  state->pc++;
  uint8_t zpg = get_byte(state, state->pc-1)+state->x;
  uint16_t addr = (get_byte(state, zpg+1)<<8) | get_byte(state, zpg);
  // With NMOS 0xff bug simulation
  return (get_byte(state, (addr+1)&0xff)<<8) | get_byte(state, addr);
}

uint16_t addr_rel(struct nes_state* state) {
  state->pc++;
  int8_t offset = (int8_t) get_byte(state, state->pc);
  uint16_t addr = state->pc+offset;
  // Page boundary cycle
  if ((addr&0xff00) != (state->pc&0xff00))
    cycle(state, 1);
  return addr;
}

uint8_t read_imm(struct nes_state* state) {
  state->pc++;
  return get_byte(state, state->pc-1);
}

void cpu_step(struct nes_state* state) {
  uint8_t inst, temp, buffer;
  inst = get_byte(state, state->pc);
  state->pc++;
  switch(inst) {
    // Nop
    case NOP_impl:
      cycle(state, 2);
      break;
    // Jumps
    case JSR_abs:
      push_addr(state, state->pc+2);
      state->pc = addr_abs(state);
      cycle(state, 6);
      break;
    case RTS_impl:
      pop_addr(state);
      cycle(state, 6);
      break;
    case JMP_abs:
      state->pc = addr_abs(state);
      cycle(state, 3);
      break;
    case JMP_ind:
      state->pc = addr_ind(state);
      cycle(state, 5);
      break;
    case RTI_impl:
      pop_status(state);
      pop_addr(state);
      cycle(state, 6);
      break;
    case BRK_impl:
      set_flag(state, B, true);
      interrupt(state, RESET_VECTOR);
      break;
    case BCC_rel:
      if ( !(state->p&C) ) {
        state->pc = addr_rel(state);
        cycle(state, 1);
      }
      else
        state->pc++;
      cycle(state, 2);
      break;
    case BCS_rel:
      if (state->p&C) {
        state->pc = addr_rel(state);
        cycle(state, 1);
      }
      else
        state->pc++;
      cycle(state, 2);
      break;
    case BEQ_rel:
      if (state->p&Z) {
        state->pc = addr_rel(state);
        cycle(state, 1);
      }
      else
        state->pc++;
      cycle(state, 2);
      break;
    case BMI_rel:
      if (state->p&N) {
        state->pc = addr_rel(state);
        cycle(state, 1);
      }
      else
        state->pc++;
      cycle(state, 2);
      break;
    case BNE_rel:
      if ( !(state->p&Z) ) {
        state->pc = addr_rel(state);
        cycle(state, 1);
      }
      else
        state->pc++;
      cycle(state, 2);
      break;
    case BPL_rel:
      if ( !(state->p&N) ) {
        state->pc = addr_rel(state);
        cycle(state, 1);
      }
      else
        state->pc++;
      cycle(state, 2);
      break;
    case BVC_rel:
      if ( !(state->p&V) ) {
        state->pc = addr_rel(state);
        cycle(state, 1);
      }
      else
        state->pc++;
      cycle(state, 2);
      break;
    case BVS_rel:
      if (state->p&V) {
        state->pc = addr_rel(state);
        cycle(state, 1);
      }
      else
        state->pc++;
      cycle(state, 2);
      break;
    // Flags
    case CLD_impl:
      set_flag(state, D, false);
      cycle(state, 2);
      break;
    case CLI_impl:
      set_flag(state, I, false);
      cycle(state, 2);
      break;
    case CLV_impl:
      set_flag(state, V, false);
      cycle(state, 2);
      break;
    case CLC_impl:
      set_flag(state, C, false);
      cycle(state, 2);
      break;
    case SED_impl:
      set_flag(state, D, true);
      cycle(state, 2);
      break;
    case SEI_impl:
      set_flag(state, I, true);
      cycle(state, 2);
      break;
    case SEC_impl:
      set_flag(state, C, true);
      cycle(state, 2);
      break;
    // STA
    case STA_abs:
      set_byte(state, addr_abs(state), state->a);
      cycle(state, 4);
      break;
    case STA_absx:
      set_byte(state, addr_absx(state), state->a);
      cycle(state, 5);
      break;
    case STA_absy:
      set_byte(state, addr_absy(state), state->a);
      cycle(state, 5);
      break;
    case STA_zpg:
      set_byte(state, addr_zpg(state), state->a);
      cycle(state, 3);
      break;
    case STA_zpgx:
      set_byte(state, addr_zpgx(state), state->a);
      cycle(state, 4);
      break;
    case STA_indy:
      set_byte(state, addr_indy(state), state->a);
      cycle(state, 6);
      break;
    case STA_xind:
      set_byte(state, addr_xind(state), state->a);
      cycle(state, 6);
      break;
    // STX
    case STX_zpg:
      set_byte(state, addr_zpg(state), state->x);
      cycle(state, 3);
      break;
    case STX_zpgy:
      set_byte(state, addr_zpgy(state), state->x);
      cycle(state, 4);
      break;
    case STX_abs:
      set_byte(state, addr_abs(state), state->x);
      cycle(state, 4);
      break;
    // STY
    case STY_zpg:
      set_byte(state, addr_zpg(state), state->y);
      cycle(state, 3);
      break;
    case STY_zpgx:
      set_byte(state, addr_zpgx(state), state->y);
      cycle(state, 4);
      break;
    case STY_abs:
      set_byte(state, addr_abs(state), state->y);
      cycle(state, 4);
      break;
    // LDA
    case LDA_imm:
      state->a = read_imm(state);
      cycle(state, 2);
      break;
    case LDA_zpg:
      state->a = get_byte(state, addr_zpg(state));
      cycle(state, 3);
      break;
    case LDA_zpgx:
      state->a = get_byte(state, addr_zpgx(state));
      cycle(state, 4);
      break;
    case LDA_abs:
      state->a = get_byte(state, addr_abs(state));
      cycle(state, 4);
      break;
    case LDA_absx:
      state->a = get_byte(state, addr_absx(state));
      cycle(state, 4);
      break;
    case LDA_absy:
      state->a = get_byte(state, addr_absy(state));
      cycle(state, 4);
      break;
    case LDA_xind:
      state->a = get_byte(state, addr_xind(state));
      cycle(state, 6);
      break;
    case LDA_indy:
      state->a = get_byte(state, addr_indy(state));
      cycle(state, 5);
      break;
    // LDX
    case LDX_imm:
      state->x = read_imm(state);
      cycle(state, 2);
      break;
    case LDX_zpg:
      state->x = get_byte(state, addr_zpg(state));
      cycle(state, 3);
      break;
    case LDX_zpgy:
      state->x = get_byte(state, addr_zpgy(state));
      cycle(state, 4);
      break;
    case LDX_abs:
      state->x = get_byte(state, addr_abs(state));
      cycle(state, 4);
      break;
    case LDX_absy:
      state->x = get_byte(state, addr_absy(state));
      cycle(state, 4);
      break;
    // LDY
    case LDY_imm:
      state->y = read_imm(state);
      update_zn(state, state->y);
      cycle(state, 2);
      break;
    case LDY_zpg:
      state->y = get_byte(state, addr_zpg(state));
      update_zn(state, state->y);
      cycle(state, 3);
      break;
    case LDY_zpgx:
      state->y = get_byte(state, addr_zpgx(state));
      update_zn(state, state->y);
      cycle(state, 4);
      break;
    case LDY_abs:
      state->y = get_byte(state, addr_abs(state));
      update_zn(state, state->y);
      cycle(state, 4);
      break;
    case LDY_absx:
      state->y = get_byte(state, addr_absx(state));
      update_zn(state, state->y);
      cycle(state, 4);
      break;
    // Stack
    case PHP_impl:
      set_flag(state, B, true);
      push_status(state);
      cycle(state, 3);
      break;
    case PLP_impl:
      pop_status(state);
      cycle(state, 4);
      break;
    case PHA_impl:
      push(state, state->a);
      cycle(state, 3);
      break;
    case PLA_impl:
      state->a = pop(state);
      update_zn(state, state->a);
      cycle(state, 4);
      break;
    // Transfers
    case TAX_impl:
      state->x = state->a;
      update_zn(state, state->x);
      cycle(state, 2);
      break;
    case TAY_impl:
      state->y = state->a;
      update_zn(state, state->y);
      cycle(state, 2);
      break;
    case TSX_impl:
      state->x = state->s;
      update_zn(state, state->x);
      cycle(state, 2);
      break;
    case TXA_impl:
      state->a = state->x;
      update_zn(state, state->a);
      cycle(state, 2);
      break;
    case TXS_impl:
      state->s = state->x;
      cycle(state, 2);
      break;
    case TYA_impl:
      state->a = state->y;
      update_zn(state, state->a);
      cycle(state, 2);
      break;
    // Arithmetic
    case INX_impl:
      state->x++;
      update_zn(state, state->x);
      cycle(state, 2);
      break;
    case INY_impl:
      state->y++;
      update_zn(state, state->y);
      cycle(state, 2);
      break;
    case DEX_impl:
      state->x--;
      update_zn(state, state->x);
      cycle(state, 2);
      break;
    case DEY_impl:
      state->y--;
      update_zn(state, state->y);
      cycle(state, 2);
      break;
    // ORA
    case ORA_imm:
      state->a |= read_imm(state);
      update_zn(state, state->a);
      cycle(state, 2);
      break;
    case ORA_zpg:
      state->a |= get_byte(state, addr_zpg(state));
      update_zn(state, state->a);
      cycle(state, 3);
      break;
    case ORA_zpgx:
      state->a |= get_byte(state, addr_zpgx(state));
      update_zn(state, state->a);
      cycle(state, 4);
      break;
    case ORA_abs:
      state->a |= get_byte(state, addr_abs(state));
      update_zn(state, state->a);
      cycle(state, 4);
      break;
    case ORA_absx:
      state->a |= get_byte(state, addr_absx(state));
      update_zn(state, state->a);
      cycle(state, 4);
      break;
    case ORA_absy:
      state->a |= get_byte(state, addr_absy(state));
      update_zn(state, state->a);
      cycle(state, 4);
      break;
    case ORA_xind:
      state->a |= get_byte(state, addr_xind(state));
      update_zn(state, state->a);
      cycle(state, 6);
      break;
    case ORA_indy:
      state->a |= get_byte(state, addr_indy(state));
      update_zn(state, state->a);
      cycle(state, 5);
      break;
    // AND
    case AND_imm:
      state->a &= read_imm(state);
      update_zn(state, state->a);
      cycle(state, 2);
      break;
    case AND_zpg:
      state->a &= get_byte(state, addr_zpg(state));
      update_zn(state, state->a);
      cycle(state, 3);
      break;
    case AND_zpgx:
      state->a &= get_byte(state, addr_zpgx(state));
      update_zn(state, state->a);
      cycle(state, 4);
      break;
    case AND_abs:
      state->a &= get_byte(state, addr_abs(state));
      update_zn(state, state->a);
      cycle(state, 4);
      break;
    case AND_absx:
      state->a &= get_byte(state, addr_absx(state));
      update_zn(state, state->a);
      cycle(state, 4);
      break;
    case AND_absy:
      state->a &= get_byte(state, addr_absy(state));
      update_zn(state, state->a);
      cycle(state, 4);
      break;
    case AND_xind:
      state->a &= get_byte(state, addr_xind(state));
      update_zn(state, state->a);
      cycle(state, 6);
      break;
    case AND_indy:
      state->a &= get_byte(state, addr_indy(state));
      update_zn(state, state->a);
      cycle(state, 5);
      break;
    // EOR
    case EOR_imm:
      state->a ^= read_imm(state);
      update_zn(state, state->a);
      cycle(state, 2);
      break;
    case EOR_zpg:
      state->a ^= get_byte(state, addr_zpg(state));
      update_zn(state, state->a);
      cycle(state, 3);
      break;
    case EOR_zpgx:
      state->a ^= get_byte(state, addr_zpgx(state));
      update_zn(state, state->a);
      cycle(state, 4);
      break;
    case EOR_abs:
      state->a ^= get_byte(state, addr_abs(state));
      update_zn(state, state->a);
      cycle(state, 4);
      break;
    case EOR_absx:
      state->a ^= get_byte(state, addr_absx(state));
      update_zn(state, state->a);
      cycle(state, 4);
      break;
    case EOR_absy:
      state->a ^= get_byte(state, addr_absy(state));
      update_zn(state, state->a);
      cycle(state, 4);
      break;
    case EOR_xind:
      state->a ^= get_byte(state, addr_xind(state));
      update_zn(state, state->a);
      cycle(state, 6);
      break;
    case EOR_indy:
      state->a ^= get_byte(state, addr_indy(state));
      update_zn(state, state->a);
      cycle(state, 5);
      break;
    // ADC
    case ADC_imm:
      temp = read_imm(state);
      update_v(state, state->a, ~temp); 
      temp = state->a;
      state->a += temp;
      update_zn(state, state->a);
      if (temp > state->a)
        set_flag(state, C, true);
      else
        set_flag(state, C, false);
      cycle(state, 2);
      break;
    case ADC_zpg:
      temp = get_byte(state, addr_zpg(state));
      update_v(state, state->a, ~temp); 
      temp = state->a;
      state->a += temp;
      update_zn(state, state->a);
      if (temp > state->a)
        set_flag(state, C, true);
      else
        set_flag(state, C, false);
      cycle(state, 3);
      break;
    case ADC_zpgx:
      temp = get_byte(state, addr_zpgx(state));
      update_v(state, state->a, ~temp); 
      temp = state->a;
      state->a += temp;
      update_zn(state, state->a);
      if (temp > state->a)
        set_flag(state, C, true);
      else
        set_flag(state, C, false);
      cycle(state, 4);
      break;
    case ADC_abs:
      temp = get_byte(state, addr_abs(state));
      update_v(state, state->a, ~temp); 
      temp = state->a;
      state->a += temp;
      update_zn(state, state->a);
      if (temp > state->a)
        set_flag(state, C, true);
      else
        set_flag(state, C, false);
      cycle(state, 4);
      break;
    case ADC_absx:
      temp = get_byte(state, addr_absx(state));
      update_v(state, state->a, ~temp); 
      temp = state->a;
      state->a += temp;
      update_zn(state, state->a);
      if (temp > state->a)
        set_flag(state, C, true);
      else
        set_flag(state, C, false);
      cycle(state, 4);
      break;
    case ADC_absy:
      temp = get_byte(state, addr_absy(state));
      update_v(state, state->a, ~temp); 
      temp = state->a;
      state->a += temp;
      update_zn(state, state->a);
      if (temp > state->a)
        set_flag(state, C, true);
      else
        set_flag(state, C, false);
      cycle(state, 4);
      break;
    case ADC_xind:
      temp = get_byte(state, addr_xind(state));
      update_v(state, state->a, ~temp); 
      temp = state->a;
      state->a += temp;
      update_zn(state, state->a);
      if (temp > state->a)
        set_flag(state, C, true);
      else
        set_flag(state, C, false);
      cycle(state, 6);
      break;
    case ADC_indy:
      temp = get_byte(state, addr_indy(state));
      update_v(state, state->a, ~temp); 
      temp = state->a;
      state->a += temp;
      update_zn(state, state->a);
      if (temp > state->a)
        set_flag(state, C, true);
      else
        set_flag(state, C, false);
      cycle(state, 5);
      break;
    // SBC
    case SBC_imm:
      temp =  read_imm(state);
      update_v(state, state->a, temp);
      temp = state->a;
      state->a -= temp;
      update_zn(state, state->a);
      if (state->a > temp)
        set_flag(state, C, true);
      else
        set_flag(state, C, false);
      cycle(state, 2);
      break;
    case SBC_zpg:
      temp = get_byte(state, addr_zpg(state));
      update_v(state, state->a, temp);
      temp = state->a;
      state->a -= temp;
      update_zn(state, state->a);
      if (state->a > temp)
        set_flag(state, C, true);
      else
        set_flag(state, C, false);
      cycle(state, 3);
      break;
    case SBC_zpgx:
      temp = get_byte(state, addr_zpgx(state));
      update_v(state, state->a, temp);
      temp = state->a;
      state->a -= temp;
      update_zn(state, state->a);
      if (state->a > temp)
        set_flag(state, C, true);
      else
        set_flag(state, C, false);
      cycle(state, 4);
      break;
    case SBC_abs:
      temp = get_byte(state, addr_abs(state));
      update_v(state, state->a, temp);
      temp = state->a;
      state->a -= temp;
      update_zn(state, state->a);
      if (state->a > temp)
        set_flag(state, C, true);
      else
        set_flag(state, C, false);
      cycle(state, 4);
      break;
    case SBC_absx:
      temp = get_byte(state, addr_absx(state));
      update_v(state, state->a, temp);
      temp = state->a;
      state->a -= temp;
      update_zn(state, state->a);
      if (state->a > temp)
        set_flag(state, C, true);
      else
        set_flag(state, C, false);
      cycle(state, 4);
      break;
    case SBC_absy:
      temp = get_byte(state, addr_absy(state));
      update_v(state, state->a, temp);
      temp = state->a;
      state->a -= temp;
      update_zn(state, state->a);
      if (state->a > temp)
        set_flag(state, C, true);
      else
        set_flag(state, C, false);
      cycle(state, 4);
      break;
    case SBC_xind:
      temp = get_byte(state, addr_xind(state));
      update_v(state, state->a, temp);
      temp = state->a;
      state->a -= temp;
      update_zn(state, state->a);
      if (state->a > temp)
        set_flag(state, C, true);
      else
        set_flag(state, C, false);
      cycle(state, 6);
      break;
    case SBC_indy:
      temp = get_byte(state, addr_indy(state));
      update_v(state, state->a, temp);
      temp = state->a;
      state->a -= temp;
      update_zn(state, state->a);
      if (state->a > temp)
        set_flag(state, C, true);
      else
        set_flag(state, C, false);
      cycle(state, 5);
      break;
    // ASL
    case ASL_acc:
      state->a <<= 1;
      cycle(state, 2);
      break;
    case ASL_zpg:
      temp = addr_zpg(state);
      buffer = get_byte(state, temp);
      buffer <<= 1;
      set_byte(state, temp, buffer);
      cycle(state, 5);
      break;
    case ASL_zpgx:
      temp = addr_zpgx(state);
      buffer = get_byte(state, temp);
      buffer <<= 1;
      set_byte(state, temp, buffer);
      cycle(state, 6);
      break;
    case ASL_abs:
      temp = addr_abs(state);
      buffer = get_byte(state, temp);
      buffer <<= 1;
      set_byte(state, temp, buffer);
      cycle(state, 6);
      break;
    case ASL_absx:
      temp = addr_absx(state);
      buffer = get_byte(state, temp);
      buffer <<= 1;
      set_byte(state, temp, buffer);
      cycle(state, 7);
      break;
    // BIT
    case BIT_zpg:
      buffer = state->a;
      buffer &= get_byte(state, addr_zpg(state));
      set_flag(state, N, buffer&0b10000000);
      set_flag(state, V, buffer&0b01000000);
      set_flag(state, Z, buffer == 0);
      cycle(state, 3);
      break;
    case BIT_abs:
      buffer = state->a;
      buffer &= get_byte(state, addr_abs(state));
      set_flag(state, N, buffer&0b10000000);
      set_flag(state, V, buffer&0b01000000);
      set_flag(state, Z, buffer == 0);
      cycle(state, 4);
      break;
    // Unimplemented
    default:
      print(u"0x");
      print_hex(state->pc);
      print(u" Warning: Unimplemented Instruction $");
      print_hex(inst);
      print(u"\n\r");
  }
}


uint8_t get_chr_color(uint8_t* chr_rom, unsigned index, uint8_t x, uint8_t y) {
  uint8_t upper = chr_rom[index*16+y]&(0x80 >> x);
  upper = upper >> (7-x) << 1;
  uint8_t lower = chr_rom[index*16+y+8]&(0x80 >> x);
  lower = lower >> (7-x);
  return upper | lower;
}

void render_chr_tile(uint8_t* chr_rom, unsigned index, struct efi_graphics_output_blt_pixel* image, struct efi_graphics_output_blt_pixel color1, struct efi_graphics_output_blt_pixel color2, struct efi_graphics_output_blt_pixel color3) {
  for (uint8_t x = 0; x < 8; x++) for (uint8_t y = 0; y < 8; y++) {
    switch(get_chr_color(chr_rom, index, x, y)) {
      case 0:
        // Transparent (No color change)
        break;
      case 1:
        image[x%8+y*8] = color1;
        break;
      case 2:
        image[x%8+y*8] = color2;
        break;
      case 3:
        image[x%8+y*8] = color3;
        break;
    }
  }
}

efi_status_t run_nes_rom(struct efi_file_protocol* rom) {
  efi_status_t status;
  // Get File Info
  struct efi_file_info file_info;
  if (rom == NULL) {
    print(u"File was null!");
    return EFI_INVALID_PARAMETER;
  }
  else {
    struct efi_guid file_info_guid = EFI_FILE_INFO_ID;
    efi_uint_t file_info_size = sizeof(file_info);
    status = rom->getInfo(rom, &file_info_guid, &file_info_size, &file_info);
    if (status != EFI_SUCCESS) {
      print(u"Failed to get file info!");
      return EFI_INVALID_PARAMETER;
    }
  }

  // Read Header
  struct ines_header header;
  efi_uint_t header_size = sizeof(header);
  status = rom->read(rom, &header_size, &header);
  if (status != EFI_SUCCESS || header_size != sizeof(header)) {
    print(u"Failed to read iNES Header!");
    return EFI_LOAD_ERROR;
  }
  // Verify magic
  for (int i = 0; i < 4; i++) if (header.magic[i] != INES_MAGIC[i]) {
    print(u"iNES Header magic value incorrect!");
    return EFI_LOAD_ERROR;
  }
  uint16_t prg_rom_size = header.prg_rom_size;
  uint16_t chr_rom_size = header.chr_rom_size;
  // Detect NES 2.0
  bool nes2 = ((header.flags[1]&0xc)==0x8) ? true : false;
  if (nes2) {
    prg_rom_size |= (header.flags[2]&0xf)<<8;
    chr_rom_size |= (header.flags[2]&0xf0)<<4;
    if (0x4000*prg_rom_size+0x2000*chr_rom_size > file_info.fileSize) {
      nes2 = false;
      prg_rom_size &= 0xff;
      chr_rom_size &= 0xff;
    }
  }
  // Detect Archaic iNES
  bool archaicInes = ((header.flags[0]&0xc)==0x4) ? true : false;
  // Detect iNES
  bool ines = ((header.flags[0]&0xc)==0x0) ? true : false;
  if (ines) {
    for (int i = 0; i < 10; i++)
      if (header.flags[i] != 0)
        ines = false;
  }
  // Detect iNES 0.7
  bool ines07 = (!(nes2 || archaicInes || ines)) ? true : false;
  // Display iNES Format
  if (nes2)
    print(u"Format: NES 2.0\n\r");
  else if (archaicInes)
    print(u"Format: Archaic iNES\n\r");
  else if (ines)
    print(u"Format: iNES\n\r");
  else if (ines07)
    print(u"Format: iNES 0.7\n\r");
  else
    print(u"Format: Unknown");
  // Read Trainer
  bool trainer_present = ((header.flags[0]&(1<<2))!=0) ? true : false;
  uint8_t trainer[512];
  if (trainer_present) {
    print(u"Trainer present\n\r");
    efi_uint_t trainer_size = sizeof(trainer);
    status = rom->read(rom, &trainer_size, trainer);
    if (status != EFI_SUCCESS || trainer_size != sizeof(trainer)) {
      print(u"Failed to Read Trainer!");
      return EFI_LOAD_ERROR;
    }
  }
  // Read PRG Rom
  uint8_t* prg_rom = malloc(0x4000*prg_rom_size);
  if (prg_rom == NULL) {
    print(u"Failed to allocate memory for PRG Rom!");
    return EFI_LOAD_ERROR;
  }
  efi_uint_t prg_rom_file_size = 0x4000*prg_rom_size;
  status = rom->read(rom, &prg_rom_file_size, prg_rom);
  if (status != EFI_SUCCESS || prg_rom_file_size != 0x4000*prg_rom_size) {
    print(u"Failed to read PRG Rom!");
    free(prg_rom);
    return EFI_LOAD_ERROR;
  }
  // Read CHR Rom
  uint8_t* chr_rom = malloc(0x2000*chr_rom_size);
  if (chr_rom == NULL) {
    print(u"Failed to allocate memory for PRG Rom!");
    free(prg_rom);
    return EFI_LOAD_ERROR;
  }
  efi_uint_t chr_rom_file_size = 0x2000*chr_rom_size;
  status = rom->read(rom, &chr_rom_file_size, chr_rom);
  if (status != EFI_SUCCESS || chr_rom_file_size != 0x2000*chr_rom_size) {
    print(u"Failed to read CHR Rom!");
    free(prg_rom);
    free(chr_rom);
    return EFI_LOAD_ERROR;
  }
  // Determine Console Type
  enum console_type console_type = (archaicInes || ines07) ? NES : (header.flags[1]&0b11);
  print(u"Console Type: ");
  if (console_type == NES)
    print(u"NES\r\n");
  else if (console_type == VS_UNISYSTEM)
    print(u"VS Unisystem\r\n");
  else if (console_type == PLAYCHOICE_10)
    print(u"PlayChoice-10\r\n");
  else
    print(u"Extended\r\n");
  // Read PlayChoice-10 Data
  uint8_t* playchoice_10 = NULL;
  if (console_type == PLAYCHOICE_10) {
    playchoice_10 = malloc(0x2000+16+16);
    if (playchoice_10 == NULL) {
      print(u"Failed to allocate memory for PlayChoice-10 Data!");
      free(prg_rom);
      free(chr_rom);
      return EFI_LOAD_ERROR;
    }
    efi_uint_t playchoice_10_size = 0x2000+16+16;
    status = rom->read(rom, &playchoice_10_size, &playchoice_10);
    if (status != EFI_SUCCESS || playchoice_10_size != 0x2000+16+16) {
      print(u"Failed to load PlayChoice-10 Data!");
      free(prg_rom);
      free(chr_rom);
      free(playchoice_10);
      return EFI_LOAD_ERROR;
    }
  }
  // Read Title (If Present)
  char title[128] = "\0";
  efi_uint_t title_size = 128;
  status = rom->read(rom, &title_size, &title);
  if (status != EFI_SUCCESS) {
    free(prg_rom);
    free(chr_rom);
    if (console_type == PLAYCHOICE_10)
      free(playchoice_10);
    return EFI_LOAD_ERROR;
  }
  if (title_size > 0) {
    print(u"Title: ");
    print_ascii(&(title[0]));
  }

  // Read Mapper Value
  uint16_t mapper = 0;
  uint8_t submapper = 0;
  mapper |= (header.flags[0]&0xf0)>>4;
  if (nes2 || ines || ines07) mapper |= (header.flags[0]&0xf0)>>4;
  if (nes2) mapper |= (header.flags[1]&0xf)<<8;
  if (nes2) submapper = (header.flags[1]&0xf0)>>4;
  // Print mapper id
  print(u"Mapper: ");
  print_number(mapper);
  print(u"\n\rSubmapper: ");
  print_number(submapper);
  print(u"\n\r");

  // Unsupported Mapper
  if (mapper != 0) {
    free(prg_rom);
    free(chr_rom);
    if (console_type == PLAYCHOICE_10)
      free(playchoice_10);
    print(u"Unsupported Mapper\n\r");
    return EFI_UNSUPPORTED;
  }

  // Allocate Mapper Memory

  // Setup NES
  struct nes_state nes;
  nes.prg_rom = prg_rom;
  nes.chr_rom = chr_rom;
  nes.prg_size = prg_rom_size;
  nes.chr_size = chr_rom_size;
  nes.mapper = mapper;

  reset(&nes);

  for(unsigned i = 0; i < 3; i++) {
    while(nes.cycles < VBLANK_CYCLES) {
      cpu_step(&nes);
      /*
      print_number(nes.pc);
      print(u", ");
      print_number(nes.cycles);
      print(u", a:");
      print_number(nes.a);
      print(u", x:");
      print_number(nes.x);
      print(u", y:");
      print_number(nes.y);
      print(u", s:");
      print_number(nes.s);
      print(u"\n\r");
      */
    }
    nes.cycles = 0;
    print(u"----- VBLANK ------\n\r");
    nmi(&nes);
  }
  
  // Cleanup
  free(prg_rom);
  free(chr_rom);
  if (console_type == PLAYCHOICE_10)
    free(playchoice_10);
  return EFI_SUCCESS;
}
