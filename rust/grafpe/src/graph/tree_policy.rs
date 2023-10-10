use crate::graph::ValidGraph;
use crate::irs;
use crate::perm::{self, valid, inverse};
use std::cell::RefCell;
use std::rc;

const MAX_CHILDREN_CNT: usize = 10;

#[derive(Debug, Clone)]
struct PermutationNode {
    children: Vec<PermutationNode>,
    permutation: rc::Rc<RefCell<Vec<u64>>>,
    longest_path: usize,
}

fn permute(perm: &mut Vec<u64>, irs: &mut irs::Irs, ctx: irs::IrsCtx) -> irs::IrsCtx {
    let mut perm = perm;
    let mut ctx = ctx;
    ctx.clear_slots();
    valid::permute(&mut perm, irs, ctx)
}

pub fn new_graph(n: usize, d: usize, irs: &mut irs::Irs) -> ValidGraph {
    let mut perm = perm::new(n);
    let mut ctx = irs::IrsCtx::new(&perm);

    ctx = permute(&mut perm, irs, ctx);

    let mut root = PermutationNode {
        children: Vec::new(),
        permutation: rc::Rc::new(RefCell::new(perm)),
        longest_path: 1,
    };

    fn push_where_valid(current: &mut PermutationNode, node: &PermutationNode) -> bool {
        if !valid::is_pair_valid(&current.permutation.borrow(), &node.permutation.borrow()) {
            return false;
        }

        let mut added_to_child = false;

        for child in &mut current.children {
            added_to_child = added_to_child || push_where_valid(child, node);
            if current.longest_path < child.longest_path + 1 {
                current.longest_path = child.longest_path + 1;
            }
        }
        if current.children.is_empty() {
            current.longest_path += 1;
        }
        if current.children.len() < MAX_CHILDREN_CNT {
            current.children.push(node.clone());
        }

        true
    }

    fn get_the_longest_path(node: PermutationNode, d: usize) -> Vec<Vec<u64>> {
        let mut permutations: Vec<Vec<u64>> = Vec::with_capacity(d);

        fn recursively_add_permutations(
            node_: PermutationNode,
            permutations: &mut Vec<Vec<u64>>,
            depth: usize,
        ) {
            let mut node_ = node_;

            let perm = RefCell::new(Vec::new());

            node_.permutation.swap(&perm);

            let perm = perm.into_inner();

            permutations.push(perm);
            if node_.children.is_empty() {
                return;
            }

            let mut child_ = node_.children.pop().unwrap();
            while !node_.children.is_empty() && child_.longest_path < depth {
                let child = node_.children.pop().unwrap();
                if child.longest_path > child_.longest_path {
                    child_ = child;
                }
            }

            recursively_add_permutations(child_, permutations, depth - 1);
        }

        recursively_add_permutations(node, &mut permutations, d / 2);

        permutations
    }

    while root.longest_path < d / 2 {
        let mut perm = perm::new(n);
        ctx = permute(&mut perm, irs, ctx);

        let node = PermutationNode {
            children: Vec::new(),
            permutation: rc::Rc::new(RefCell::new(perm)),
            longest_path: 1,
        };

        while !push_where_valid(&mut root, &node) {
            let mut permutation = node.permutation.borrow_mut();
            ctx = valid::repermute(&mut permutation, irs, ctx);
        }
    }

    let mut permutations = get_the_longest_path(root, d);

    for i in 0..d/2 {
        permutations.push(inverse(&permutations[i]));
    }

    ValidGraph {
        n,
        d,
        permutations
    }
}
