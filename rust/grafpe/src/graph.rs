use crate::irs::{ IrsCtx, Irs };
use crate::perm::{self, inverse};
use std::any::Any;

pub mod srw;
pub mod convertible;
pub mod to_perm;
mod random_walks;
mod tree_policy;
mod walker;

pub use self::walker::Walker;
pub use self::to_perm::{to_perm_directed, from_perm_directed};

/// Trait defining a Graph that can be traversed by `Walkers` and used for encryption
/// in `Grafpe`.
pub trait Graph {
    /// Obtain the number of vertices in the graph.
    fn size(&self) -> usize;
    /// Obtain the degree of the graph.
    fn degree(&self) -> usize;
    /// Obtain a reference to the permutations used internally in the graph.
    fn permutations(&self) -> &[Vec<u64>];
    /// Obtain a reference to permutations pointing *forward*.
    fn forward_permutations(&self) -> &[Vec<u64>];
    /// Obtain a reference to permutations pointing *backward*.
    fn backward_permutations(&self) -> &[Vec<u64>];

    fn as_any(&self) -> &dyn Any;
}

pub trait SecureDirected {
    fn directed_walk_length(&self, sec_param: usize) -> usize;
}
pub trait SecureUndirected {
    fn undirected_walk_length(&self, sec_param: usize) -> usize;
}

/// A graph generated in such a way that it conforms requirements
/// stated by the Sly's and Lubetzky's theorem. A secure length of
/// a random walk over such a graph can be estimated using a
/// constant-time approximation.
///
/// For reference see:
///
/// `Lubetzky, E., & Sly, A. (2010).
/// Cutoff phenomena for random walks on random regular graphs.
/// Duke Mathematical Journal, 153(3), 475-510.`
///
#[derive(Debug, Eq, PartialEq)]
pub struct ValidGraph {
    n: usize,
    d: usize,
    permutations: Vec<Vec<u64>>,
}

impl SecureDirected for ValidGraph {
    fn directed_walk_length(&self, sec_param: usize) -> usize {
        random_walks::graph_exp_tvd(self.forward_permutations(), sec_param)
            .max(sec_param / self.forward_permutations().len())
    }
}

impl SecureUndirected for ValidGraph {
    fn undirected_walk_length(&self, sec_param: usize) -> usize {
        random_walks::graph_exp_tvd(self.permutations(), sec_param)
            .max(sec_param / self.permutations().len())
    }
}

#[derive(Clone)]
pub enum ConstructionMethod {
    Naive,
    Tree,
}

impl ValidGraph {
    pub fn new(method: ConstructionMethod, n: usize, d: usize, irs: &mut Irs) -> ValidGraph {
        match method {
            ConstructionMethod::Naive => ValidGraph::new_naive(n, d, irs),
            ConstructionMethod::Tree => ValidGraph::new_tree(n, d, irs),
        }
    }

    pub fn from_permutations(n: usize, d: usize, permutations: Vec<Vec<u64>>) -> ValidGraph {
        ValidGraph { n, d, permutations }
    }

    fn new_naive(n: usize, d: usize, irs: &mut Irs) -> ValidGraph {
        let mut permutations: Vec<Vec<u64>> = Vec::with_capacity(d);

        let base_permutation = perm::new(n);
        let mut perm = base_permutation.clone();
        let ctx = IrsCtx::new(&perm);

        let mut ctx = perm::valid::permute(&mut perm, irs, ctx);

        permutations.push(perm);

        for _ in 1..d / 2 {
            let mut perm = base_permutation.clone();
            ctx.clear_slots();
            ctx = perm::valid::permute(&mut perm, irs, ctx);

            while !perm::valid::is_new_valid(&permutations, &perm) {
                ctx = perm::valid::repermute(&mut perm, irs, ctx);
            }

            permutations.push(perm);
        }

        for i in 0..d / 2 {
            permutations.push(inverse(&permutations[i]));
        }

        ValidGraph { n, d, permutations }
    }

    fn new_tree(n: usize, d: usize, irs: &mut Irs) -> ValidGraph {
        tree_policy::new_graph(n, d, irs)
    }
}

pub fn is_valid<G: Graph>(graph: &G) -> bool {
    let perms = graph.permutations();
    for i in 0..perms.len() / 2 {
        if !perm::valid::is_valid(&perms[i])
          || !perm::valid::is_new_valid(&perms[..i], &perms[i])
        {
            return false;
        }
    }
    true
}

impl Graph for ValidGraph {
    fn size(&self) -> usize {
        self.n
    }

    fn degree(&self) -> usize {
        self.d
    }

    fn permutations(&self) -> &[Vec<u64>] {
        &self.permutations
    }

    fn forward_permutations(&self) -> &[Vec<u64>] {
        &self.permutations[..self.d/2]
    }

    fn backward_permutations(&self) -> &[Vec<u64>] {
        &self.permutations[self.d/2..]
    }

    fn as_any(&self) -> &Any {
        self
    }
}


#[derive(Debug)]
pub struct NewGraph {
    pub n: usize,
    pub d: usize,
    pub permutations: Vec<Vec<u64>>
}

impl SecureDirected for NewGraph {
    fn directed_walk_length(&self, sec_param: usize) -> usize {
//        random_walks::graph_exp_tvd(&self.forward_permutations(), sec_param)
        // cant use graph_exp_tvd as it requires the graph to be valid
        // TODO add a trait for valid graphs (?)
        unimplemented!()
    }
}

impl NewGraph {
    pub fn new(n: usize, d: usize, irs: &mut Irs) -> NewGraph {
        let mut permutations: Vec<Vec<u64>> = Vec::with_capacity(d * 2);
        let base_permutation = perm::new(n);
        let mut ctx = IrsCtx::new(&base_permutation);

        for _ in 0..d {
            let mut perm = base_permutation.clone();
            ctx.clear_slots();
            ctx = perm::valid::permute(&mut perm, irs, ctx);
            permutations.push(perm);
        }

        for i in 0..d {
            permutations.push(inverse(&permutations[i]));
        }

        NewGraph { n, d, permutations }
    }
}

impl Graph for NewGraph {
    fn size(&self) -> usize {
        self.n
    }

    fn degree(&self) -> usize {
        self.d
    }

    fn permutations(&self) -> &[Vec<u64>] {
        &self.permutations
    }

    fn forward_permutations(&self) -> &[Vec<u64>] {
        &self.permutations[..self.d]
    }

    fn backward_permutations(&self) -> &[Vec<u64>] {
        &self.permutations[self.d..]
    }

    fn as_any(&self) -> &Any {
        self
    }
}
