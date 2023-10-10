pub mod error;
pub mod valid;

pub fn new(size: usize) -> Vec<u64> {
    (0..size as u64).collect()
}

pub fn inverse(permutation: &[u64]) -> Vec<u64> {
    let mut inv: Vec<u64> = vec![0; permutation.len()];

    for i in 0..inv.len() {
        inv[permutation[i] as usize] = i as u64;
    }

    inv
}
