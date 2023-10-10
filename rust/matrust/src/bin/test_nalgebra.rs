use nalgebra::{Matrix, Dynamic, VecStorage, DVector};
use matrust::rotate_permutations;

type GMatrix = Matrix<f64, Dynamic, Dynamic, VecStorage<f64, Dynamic, Dynamic>>;

fn compute_tvd(mat: &GMatrix, n: f64) -> f64 {
    mat.row_iter().map(|row| row.iter().fold(0.0, |s, c| s + (c - 1./n).abs()))
        .fold(0.0, f64::max) / 2.
}

fn main() {
    let permutations: Vec<Vec<u64>> = vec![
        vec![6, 7, 3, 0, 2, 1, 5, 4] ,
        vec![4, 2 ,5, 7, 6, 3, 1, 0],
    ];
//    let mat = GMatrix::from_row_slice(3, 3, &[1., 2., 3., 4., 5., 6., 7., 8., 9.]);
    let mut mat = GMatrix::from_element(8, 8, 0_f64);
    let rotated = rotate_permutations(&permutations);
    for (row_i, row) in rotated.into_iter().enumerate() {
        for i in row {
            *mat.index_mut((row_i, i as usize)) = 0.5;
        }
    }
    for i in 0..10 {
        let tvd = compute_tvd(&mat, 8_f64);
        println!("tvd: {}", tvd);
        mat = &mat * &mat;
    }
}
