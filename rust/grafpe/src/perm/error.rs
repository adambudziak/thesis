use std::error::Error;
use std::fmt::{self, Formatter};

#[derive(Debug)]
pub enum PermutationError {
    SizeTooSmall,
}

impl Error for PermutationError {}

impl fmt::Display for PermutationError {
    fn fmt(&self, f: &mut Formatter) -> fmt::Result {
        write!(f, "Permutation error occurred.")
    }
}
