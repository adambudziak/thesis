use rayon::iter::IntoParallelRefIterator;
use rayon::prelude::*;

use rug::Float;
use nalgebra::{Matrix, Dynamic, VecStorage};

type GMatrix = Matrix<Float, Dynamic, Dynamic, VecStorage<f64, Dynamic, Dynamic>>;

pub fn rotate_permutations(permutations: &[Vec<u64>]) -> Vec<Vec<u32>> {

    let d = permutations.len();
    let n = permutations[0].len();

    let mut rotated = vec![vec![0; d]; n];

    for pi in 0..d {
        for xi in 0..n {
            rotated[xi][pi] = permutations[pi][xi] as u32;
        }
    }

    rotated
}

pub fn permutations_to_matrix(rotated: &[Vec<u32>]) -> Vec<Vec<Float>> {
    let n = rotated.len();
    let d = rotated[0].len();
    (0..n).map(|row_i| {
        let mut new_row = vec![Float::with_val(32, 0.); n];
        for &tgt in &rotated[row_i] {
            new_row[tgt as usize] = Float::with_val(32, 1.) / d as f64;
        }
        new_row
    }).collect()
}

pub fn multiply_matrix(rotated: &[Vec<u32>], matrix: &[Vec<Float>]) -> Vec<Vec<Float>> {
    let n = rotated.len();
    let d = rotated[0].len() as f64;

    (0..n).into_par_iter()
        .map(|row_i|
            (0..n).map(|col_i|
                Float::with_val(272, &rotated[row_i].iter()
                    .fold(Float::with_val(272, 0.),
                          |s, &tgt| s + &matrix[tgt as usize][col_i])
                / d)
            ).collect()
    ).collect()
}


//pub fn compute_tvd(mat: &Vec<Vec<Float>>) -> Float {
//    let n = mat.ncols();
//    mat.row_iter()
//        .map(|row|
//            row.iter().fold(Float::with_val(272, 0.),
//                            |s, c| s + (c - Float::with_val(272, 1.) / n as f64).abs()))
//        .reduce(|| Float::with_val(272, 0.), |m, c| m.max(&c)) / 2.
//}
pub fn compute_tvd(matrix: &[Vec<Float>]) -> Float {
    let n = matrix.len();

    matrix.par_iter()
        .map(|row|
            row.iter().fold(Float::with_val(272, 0.),
                            |s, c| s + (c - Float::with_val(272, 1.) / n as f64).abs()))
        .reduce(|| Float::with_val(272, 0.), |m, c| m.max(&c)) / 2.
}