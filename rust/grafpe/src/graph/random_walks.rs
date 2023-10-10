use matrust::*;

pub fn graph_exp_tvd(perms: &[Vec<u64>], sec_param: usize) -> usize {
    let rotated = rotate_permutations(perms);
    let mut matrix = permutations_to_matrix(&rotated);
    let mut result = 0;

    loop {
        let tvd = compute_tvd(&matrix);
        let security = tvd.log2() * -1;
        if security > sec_param {
            break result;
        }
        matrix = multiply_matrix(&rotated, &matrix);
        result += 1;
    }
}
