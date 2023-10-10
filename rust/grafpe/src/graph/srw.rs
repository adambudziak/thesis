//! Module containing various implementations of Simple Random Walk over the graphs.
//!
use super::walker::Walker;

use crate::graph::{Graph, SecureDirected, SecureUndirected};

fn get_bits_from_u32(source: u32, start: usize, bits_per_step: usize) -> u32 {
    (source >> start) & ((1_u32 << bits_per_step) - 1)
}

fn get_bits(buffer: &[u8], start: usize, bits_per_step: usize) -> u32 {
    let mut result = u32::from(buffer[start / 8]);
    let byte_count = bits_per_step / 8;
    for i in 1..byte_count {  // TODO check for buffer overflows
        result |= u32::from(buffer[start / 8 + i]) << (i * 8);
    }
    get_bits_from_u32(result, start % 8, bits_per_step)
}

fn get_permutation(buffer: &[u8], bits_per_perm: usize, index: usize) -> u32 {
    get_bits(&buffer, index * bits_per_perm, bits_per_perm)
}

/// Simple Random Walk over undirected graphs.
pub struct UndirectedSrw<'a>
{
    perms: &'a[Vec<u64>],
    bits_per_perm: usize,
}

impl<'a> UndirectedSrw<'a>
{
    pub fn new<G: Graph + SecureUndirected>(graph: &'a G) -> Self {
        let perms = graph.permutations();
        let bits_per_perm = (perms.len() as f32 - 1.0).log2() as usize + 1;
        UndirectedSrw {
            perms,
            bits_per_perm,
        }
    }

    pub fn one_step(&self, start: u32, step_number: usize, random_buf: &[u8]) -> u32 {
        let p_index = get_permutation(&random_buf, self.bits_per_perm, step_number);
        self.perms[p_index as usize][start as usize] as u32
    }

    pub fn one_step_back(&self, start: u32, step_number: usize, random_buf: &[u8]) -> u32 {
        let p_index = get_permutation(&random_buf, self.bits_per_perm, step_number) as usize;
        let p_index = (p_index + self.perms.len() / 2) % self.perms.len();
        self.perms[p_index][start as usize] as u32
    }
}

impl<'a> Walker for UndirectedSrw<'a>
{
    fn go(&self, start: u32, walk_length: usize, random_buf: &[u8]) -> u32 {
        (0..walk_length).fold(start, |x, i|
                self.one_step(x, i, random_buf))
    }

    fn go_back(&self, start: u32, walk_length: usize, random_buf: &[u8]) -> u32 {
        (0..walk_length).rev().fold(start, |x, i|
                self.one_step_back(x, i, random_buf))
    }

    fn bits_per_step(&self) -> usize {
        self.bits_per_perm
    }
}

pub struct DirectedSrw<'a>
{
    bits_per_perm: usize,
    forward_perms: &'a[Vec<u64>],
    backward_perms: &'a[Vec<u64>]
}

impl<'a> DirectedSrw<'a>
{
    pub fn new<G: Graph + SecureDirected>(graph: &'a G) -> Self {
        let perms_cnt = graph.forward_permutations().len();
        let bits_per_perm = (perms_cnt as f32 - 1.0).log2() as usize + 1;
        DirectedSrw {
            bits_per_perm,
            forward_perms: graph.forward_permutations(),
            backward_perms: graph.backward_permutations(),
        }
    }

    pub fn one_step(&self, start: u32, step_number: usize, random_buf: &[u8]) -> u32 {
        let p_index = get_permutation(&random_buf, self.bits_per_perm, step_number);
        self.forward_perms[p_index as usize][start as usize] as u32
    }

    pub fn one_step_back(&self, start: u32, step_number: usize, random_buf: &[u8]) -> u32 {
        let p_index = get_permutation(&random_buf, self.bits_per_perm, step_number);
        self.backward_perms[p_index as usize][start as usize] as u32
    }
}

impl<'a> Walker for DirectedSrw<'a>
{
    fn go(&self, start: u32, walk_length: usize, random_buf: &[u8]) -> u32 {
        (0..walk_length).fold(start, |x, i|
            self.one_step(x, i, random_buf))
    }

    fn go_back(&self, start: u32, walk_length: usize, random_buf: &[u8]) -> u32 {
        (0..walk_length).rev().fold(start, |x, i|
            self.one_step_back(x, i, random_buf))
    }

    fn bits_per_step(&self) -> usize {
        self.bits_per_perm
    }
}
