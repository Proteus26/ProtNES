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

    fn tax(&mut self) {
        self.register_x = self.register_a;
        self.update_zero_and_negative_flags(self.register_x);
    }

    fn inx(&mut self) {
        self.register_x = self.register_x.wrapping_add(1);
        self.update_zero_and_negative_flags(self.register_x);
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

    fn ldx(&mut self, value: u8) {
        self.register_x = value;
        self.update_zero_and_negative_flags(self.register_x);
    }

    fn ldy(&mut self, value: u8) {
        self.register_y = value;
        self.update_zero_and_negative_flags(self.register_y);
    }

    fn iny(&mut self) {
        self.register_y = self.register_y.wrapping_add(1);
        self.update_zero_and_negative_flags(self.register_y);
    }

    fn tya(&mut self) {
        self.register_a = self.register_y;
        self.update_zero_and_negative_flags(self.register_a);
    }

    fn txa(&mut self) {
        self.register_a = self.register_x;
        self.update_zero_and_negative_flags(self.register_a);
    }

    fn dex(&mut self) {
        self.register_x = self.register_x.wrapping_sub(1);
        self.update_zero_and_negative_flags(self.register_x);
    }

    fn tay(&mut self) {
        self.register_y = self.register_a;
        self.update_zero_and_negative_flags(self.register_y);
    }

    fn dey(&mut self) {
        self.register_y = self.register_y.wrapping_sub(1);
        self.update_zero_and_negative_flags(self.register_y);
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

                0xAA => self.tax(),

                0xE8 => self.inx(),

                0xC0 => {
                    let param = program[self.program_counter as usize];
                    self.program_counter += 1;
                    self.cpy(param);
                }

                0xE0 => {
                    let param = program[self.program_counter as usize];
                    self.program_counter += 1;
                    self.cpx(param);
                }

                0xA2 => {
                    let param = program[self.program_counter as usize];
                    self.program_counter += 1;
                    self.ldx(param);
                }

                0xA0 => {
                    let param = program[self.program_counter as usize];
                    self.program_counter += 1;
                    self.ldy(param);
                }

                0xC8 => self.iny(),

                0x98 => self.tya(),

                0x8A => self.txa(),

                0xCA => self.dex(),

                0xA8 => self.tay(),

                0x88 => self.dey(),

                0x00 => return,

                //NOP #imm, 0x80
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
        assert!(!cpu.status.zero());
        assert!(!cpu.status.negative());
    }

    #[test]
    fn test_iny_overflow_wraps_to_zero() {
        let mut cpu = CPU::new();
        cpu.register_y = 0xFF;

        cpu.iny();

        assert_eq!(cpu.register_y, 0x00);
        assert!(cpu.status.zero()); // 0x00 sets Zero flag
        assert!(!cpu.status.negative()); // Bit 7 not set
    }

    #[test]
    fn test_iny_sets_negative_flag() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x7F; // 127 → 128

        cpu.iny();

        assert_eq!(cpu.register_y, 0x80);
        assert!(!cpu.status.zero());
        assert!(cpu.status.negative()); // Bit 7 set
    }

    #[test]
    fn test_tya_transfer() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x42;
        cpu.register_a = 0x00;

        cpu.tya();

        assert_eq!(cpu.register_a, 0x42);
        assert!(!cpu.status.zero()); // Not zero
        assert!(!cpu.status.negative()); // Bit 7 not set
    }

    #[test]
    fn test_tya_sets_zero_flag() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x00;
        cpu.register_a = 0xFF;

        cpu.tya();

        assert_eq!(cpu.register_a, 0x00);
        assert!(cpu.status.zero()); // Should be set
        assert!(!cpu.status.negative()); // Should not be set
    }

    #[test]
    fn test_tya_sets_negative_flag() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x80; // Negative (bit 7 set)
        cpu.register_a = 0x00;

        cpu.tya();

        assert_eq!(cpu.register_a, 0x80);
        assert!(!cpu.status.zero()); // Should not be set
        assert!(cpu.status.negative()); // Should be set
    }

    #[test]
    fn test_txa_transfer() {
        let mut cpu = CPU::new();
        cpu.register_x = 0x42;
        cpu.register_a = 0x00;

        cpu.txa();

        assert_eq!(cpu.register_a, 0x42);
        assert!(!cpu.status.zero());
        assert!(!cpu.status.negative());
    }

    #[test]
    fn test_txa_sets_zero_flag() {
        let mut cpu = CPU::new();
        cpu.register_x = 0x00;
        cpu.register_a = 0xFF;

        cpu.txa();

        assert_eq!(cpu.register_a, 0x00);
        assert!(cpu.status.zero());
        assert!(!cpu.status.negative());
    }

    #[test]
    fn test_txa_sets_negative_flag() {
        let mut cpu = CPU::new();
        cpu.register_x = 0x80;
        cpu.register_a = 0x00;

        cpu.txa();

        assert_eq!(cpu.register_a, 0x80);
        assert!(!cpu.status.zero());
        assert!(cpu.status.negative());
    }

    #[test]
    fn test_dex_basic_decrement() {
        let mut cpu = CPU::new();
        cpu.register_x = 0x10;

        cpu.dex();

        assert_eq!(cpu.register_x, 0x0F);
        assert!(!cpu.status.zero());
        assert!(!cpu.status.negative());
    }

    #[test]
    fn test_dex_underflow_wraps_to_ff() {
        let mut cpu = CPU::new();
        cpu.register_x = 0x00;

        cpu.dex();

        assert_eq!(cpu.register_x, 0xFF);
        assert!(!cpu.status.zero());
        assert!(cpu.status.negative());
    }

    #[test]
    fn test_dex_sets_zero_flag() {
        let mut cpu = CPU::new();
        cpu.register_x = 0x01;

        cpu.dex();

        assert_eq!(cpu.register_x, 0x00);
        assert!(cpu.status.zero());
        assert!(!cpu.status.negative());
    }

    #[test]
    fn test_tay_transfer() {
        let mut cpu = CPU::new();
        cpu.register_a = 0x37;
        cpu.register_y = 0x00;

        cpu.tay();

        assert_eq!(cpu.register_y, 0x37);
        assert!(!cpu.status.zero());
        assert!(!cpu.status.negative());
    }

    #[test]
    fn test_tay_sets_zero_flag() {
        let mut cpu = CPU::new();
        cpu.register_a = 0x00;
        cpu.register_y = 0xFF;

        cpu.tay();

        assert_eq!(cpu.register_y, 0x00);
        assert!(cpu.status.zero());
        assert!(!cpu.status.negative());
    }

    #[test]
    fn test_tay_sets_negative_flag() {
        let mut cpu = CPU::new();
        cpu.register_a = 0x80;
        cpu.register_y = 0x00;

        cpu.tay();

        assert_eq!(cpu.register_y, 0x80);
        assert!(!cpu.status.zero());
        assert!(cpu.status.negative());
    }

    #[test]
    fn test_dey_basic_decrement() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x10;

        cpu.dey();

        assert_eq!(cpu.register_y, 0x0F);
        assert!(!cpu.status.zero());
        assert!(!cpu.status.negative());
    }

    #[test]
    fn test_dey_wraps_to_ff() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x00;

        cpu.dey();

        assert_eq!(cpu.register_y, 0xFF);
        assert!(!cpu.status.zero());
        assert!(cpu.status.negative());
    }

    #[test]
    fn test_dey_sets_zero_flag() {
        let mut cpu = CPU::new();
        cpu.register_y = 0x01;

        cpu.dey();

        assert_eq!(cpu.register_y, 0x00);
        assert!(cpu.status.zero());
        assert!(!cpu.status.negative());
    }
}
