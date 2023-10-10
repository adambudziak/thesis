use itertools::join;
use std::fs::File;
use std::io::BufRead;
use std::io::BufReader;
use std::error::Error;

use crate::graph::ValidGraph;
use crate::perm::inverse;

pub fn to_perm_directed(ctx: &ValidGraph, walk_length: usize) -> String {
    String::from(format!("S\t{}\n", walk_length))
        + &format!("N\t{}\tD\t{}\n", ctx.n, ctx.d)
        + &join((&ctx.permutations[..ctx.d/2]).iter()
            .map(|p| join(p, "\t")), "\n")
}

pub fn to_perm_undirected(ctx: &ValidGraph, walk_length: usize) -> String {
    String::from(format!("S\t{}\n", walk_length))
        + &format!("N\t{}\tD\t{}\n", ctx.n, ctx.d)
        + &join(ctx.permutations.iter()
            .map(|p| join(p, "\t")), "\n")
}


pub fn from_perm_directed(filepath: &str) -> Result<ValidGraph, Box<Error>> {
    let mut buf_reader = BufReader::new(File::open(filepath)?);
    let mut line = String::new();
    buf_reader.read_line(&mut line)?;
    line.clear();
    buf_reader.read_line(&mut line)?;
    let line_split: Vec<&str> = line.split_whitespace().collect();
    let n = line_split[1].parse::<usize>()?;
    let d = line_split[3].parse::<usize>()?;
    let mut permutations: Vec<Vec<u64>> = Vec::with_capacity(d/2);
    for _ in 0..d/2 {
        line.clear();
        buf_reader.read_line(&mut line)?;
        let line_split = line.split_whitespace();
        let permutation: Result<Vec<u64>, _> = line_split.map(|c| c.parse::<u64>())
            .collect();
        permutations.push(permutation?);
    }

    for i in 0..d/2 {
        permutations.push(inverse(&permutations[i]));
    }

    Ok(ValidGraph {n, d, permutations})
}

pub fn walk_length_from_perm(filepath: &str) -> Result<usize, Box<Error>> {
    let mut buf_reader = BufReader::new(File::open(filepath)?);
    let mut line = String::new();
    buf_reader.read_line(&mut line)?;
    let _line_split: Vec<&str> = line.split_whitespace().collect();
    if let Some(wl) = line.split_whitespace().nth(1) {
        wl.parse::<usize>().map_err(|e| Box::from(e))
    } else {
        Err(Box::new(std::fmt::Error))
    }
}


#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_small_graph_to_perm() {
        let graph = ValidGraph {
            n: 6,
            d: 2,
            permutations: vec![
                vec![1, 2, 3, 4, 5, 6],
                vec![2, 4, 6, 8, 10, 12]
            ]
        };

        let result_raw = to_perm_directed(&graph, 80);
        print!("RESULT:\n{}\n", result_raw);
        let result: Vec<&str> = result_raw.split('\n').collect();
        assert_eq!(result.len(), 3);
        assert_eq!(result[0], "S\t80");
        assert_eq!(result[1], "N\t6\tD\t2");
        assert_eq!(result[2], "1\t2\t3\t4\t5\t6");
    }
}
