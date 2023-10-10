use std::collections::{ HashSet };
use rand::thread_rng;
use rand::Rng;
use std::iter::FromIterator;
use grafpe::graph::{ValidGraph, is_valid};

use grafpe::graph::convertible::{Convertible, GraphFormat};





fn main() {
    let N = 10000_usize;
    let D = 32_usize;
    let mut rng = thread_rng();
    let mut counter = 0;
    let graph = loop {
        let mut unp_from: HashSet<usize> = HashSet::from_iter((0..N*D).map(|x| x));
        let mut unp_to: HashSet<usize> = HashSet::from_iter((0..N*D).map(|x| x));
        let mut permutations: Vec<Vec<u64>> = vec![vec![0; N]; D * 2];
        let mut vert_perm: Vec<Vec<usize>> = vec![vec![]; N];
        loop {
            let left = loop {
                let val: usize = rng.gen_range(0, N * D);
                if unp_from.contains(&val) { break val }
            };
            let right = loop {
                let val = rng.gen_range(0, N * D);
                if unp_to.contains(&val) { break val }
            };
            if left / D != right / D
              && !vert_perm[left / D].contains(&(right / D))
              && !vert_perm[right / D].contains(&(left / D)) {
                vert_perm[left / D].push(right / D);
                unp_from.remove(&left);
                unp_to.remove(&right);
            }
            if unp_from.len() == 0 { break; }
            let mut possible = false;
            for u in &unp_from {
                for u2 in &unp_to {
                    if u % (N / D) != u2 % (N / D) {
                        possible = true;
                        break;
                    }
                }
                if possible { break; }
            }
            if !possible { break; }
        }
        let mut all_valid = true;
        for c in &vert_perm {
            if c.len() != D { all_valid = false; break; }
        }
        counter += 1;
        if counter % 1000 == 0 { eprintln!("{}", counter); }

        if all_valid {
            for i in 0..D {
                for j in 0..N {
                    permutations[i][j] = vert_perm[j][i] as u64;
                }
            }
            let graph = ValidGraph::from_permutations(N, D * 2, permutations);
            if is_valid(&graph) {
                break graph
            }
        }

//        let graph = convert(&edges, N, D*2);
//        let graph = Graph { n: N, d: D, permutations };
//        println!("{}", to_perm_directed(&graph, 0));
//        if is_valid(&graph) { break graph }
//        let mut all_valid = true;
//        for p in &graph.permutations[..graph.permutations.len()/2] {
//            if !is_permutation(&p)  {
//                all_valid = false;
//                break;
//            }
//        };
//        let mut all_valid = true;
//        for p in vert_perm {
//            if p != D { all_valid = false; break; }
//        }
//        if all_valid { break graph }
    };
    println!("{}", graph.to_string_directed(GraphFormat::Dot));
}
