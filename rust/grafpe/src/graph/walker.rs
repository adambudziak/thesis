pub trait Walker {
    fn go(self: &Self, start: u32, walk_length: usize, random_buf: &[u8]) -> u32;
    fn go_back(self: &Self, start: u32, walk_length: usize, random_buf: &[u8]) -> u32;
    fn bits_per_step(&self) -> usize;
}
