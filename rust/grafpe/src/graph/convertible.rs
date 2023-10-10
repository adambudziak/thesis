use crate::graph::{ValidGraph, NewGraph};

pub enum GraphFormat {
    Dot,
    Perm(usize),
}

pub trait Convertible {
    fn to_string_directed(&self, f: GraphFormat) -> String;
    fn to_string_undirected(&self, f: GraphFormat) -> String;
}

impl Convertible for ValidGraph {
    fn to_string_directed(&self, f: GraphFormat) -> String {
        match f {
            GraphFormat::Dot => dot::perms_to_directed(&self.permutations),
            GraphFormat::Perm(walk_length) => perm::perms_to_directed(&self.permutations, walk_length, self.n, self.d)
        }
    }

    fn to_string_undirected(&self, f: GraphFormat) -> String {
        match f {
            GraphFormat::Dot => dot::perms_to_undirected(&self.permutations),
            GraphFormat::Perm(walk_length) => perm::perms_to_undirected(&self.permutations, walk_length, self.n, self.d)
        }
    }
}

impl Convertible for NewGraph {
    fn to_string_directed(&self, f: GraphFormat) -> String {
        match f {
            GraphFormat::Dot => dot::perms_to_directed(&self.permutations),
            GraphFormat::Perm(walk_length) => perm::perms_to_directed(&self.permutations, walk_length, self.n, self.d)
        }
    }

    fn to_string_undirected(&self, f: GraphFormat) -> String {
        match f {
            GraphFormat::Dot => dot::perms_to_undirected(&self.permutations),
            GraphFormat::Perm(walk_length) => perm::perms_to_undirected(&self.permutations, walk_length, self.n, self.d)
        }
    }
}

pub mod dot {
    use std::string::ToString;
    use crate::graph::ValidGraph;
    use std::fs::File;
    use std::io::{BufReader, BufRead};
    use regex::Regex;
    use crate::perm::inverse;
    use std::error::Error;

    pub fn perms_to_directed(perms: &[Vec<u64>]) -> String {
        "digraph {\n".to_string()
        + &perms[..perms.len()/2].iter().flat_map(|p| p.iter().enumerate())
            .fold(String::new(), |s, (i, v)| s + &format!("\t{} -> {};\n", i, v))
        + "}\n"
    }

    pub fn perms_to_undirected(perms: &[Vec<u64>]) -> String {
        "graph {\n".to_string()
        + &perms[..perms.len()/2].iter().flat_map(|p| p.iter().enumerate())
            .fold(String::new(), |s, (i, v)| s + &format!("\t{} -- {};\n", i, v))
        + "}\n"
    }

    pub fn perms_from_undirected(filename: &str) -> Result<ValidGraph, Box<dyn Error>> {
        let mut reader = BufReader::new(File::open(filename)?);
        let mut graph_type = String::new();
        reader.read_line(&mut graph_type)?;
        if graph_type.contains("digraph") {
            return Err(Box::new(std::io::Error::new(std::io::ErrorKind::InvalidData, "File contains a digraph instead of a graph.")));
        }

        let edge_regex = Regex::new(r"\s*(\d+)\s*--\s*(\d+);?").unwrap();

        let mut edges: Vec<(u64, u64)> = vec![];
        let mut n = 0;

        for line in reader.lines() {
            let line = line?;
            for cap in edge_regex.captures_iter(&line) {
                let v_from = cap[1].parse::<u64>()?;
                let v_to = cap[2].parse::<u64>()?;
                n = n.max(v_from).max(v_to);
                edges.push((v_from, v_to));
            }
        }

        let n = n as usize + 1;
        let d = edges.len() / n * 2;

        let mut permutations: Vec<Vec<u64>> = vec![vec![0; n]; d];
        for (i, (from, to)) in edges.into_iter().enumerate() {
            permutations[i / n][from as usize] = to;
            permutations[i / n + d/2][to as usize] = from;
        }

        Ok(ValidGraph::from_permutations(n, d, permutations))
    }
}

mod perm {
    use itertools::join;

    pub fn perms_to_directed(perms: &[Vec<u64>], walk_length: usize, n: usize, d: usize) -> String {
    String::from(format!("S\t{}\n", walk_length))
        + &format!("N\t{}\tD\t{}\n", n, d)
        + &join((&perms[..perms.len()/2]).iter()
            .map(|p| join(p, "\t")), "\n")
    }

    pub fn perms_to_undirected(perms: &[Vec<u64>], walk_length: usize, n: usize, d:usize) -> String {
        String::from(format!("S\t{}\n", walk_length))
            + &format!("N\t{}\tD\t{}\n", n, d)
            + &join(perms.iter()
                .map(|p| join(p, "\t")), "\n")
    }
}

#[cfg(test)]
mod test {
    use crate::graph::ValidGraph;
    use crate::perm::inverse;
    use crate::graph::convertible::Convertible;
    use super::{GraphFormat, dot};
    use std::io::{BufReader, Write};
    use std::fs::File;

    #[test]
    fn test_dot_from_to() {
        let perm_a = vec![7, 0, 6, 1, 5, 3, 4, 2];
        let perm_b = vec![5, 6, 1, 4, 7, 2, 0, 3];
        let pa_inv = inverse(&perm_a);
        let pb_inv = inverse(&perm_b);
        let permutations = vec![
            perm_a, perm_b, pa_inv, pb_inv
        ];
        let graph = ValidGraph::from_permutations(8, 4, permutations);
        let graph_string = graph.to_string_undirected(GraphFormat::Dot);
        let mut file = File::create("test.txt").unwrap();
        file.write_all(graph_string.as_bytes());
        let graph_from_dot = dot::perms_from_undirected("test.txt").unwrap();
        assert_eq!(graph_from_dot, graph);
    }
}


