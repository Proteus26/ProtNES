pub struct CPU {
    pub register_a: u8,
    pub register_x: u8,
    pub register_y: u8,
    pub status: u8,
    pub program_counter: u16,
}

impl CPU {
    pub fn new() -> Self {
        CPU {
            register_a: 0,
            register_x: 0,
            register_y: 0,
            status: 0,
            program_counter: 0,
        }
    }

    fn lda(&mut self, value: u8) {
        self.register_a = value;
        self.update_zero_and_negative_flags(self.register_a);
    }

    fn ldx(&mut self, value: u8) {
        self.register_x = value;
        self.update_zero_and_negative_flags(self.register_x);
    }

    fn ldy(&mut self, value: u8) {
        self.register_y = value;
        self.update_zero_and_negative_flags(self.register_y);
    }

    fn tax(&mut self) {
        self.register_x = self.register_a;
        self.update_zero_and_negative_flags(self.register_x);
    }

    fn tay(&mut self) {
        self.register_y = self.register_a;
        self.update_zero_and_negative_flags(self.register_y);
    }

    fn txa(&mut self) {
        self.register_a = self.register_x;
        self.update_zero_and_negative_flags(self.register_a);
    }

    fn tya(&mut self) {
        self.register_a = self.register_y;
        self.update_zero_and_negative_flags(self.register_a);
    }

    fn tsx(&mut self) {
        self.register_x = self.status;
        self.update_zero_and_negative_flags(self.register_x);
    }

    fn txs(&mut self) {
        self.status = self.register_x;
    }

    fn pha(&mut self) {}

    fn pla(&mut self) {
        self.update_zero_and_negative_flags(self.register_a);
    }

    fn php(&mut self) {}

    fn plp(&mut self) {}

    fn adc(&mut self, value: u8) {
        let sum = self.register_a as u16 + value as u16 + (self.status & 1) as u16;
        let carry = sum > 0xFF;
        if carry {
            self.status |= 0b0000_0001;
        } else {
            self.status &= 0b1111_1110;
        }
        let result = sum as u8;
        if (self.register_a ^ result) & (value ^ result) & 0x80 != 0 {
            self.status |= 0b0100_0000;
        } else {
            self.status &= 0b1011_1111;
        }
        self.register_a = result;
        self.update_zero_and_negative_flags(self.register_a);
    }

    fn sbc(&mut self, value: u8) {
        self.adc(value.wrapping_neg().wrapping_sub(1));
    }

    fn inx(&mut self) {
        self.register_x = self.register_x.wrapping_add(1);
        self.update_zero_and_negative_flags(self.register_x);
    }

    fn iny(&mut self) {
        self.register_y = self.register_y.wrapping_add(1);
        self.update_zero_and_negative_flags(self.register_y);
    }

    fn dex(&mut self) {
        self.register_x = self.register_x.wrapping_sub(1);
        self.update_zero_and_negative_flags(self.register_x);
    }

    fn dey(&mut self) {
        self.register_y = self.register_y.wrapping_sub(1);
        self.update_zero_and_negative_flags(self.register_y);
    }

    fn asl_accumulator(&mut self) {
        let old_a = self.register_a;
        self.register_a <<= 1;
        self.update_zero_and_negative_flags(self.register_a);
        if old_a & 0b1000_0000 != 0 {
            self.status |= 0b0000_0001;
        } else {
            self.status &= 0b1111_1110;
        }
    }


    fn lsr_accumulator(&mut self) {
        let old_a = self.register_a;
        self.register_a >>= 1;
        self.update_zero_and_negative_flags(self.register_a);
        if old_a & 0b0000_0001 != 0 {
            self.status |= 0b0000_0001;
        } else {
            self.status &= 0b1111_1110;
        }
    }


    fn rol_accumulator(&mut self) {
        let old_carry = self.status & 1;
        let old_a = self.register_a;
        self.register_a <<= 1;
        self.register_a |= old_carry;
        self.update_zero_and_negative_flags(self.register_a);
        if old_a & 0b1000_0000 != 0 {
            self.status |= 0b0000_0001;
        } else {
            self.status &= 0b1111_1110;
        }
    }


    fn ror_accumulator(&mut self) {
        let old_carry = self.status & 1;
        let old_a = self.register_a;
        self.register_a >>= 1;
        self.register_a |= old_carry << 7;
        self.update_zero_and_negative_flags(self.register_a);
        if old_a & 0b0000_0001 != 0 {
            self.status |= 0b0000_0001;
        } else {
            self.status &= 0b1111_1110;
        }
    }

    fn clc(&mut self) {
        self.status &= 0b1111_1110;
    }

    fn cld(&mut self) {
        self.status &= 0b1111_0111;
    }

    fn cli(&mut self) {
        self.status &= 0b1111_1011;
    }

    fn clv(&mut self) {
        self.status &= 0b1011_1111;
    }

    fn sec(&mut self) {
        self.status |= 0b0000_0001;
    }

    fn sed(&mut self) {
        self.status |= 0b0000_1000;
    }

    fn sei(&mut self) {
        self.status |= 0b0000_0100;
    }

    fn and(&mut self, value: u8) {
        self.register_a &= value;
        self.update_zero_and_negative_flags(self.register_a);
    }

    fn eor(&mut self, value: u8) {
        self.register_a ^= value;
        self.update_zero_and_negative_flags(self.register_a);
    }

    fn ora(&mut self, value: u8) {
        self.register_a |= value;
        self.update_zero_and_negative_flags(self.register_a);
    }

    fn cpy(&mut self, value: u8) {
        let result = self.register_y.wrapping_sub(value);
        self.update_zero_and_negative_flags(result);

        if self.register_y >= value {
            self.status |= 0b0000_0001;
        } else {
            self.status &= 0b1111_1110;
        }
    }

    fn cpx(&mut self, value: u8) {
        let result = self.register_x.wrapping_sub(value);
        self.update_zero_and_negative_flags(result);

        if self.register_x >= value {
            self.status |= 0b0000_0001;
        } else {
            self.status &= 0b1111_1110;
        }
    }

    fn cmp(&mut self, value: u8) {
        let result = self.register_a.wrapping_sub(value);
        self.update_zero_and_negative_flags(result);

        if self.register_a >= value {
            self.status |= 0b0000_0001;
        } else {
            self.status &= 0b1111_1110;
        }
    }

    fn bit(&mut self, value: u8) {
        let result = self.register_a & value;
        if result == 0 {
            self.status |= 0b0000_0010;
        } else {
            self.status &= 0b1111_1101;
        }
        self.status = (self.status & 0b0011_1111) | (value & 0b1100_0000);
    }


    fn update_zero_and_negative_flags(&mut self, result: u8) {
        if result == 0 {
            self.status |= 0b0000_0010;
        } else {
            self.status &= 0b1111_1101;
        }

        if result & 0b1000_0000 != 0 {
            self.status |= 0b1000_0000;
        } else {
            self.status &= 0b0111_1111;
        }
    }

    pub fn interpret(&mut self, program: Vec<u8>) {
        self.program_counter = 0;

        loop {
            let opcode = program[self.program_counter as usize];
            self.program_counter += 1;

            match opcode {
                0xA9 => { 
                    let param = program[self.program_counter as usize]; 
                    self.program_counter += 1; 
                    self.lda(param);
                }
                0xA5 => {
                    // LDA Zero Page
                }
                0xB5 => {
                    // LDA Zero Page,X
                }
                0xAD => {
                    // LDA Absolute
                }
                0xBD => {
                    // LDA Absolute,X
                }
                0xB9 => {
                    // LDA Absolute,Y
                }
                0xA1 => {
                    // LDA (Indirect,X)
                }
                0xB1 => {
                    // LDA (Indirect),Y
                }

                0xA2 => { 
                    let param = program[self.program_counter as usize]; 
                    self.program_counter += 1; 
                    self.ldx(param); 
                }
                0xA6 => {
                    // LDX Zero Page
                }
                0xB6 => {
                    // LDX Zero Page,Y
                }
                0xAE => {
                    // LDX Absolute
                }
                0xBE => {
                    // LDX Absolute,Y
                }

                0xA0 => { 
                    let param = program[self.program_counter as usize]; 
                    self.program_counter += 1; 
                    self.ldy(param); 
                }
                0xA4 => {
                    // LDY Zero Page
                }
                0xB4 => {
                    // LDY Zero Page,X
                }
                0xAC => {
                    // LDY Absolute
                }
                0xBC => {
                    // LDY Absolute,X
                }

                0x85 => {
                    // STA Zero Page
                }
                0x95 => {
                    // STA Zero Page,X
                }
                0x8D => {
                    // STA Absolute
                }
                0x9D => {
                    // STA Absolute,X
                }
                0x99 => {
                    // STA Absolute,Y
                }
                0x81 => {
                    // STA (Indirect,X)
                }
                0x91 => {
                    // STA (Indirect),Y
                }

                0x86 => {
                    // STX Zero Page
                }
                0x96 => {
                    // STX Zero Page,Y
                }
                0x8E => {
                    // STX Absolute
                }

                0x84 => {
                    // STY Zero Page
                }
                0x94 => {
                    // STY Zero Page,X
                }
                0x8C => {
                    // STY Absolute
                }

                0xAA => self.tax(),
                0xA8 => self.tay(),
                0x8A => self.txa(),
                0x98 => self.tya(),
                0xBA => self.tsx(),
                0x9A => self.txs(),

                0x48 => self.pha(),
                0x68 => self.pla(),
                0x08 => self.php(),
                0x28 => self.plp(),

                0x69 => { 
                    let param = program[self.program_counter as usize]; 
                    self.program_counter += 1; 
                    self.adc(param); 
                }
                0x65 => {
                    // ADC Zero Page
                }
                0x75 => {
                    // ADC Zero Page,X
                }
                0x6D => {
                    // ADC Absolute
                }
                0x7D => {
                    // ADC Absolute,X
                }
                0x79 => {
                    // ADC Absolute,Y
                }
                0x61 => {
                    // ADC (Indirect,X)
                }
                0x71 => {
                    // ADC (Indirect),Y
                }

                0xE9 => { 
                    let param = program[self.program_counter as usize]; 
                    self.program_counter += 1; 
                    self.sbc(param); 
                }
                0xE5 => {
                    // SBC Zero Page
                }
                0xF5 => {
                    // SBC Zero Page,X
                }
                0xED => {
                    // SBC Absolute
                }
                0xFD => {
                    // SBC Absolute,X
                }
                0xF9 => {
                    // SBC Absolute,Y
                }
                0xE1 => {
                    // SBC (Indirect,X)
                }
                0xF1 => {
                    // SBC (Indirect),Y
                }

                0xE8 => self.inx(),
                0xC8 => self.iny(),
                0xCA => self.dex(),
                0x88 => self.dey(),

                0xE6 => {
                    // INC Zero Page
                }
                0xF6 => {
                    // INC Zero Page,X
                }
                0xEE => {
                    // INC Absolute
                }
                0xFE => {
                    // INC Absolute,X
                }

                0xC6 => {
                    // DEC Zero Page
                }
                0xD6 => {
                    // DEC Zero Page,X
                }
                0xCE => {
                    // DEC Absolute
                }
                0xDE => {
                    // DEC Absolute,X
                }

                0x0A => self.asl_accumulator(),
                0x06 => {
                    // ASL Zero Page
                }
                0x16 => {
                    // ASL Zero Page,X
                }
                0x0E => {
                    // ASL Absolute
                }
                0x1E => {
                    // ASL Absolute,X
                }

                0x4A => self.lsr_accumulator(),
                0x46 => {
                    // LSR Zero Page
                }
                0x56 => {
                    // LSR Zero Page,X
                }
                0x4E => {
                    // LSR Absolute
                }
                0x5E => {
                    // LSR Absolute,X
                }

                0x2A => self.rol_accumulator(),
                0x26 => {
                    // ROL Zero Page
                }
                0x36 => {
                    // ROL Zero Page,X
                }
                0x2E => {
                    // ROL Absolute
                }
                0x3E => {
                    // ROL Absolute,X
                }

                0x6A => self.ror_accumulator(),
                0x66 => {
                    // ROR Zero Page
                }
                0x76 => {
                    // ROR Zero Page,X
                }
                0x6E => {
                    // ROR Absolute
                }
                0x7E => {
                    // ROR Absolute,X
                }

                0x4C => {
                    // JMP Absolute
                }
                0x6C => {
                    // JMP Indirect
                }
                0x20 => {
                    // JSR Absolute
                }
                0x60 => {
                    // RTS
                }

                0x90 => {
                    // BCC
                }
                0xB0 => {
                    // BCS
                }
                0xF0 => {
                    // BEQ
                }
                0xD0 => {
                    // BNE
                }
                0x30 => {
                    // BMI
                }
                0x10 => {
                    // BPL
                }
                0x50 => {
                    // BVC
                }
                0x70 => {
                    // BVS
                }

                0x18 => self.clc(),
                0xD8 => self.cld(),
                0x58 => self.cli(),
                0xB8 => self.clv(),
                0x38 => self.sec(),
                0xF8 => self.sed(),
                0x78 => self.sei(),

                0x29 => { 
                    let param = program[self.program_counter as usize]; 
                    self.program_counter += 1; 
                    self.and(param); 
                }
                0x25 => {
                    // AND Zero Page
                }
                0x35 => {
                    // AND Zero Page,X
                }
                0x2D => {
                    // AND Absolute
                }
                0x3D => {
                    // AND Absolute,X
                }
                0x39 => {
                    // AND Absolute,Y
                }
                0x21 => {
                    // AND (Indirect,X)
                }
                0x31 => {
                    // AND (Indirect),Y
                }

                0x49 => { 
                    let param = program[self.program_counter as usize]; 
                    self.program_counter += 1; 
                    self.eor(param); 
                }
                0x45 => {
                    // EOR Zero Page
                }
                0x55 => {
                    // EOR Zero Page,X
                }
                0x4D => {
                    // EOR Absolute
                }
                0x5D => {
                    // EOR Absolute,X
                }
                0x59 => {
                    // EOR Absolute,Y
                }
                0x41 => {
                    // EOR (Indirect,X)
                }
                0x51 => {
                    // EOR (Indirect),Y
                }

                0x09 => { 
                    let param = program[self.program_counter as usize]; 
                    self.program_counter += 1; 
                    self.ora(param); 
                }
                0x05 => {
                    // ORA Zero Page
                }
                0x15 => {
                    // ORA Zero Page,X
                }
                0x0D => {
                    // ORA Absolute
                }
                0x1D => {
                    // ORA Absolute,X
                }
                0x19 => {
                    // ORA Absolute,Y
                }
                0x01 => {
                    // ORA (Indirect,X)
                }
                0x11 => {
                    // ORA (Indirect),Y
                }

                0xC9 => { 
                    let param = program[self.program_counter as usize]; 
                    self.program_counter += 1; 
                    self.cmp(param); 
                }
                0xC5 => {
                    // CMP Zero Page
                }
                0xD5 => {
                    // CMP Zero Page,X
                }
                0xCD => {
                    // CMP Absolute
                }
                0xDD => {
                    // CMP Absolute,X
                }
                0xD9 => {
                    // CMP Absolute,Y
                }
                0xC1 => {
                    // CMP (Indirect,X)
                }
                0xD1 => {
                    // CMP (Indirect),Y
                }

                0xE0 => { 
                    let param = program[self.program_counter as usize]; 
                    self.program_counter += 1; 
                    self.cpx(param); 
                }
                0xE4 => {
                    // CPX Zero Page
                }
                0xEC => {
                    // CPX Absolute
                }

                0xC0 => { 
                    let param = program[self.program_counter as usize]; 
                    self.program_counter += 1; 
                    self.cpy(param); 
                }
                0xC4 => {
                    // CPY Zero Page
                }
                0xCC => {
                    // CPY Absolute
                }

                0x24 => {
                    // BIT Zero Page
                }
                0x2C => {
                    // BIT Absolute
                }

                0x00 => return,
                0xEA => {}

                _ => panic!("Unknown opcode: {:#X}", opcode),
            }
        }
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_0xa9_lda_immediate_load_data() {
        let mut cpu = CPU::new();
        cpu.interpret(vec![0xa9, 0x05, 0x00]);
        assert_eq!(cpu.register_a, 5);
        assert!(cpu.status & 0b0000_0010 == 0);
        assert!(cpu.status & 0b1000_0000 == 0);
    }

    #[test]
    fn test_0xa9_lda_zero_flag() {
        let mut cpu = CPU::new();
        cpu.interpret(vec![0xa9, 0x00, 0x00]);
        assert!(cpu.status & 0b0000_0010 == 0b10);
    }

    #[test]
    fn test_0xa9_lda_negative_flag() {
        let mut cpu = CPU::new();
        cpu.interpret(vec![0xa9, 0xff, 0x00]);
        assert!(cpu.status & 0b1000_0000 == 0b1000_0000);
    }

    #[test]
    fn test_0xaa_tax_move_a_to_x() {
        let mut cpu = CPU::new();
        cpu.register_a = 10;
        cpu.interpret(vec![0xaa, 0x00]);
        assert_eq!(cpu.register_x, 10);
    }

    #[test]
    fn test_5_ops_working_together() {
        let mut cpu = CPU::new();
        cpu.interpret(vec![0xa9, 0xc0, 0xaa, 0xe8, 0x00]);
        assert_eq!(cpu.register_x, 0xc1);
    }

    #[test]
    fn test_inx_overflow() {
        let mut cpu = CPU::new();
        cpu.register_x = 0xff;
        cpu.interpret(vec![0xe8, 0xe8, 0x00]);
        assert_eq!(cpu.register_x, 1);
    }

    #[test]
    fn test_0xc0_cpy_equal() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x10;
        cpu.interpret(vec![0xc0, 0x10, 0x00]);
        assert!(cpu.status & 0b0000_0010 != 0);
        assert!(cpu.status & 0b0000_0001 != 0);
    }

    #[test]
    fn test_0xc0_cpy_less_than() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x05;
        cpu.interpret(vec![0xc0, 0x10, 0x00]);
        assert!(cpu.status & 0b0000_0010 == 0);
        assert!(cpu.status & 0b0000_0001 == 0);
        assert!(cpu.status & 0b1000_0000 != 0);
    }

    #[test]
    fn test_0xc0_cpy_greater_than() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x20;
        cpu.interpret(vec![0xc0, 0x10, 0x00]);
        assert!(cpu.status & 0b0000_0010 == 0);
        assert!(cpu.status & 0b0000_0001 != 0);
        assert!(cpu.status & 0b1000_0000 == 0);
    }

    #[test]
    fn test_0xe0_cpx_equal() {
        let mut cpu = CPU::new();
        cpu.register_x = 0x42;
        cpu.interpret(vec![0xe0, 0x42, 0x00]);
        assert!(cpu.status & 0b0000_0010 != 0);
        assert!(cpu.status & 0b0000_0001 != 0);
        assert!(cpu.status & 0b1000_0000 == 0);
    }

    #[test]
    fn test_0xe0_cpx_less_than() {
        let mut cpu = CPU::new();
        cpu.register_x = 0x10;
        cpu.interpret(vec![0xe0, 0x20, 0x00]);
        assert!(cpu.status & 0b0000_0010 == 0);
        assert!(cpu.status & 0b0000_0001 == 0);
        assert!(cpu.status & 0b1000_0000 != 0);
    }

    #[test]
    fn test_0xe0_cpx_greater_than() {
        let mut cpu = CPU::new();
        cpu.register_x = 0x30;
        cpu.interpret(vec![0xe0, 0x10, 0x00]);
        assert!(cpu.status & 0b0000_0010 == 0);
        assert!(cpu.status & 0b0000_0001 != 0);
        assert!(cpu.status & 0b1000_0000 == 0);
    }

    #[test]
    fn test_0xa2_ldx_immediate() {
        let mut cpu = CPU::new();
        cpu.interpret(vec![0xa2, 0x0F, 0x00]);
        assert_eq!(cpu.register_x, 0x0F);
        assert!(cpu.status & 0b0000_0010 == 0);
        assert!(cpu.status & 0b1000_0000 == 0);
    }

    #[test]
    fn test_0xa2_ldx_zero_flag() {
        let mut cpu = CPU::new();
        cpu.interpret(vec![0xa2, 0x00, 0x00]);
        assert_eq!(cpu.register_x, 0x00);
        assert!(cpu.status & 0b0000_0010 != 0);
    }

    #[test]
    fn test_0xa2_ldx_negative_flag() {
        let mut cpu = CPU::new();
        cpu.interpret(vec![0xa2, 0xFF, 0x00]);
        assert_eq!(cpu.register_x, 0xFF);
        assert!(cpu.status & 0b1000_0000 != 0);
    }

    #[test]
    fn test_0xa0_ldy_immediate() {
        let mut cpu = CPU::new();
        cpu.interpret(vec![0xa0, 0x42, 0x00]);
        assert_eq!(cpu.register_y, 0x42);
        assert!(cpu.status & 0b0000_0010 == 0);
        assert!(cpu.status & 0b1000_0000 == 0);
    }

    #[test]
    fn test_0xa0_ldy_zero_flag() {
        let mut cpu = CPU::new();
        cpu.interpret(vec![0xa0, 0x00, 0x00]);
        assert_eq!(cpu.register_y, 0x00);
        assert!(cpu.status & 0b0000_0010 != 0);
    }

    #[test]
    fn test_0xa0_ldy_negative_flag() {
        let mut cpu = CPU::new();
        cpu.interpret(vec![0xa0, 0x80, 0x00]);
        assert_eq!(cpu.register_y, 0x80);
        assert!(cpu.status & 0b1000_0000 != 0);
    }

    #[test]
    fn test_iny_basic_increment() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x10;
        cpu.iny();
        assert_eq!(cpu.register_y, 0x11);
        assert_eq!(cpu.status & 0b0000_0010, 0);
        assert_eq!(cpu.status & 0b1000_0000, 0);
    }

    #[test]
    fn test_iny_overflow_wraps_to_zero() {
        let mut cpu = CPU::new();
        cpu.register_y = 0xFF;
        cpu.iny();
        assert_eq!(cpu.register_y, 0x00);
        assert_ne!(cpu.status & 0b0000_0010, 0);
        assert_eq!(cpu.status & 0b1000_0000, 0);
    }

    #[test]
    fn test_iny_sets_negative_flag() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x7F;
        cpu.iny();
        assert_eq!(cpu.register_y, 0x80);
        assert_eq!(cpu.status & 0b0000_0010, 0);
        assert_ne!(cpu.status & 0b1000_0000, 0);
    }

    #[test]
    fn test_tya_transfer() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x42;
        cpu.register_a = 0x00;
        cpu.tya();
        assert_eq!(cpu.register_a, 0x42);
        assert_eq!(cpu.status & 0b0000_0010, 0);
        assert_eq!(cpu.status & 0b1000_0000, 0);
    }

    #[test]
    fn test_tya_sets_zero_flag() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x00;
        cpu.register_a = 0xFF;
        cpu.tya();
        assert_eq!(cpu.register_a, 0x00);
        assert_ne!(cpu.status & 0b0000_0010, 0);
        assert_eq!(cpu.status & 0b1000_0000, 0);
    }

    #[test]
    fn test_tya_sets_negative_flag() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x80;
        cpu.register_a = 0x00;
        cpu.tya();
        assert_eq!(cpu.register_a, 0x80);
        assert_eq!(cpu.status & 0b0000_0010, 0);
        assert_ne!(cpu.status & 0b1000_0000, 0);
    }

    #[test]
    fn test_txa_transfer() {
        let mut cpu = CPU::new();
        cpu.register_x = 0x42;
        cpu.register_a = 0x00;
        cpu.txa();
        assert_eq!(cpu.register_a, 0x42);
        assert_eq!(cpu.status & 0b0000_0010, 0);
        assert_eq!(cpu.status & 0b1000_0000, 0);
    }

    #[test]
    fn test_txa_sets_zero_flag() {
        let mut cpu = CPU::new();
        cpu.register_x = 0x00;
        cpu.register_a = 0xFF;
        cpu.txa();
        assert_eq!(cpu.register_a, 0x00);
        assert_ne!(cpu.status & 0b0000_0010, 0);
        assert_eq!(cpu.status & 0b1000_0000, 0);
    }

    #[test]
    fn test_txa_sets_negative_flag() {
        let mut cpu = CPU::new();
        cpu.register_x = 0x80;
        cpu.register_a = 0x00;
        cpu.txa();
        assert_eq!(cpu.register_a, 0x80);
        assert_eq!(cpu.status & 0b0000_0010, 0);
        assert_ne!(cpu.status & 0b1000_0000, 0);
    }

    #[test]
    fn test_dex_basic_decrement() {
        let mut cpu = CPU::new();
        cpu.register_x = 0x10;
        cpu.dex();
        assert_eq!(cpu.register_x, 0x0F);
        assert_eq!(cpu.status & 0b0000_0010, 0);
        assert_eq!(cpu.status & 0b1000_0000, 0);
    }

    #[test]
    fn test_dex_underflow_wraps_to_ff() {
        let mut cpu = CPU::new();
        cpu.register_x = 0x00;
        cpu.dex();
        assert_eq!(cpu.register_x, 0xFF);
        assert_eq!(cpu.status & 0b0000_0010, 0);
        assert_ne!(cpu.status & 0b1000_0000, 0);
    }

    #[test]
    fn test_dex_sets_zero_flag() {
        let mut cpu = CPU::new();
        cpu.register_x = 0x01;
        cpu.dex();
        assert_eq!(cpu.register_x, 0x00);
        assert_ne!(cpu.status & 0b0000_0010, 0);
        assert_eq!(cpu.status & 0b1000_0000, 0);
    }

    #[test]
    fn test_tay_transfer() {
        let mut cpu = CPU::new();
        cpu.register_a = 0x37;
        cpu.register_y = 0x00;
        cpu.tay();
        assert_eq!(cpu.register_y, 0x37);
        assert_eq!(cpu.status & 0b0000_0010, 0);
        assert_eq!(cpu.status & 0b1000_0000, 0);
    }

    #[test]
    fn test_tay_sets_zero_flag() {
        let mut cpu = CPU::new();
        cpu.register_a = 0x00;
        cpu.register_y = 0xFF;
        cpu.tay();
        assert_eq!(cpu.register_y, 0x00);
        assert_ne!(cpu.status & 0b0000_0010, 0);
        assert_eq!(cpu.status & 0b1000_0000, 0);
    }

    #[test]
    fn test_tay_sets_negative_flag() {
        let mut cpu = CPU::new();
        cpu.register_a = 0x80;
        cpu.register_y = 0x00;
        cpu.tay();
        assert_eq!(cpu.register_y, 0x80);
        assert_eq!(cpu.status & 0b0000_0010, 0);
        assert_ne!(cpu.status & 0b1000_0000, 0);
    }

    #[test]
    fn test_dey_basic_decrement() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x10;
        cpu.dey();
        assert_eq!(cpu.register_y, 0x0F);
        assert_eq!(cpu.status & 0b0000_0010, 0);
        assert_eq!(cpu.status & 0b1000_0000, 0);
    }

    #[test]
    fn test_dey_wraps_to_ff() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x00;
        cpu.dey();
        assert_eq!(cpu.register_y, 0xFF);
        assert_eq!(cpu.status & 0b0000_0010, 0);
        assert_ne!(cpu.status & 0b1000_0000, 0);
    }

    #[test]
    fn test_dey_sets_zero_flag() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x01;
        cpu.dey();
        assert_eq!(cpu.register_y, 0x00);
        assert_ne!(cpu.status & 0b0000_0010, 0);
        assert_eq!(cpu.status & 0b1000_0000, 0);
    }
}