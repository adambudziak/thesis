use crate::graph::Walker;
use crate::prng::Prng;
use openssl::symm::Cipher;
use std::mem::transmute;
use std::ptr::copy_nonoverlapping as copy_no;

fn generate_iv_gcm(tweak: &[u8], data: &[u32], index: u32, tag: u8, iv: &mut [u8]) {
    assert!(iv.len() >= tweak.len() + data.len() + 1);

    iv[..tweak.len()].clone_from_slice(tweak);
    {
        let t_size = (data.len() - 1) * 4;
        let t_index = index as isize * 4;
        let tweak_len = tweak.len() as isize;
        unsafe {
            let data_ptr = transmute::<*const u32, *const u8>(data.as_ptr());
            let tag_ptr = transmute::<*const u32, *const u8>(&index);
            copy_no(data_ptr,
                    iv.as_mut_ptr().offset(tweak_len),
                    t_index as usize);
            copy_no(data_ptr.offset(t_index + 4),
                    iv.as_mut_ptr().offset(t_index + tweak_len),
                    t_size - t_index as usize);
            copy_no(tag_ptr,
                    iv.as_mut_ptr().offset(t_size as isize + tweak_len),
                    4);
        }
    }
    if let Some(last) = iv.last_mut() {
        *last = tag;
    }
}

pub struct Grafpe<W: Walker> {
    walker: W,
    key: Vec<u8>,
    walk_length: usize,
}

impl<W: Walker> Grafpe<W> {
    pub fn new(walker: W, key: &[u8], walk_length: usize) -> Grafpe<W> {
        Grafpe {
            walker,
            key: Vec::from(key),
            walk_length,
        }
    }

    #[inline]
    fn new_context(&self, msg: &[u32], tweak: &[u8]) -> (Vec<u32>, Vec<u8>, Vec<u8>)
    {
        (
            Vec::from(msg),
            vec![0_u8; tweak.len() + msg.len() * 4 + 1],
            vec![0_u8; self.walk_length * self.walker.bits_per_step() / 8 + 1]
        )
    }

    pub fn encrypt(&self, msg: &[u32], tweak: &[u8]) -> Vec<u32> {
        if msg.is_empty() {
            return Vec::new();
        }

        let (mut msg, mut iv, mut random) = self.new_context(msg, tweak);

        let mut encrypt_ = |tag: u8| {
            for i in 0..msg.len() {
                generate_iv_gcm(tweak, &msg, i as u32, tag, &mut iv);
                Prng::from_cipher(Cipher::aes_128_gcm(), &self.key, &iv)
                    .feed_bytes(&mut random);
                msg[i] = self.walker.go(msg[i], self.walk_length, &random);
            }
        };
        encrypt_(b'a');
        encrypt_(b's');
        msg
    }

    pub fn decrypt(&self, crypt: &[u32], tweak: &[u8]) -> Vec<u32> {
        if crypt.is_empty() {
            return Vec::new();
        }

        let (mut msg, mut iv, mut random) = self.new_context(crypt, tweak);

        let mut decrypt_ = |tag: u8| {
            for i in (0..msg.len()).rev() {
                generate_iv_gcm(tweak, &msg, i as u32, tag, &mut iv);
                Prng::from_cipher(Cipher::aes_128_gcm(), &self.key, &iv)
                    .feed_bytes(&mut random);
                msg[i] = self.walker.go_back(msg[i], self.walk_length, &random);
            }
        };
        decrypt_(b's');
        decrypt_(b'a');
        msg
    }
}

#[cfg(test)]
mod test {
    use super::*;
    use crate::graph::srw::UndirectedSrw;
    use crate::graph::{ConstructionMethod, ValidGraph};
    use crate::irs::Irs;
    use crate::prng::Prng;
    use openssl::symm::Cipher;
    

    #[test]
    fn test_encrypt_decrypt() {
        let key = hex::decode("01230123012301230123012301230123").unwrap();
        let iv = hex::decode("00001111222233330000111122223333").unwrap();

        let mut irs = Irs::new(Prng::from_cipher(Cipher::aes_128_ctr(), &key, &iv));
        let graph = ValidGraph::new(ConstructionMethod::Naive, 256, 4, &mut irs);

        let grafpe = Grafpe::new(UndirectedSrw::new(&graph), &key, 60);

        let plaintext = "Hello, world!";
        let plaintext: Vec<u32> = plaintext.as_bytes().into_iter().map(|c| {
            *c as u32
        }).collect();
        let cipher = grafpe.encrypt(&plaintext, &[]);
        let decrypted = grafpe.decrypt(&cipher, &[]);
        let decrypted = String::from_utf8(decrypted.into_iter().map(|c| c as u8).collect())
            .unwrap();

        

        assert_eq!("Hello, world!", decrypted);
    }
}
