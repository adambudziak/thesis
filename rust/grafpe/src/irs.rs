use crate::prng::Prng;

pub struct IrsCtx {
    numbers_cp: Vec<u64>,
    bit_slots: Vec<u64>,
    bit_slots_cp: Vec<u64>,
}

impl IrsCtx {
    pub fn new(numbers: &[u64]) -> IrsCtx {
        IrsCtx {
            numbers_cp: numbers.to_vec(),
            bit_slots: vec![0; numbers.len()],
            bit_slots_cp: vec![0; numbers.len()],
        }
    }

    pub fn clear_slots(&mut self) {
        for elem in &mut self.bit_slots {
            *elem = 0;
        }
    }
}

pub struct Irs {
    prng: Prng,
}

impl Irs {
    pub fn new(prng: Prng) -> Irs {
        Irs { prng }
    }

    pub fn permute(&mut self, data: &mut Vec<u64>, ctx: IrsCtx) -> IrsCtx {
        let mut data = data;
        let mut ctx = ctx;
        loop {
            ctx = self.one_round(&mut data, ctx);

            if Irs::all_distinct(&ctx.bit_slots) {
                break ctx
            }
        }
    }

    fn all_distinct(sorted_data: &[u64]) -> bool {
        for i in 1..sorted_data.len() {
            if sorted_data[i - 1] == sorted_data[i] {
                return false;
            }
        }
        true
    }

    pub fn one_round(&mut self, data: &mut Vec<u64>, ctx: IrsCtx) -> IrsCtx {
        let mut data = data;
        let mut ctx = ctx;
        let mut odd_count = 0;
        for slot in ctx.bit_slots.iter_mut() {
            *slot = (*slot << 1) | (self.prng.fetch_bit() as u64);
            odd_count += *slot & 1;
        }

        let mut odd_moved = 0;
        let mut even_moved = 0;

        let mut position;

        let size = data.len() as u64;

        for i in (0..size).rev() {
            let current = ctx.bit_slots[i as usize];
            if current % 2 == 1 {
                position = size - odd_moved - 1;
                odd_moved += 1;
            } else {
                position = size - odd_count - even_moved - 1;
                even_moved += 1;
            }
            ctx.bit_slots_cp[position as usize] = current;
            ctx.numbers_cp[position as usize] = data[i as usize];
        }

        std::mem::swap(&mut ctx.numbers_cp, &mut data);
        std::mem::swap(&mut ctx.bit_slots_cp, &mut ctx.bit_slots);

        ctx
    }
}
