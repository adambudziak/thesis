//! Module containing utility functions regarding valid permutations and valid permutations sets.
//! It contains functions for creating a new valid permutation, validating a permutation,
//! repermuting a permutation using a given shuffler (currently, only InverseRiffleShuffle is supported)
//! until it is valid.
use crate::irs;
use crate::perm::error::PermutationError;
use std::collections::HashSet;
use std::iter::FromIterator;

pub fn is_permutation(permutation: &[u64]) -> bool {
    let unique: HashSet<u64> = HashSet::from_iter(permutation.iter().map(|&x| x));
    return unique.len() == permutation.len();
}

/// Checks if the given permutation is valid.
///
/// A permutation `p` is valid if and only if it is non-empty and
/// for all indexes `i` there is `p[p[i]] != i`.
///
/// **Note:** This function *does not* check if the given slice is a permutation.
///
/// # Examples:
/// ```
/// use grafpe::perm::valid::is_valid;
/// let perm = vec![0, 1, 2, 3];
///
/// assert!(!is_valid(&perm)) // because perm[0] == 0
/// ```
///
/// ```
/// use grafpe::perm::valid::is_valid;
/// let perm = vec![1, 2, 3, 0];
/// assert!(is_valid(&perm))
/// ```
pub fn is_valid(permutation: &[u64]) -> bool {
    if permutation.is_empty() {
        return false;
    }

    for i in 0..permutation.len() {
        if permutation[permutation[i] as usize] == i as u64 {
            return false;
        }
    }
    true
}

pub fn new(size: usize, irs: &mut irs::Irs) -> Result<Vec<u64>, PermutationError> {
    if size < 3 {
        return Err(PermutationError::SizeTooSmall);
    }

    let mut numbers: Vec<u64> = (0..size as u64).collect();
    let ctx = irs::IrsCtx::new(&numbers);
    permute(&mut numbers, irs, ctx);

    Ok(numbers)
}

pub fn permute(numbers: &mut Vec<u64>, irs: &mut irs::Irs, ctx: irs::IrsCtx) -> irs::IrsCtx {
    let mut ctx = irs.permute(numbers, ctx);

    while !is_valid(&numbers) {
        ctx = irs.one_round(numbers, ctx);
    }

    ctx
}

pub fn repermute(numbers: &mut Vec<u64>, irs: &mut irs::Irs, ctx: irs::IrsCtx) -> irs::IrsCtx {

    let mut numbers = numbers;
    let mut ctx = ctx;
    loop {
        ctx = irs.one_round(&mut numbers, ctx);

        if is_valid(&numbers) {
            break ctx;
        }
    }
}

pub fn is_pair_valid(lhs: &[u64], rhs: &[u64]) -> bool {
    assert_eq!(
        lhs.len(),
        rhs.len(),
        "is_pair_valid slices must have equal length"
    );
    for i in 0..lhs.len() {
        if i as u64 == lhs[rhs[i] as usize] || lhs[i] == rhs[i] {
            return false;
        }
    }
    true
}

pub fn is_new_valid(permutations: &[Vec<u64>], new: &[u64]) -> bool {
    permutations.iter()
        .find(|p| !is_pair_valid(p, new))
        .is_none()
}

#[cfg(test)]
mod test {
    use super::*;
    use crate::prng;
    use openssl::symm::{Cipher, Crypter, Mode};

    fn get_irs() -> irs::Irs {
        let key = hex::decode("01230123012301230123012301230123").unwrap();
        let iv = hex::decode("00001111222233330000111122223333").unwrap();

        irs::Irs::new(prng::Prng::new(
            Crypter::new(Cipher::aes_128_ctr(), Mode::Encrypt, &key, Some(&iv)).unwrap(),
        ))
    }

    #[test]
    fn test_new() {
        let mut irs = get_irs();
        assert!(new(0, &mut irs).is_err());
        assert!(new(1, &mut irs).is_err());
        assert!(new(2, &mut irs).is_err());
        assert_eq!(new(3, &mut irs).unwrap(), vec![1, 2, 0]);
        assert_eq!(new(4, &mut irs).unwrap(), vec![3, 0, 1, 2]);
        assert_eq!(new(5, &mut irs).unwrap(), vec![1, 3, 0, 4, 2]);
    }

    #[test]
    fn test_valid() {
        assert!(!is_valid(&Vec::new()));
        assert!(!is_valid(&vec![0]));
    }

    #[test]
    #[should_panic]
    fn valid_does_not_check_bounds() {
        is_valid(&vec![1]);
    }

    #[test]
    fn duplicate_values() {
        // is_valid doesn't check if the vector is a permutation
        assert!(is_valid(&vec![1, 2, 0, 1]));
        assert!(!is_valid(&vec![1, 1, 1, 1]));
    }

    #[test]
    fn new_are_valid() {
        let mut irs = get_irs();
        for i in 10..20 {
            assert!(is_valid(&new(i, &mut irs).unwrap()));
        }
    }
}
