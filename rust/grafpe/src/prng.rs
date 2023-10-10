use openssl::symm::{Cipher, Crypter, Mode};

const DUMMY_PLAINTEXT: [u8; 16] = [0; 16];

pub struct Prng {
    buffer: Vec<u8>,
    index: usize,
    crypter: Crypter,
}

impl Prng {
    const BLOCK_BYTES_COUNT: usize = 16; // for some reason it's not easy to get it out of a crypter

    fn get_dummy_buffer() -> Vec<u8> {
        vec![0; 1 + Prng::BLOCK_BYTES_COUNT]
    }

    pub fn new(crypter: Crypter) -> Prng {
        Prng {
            buffer: Prng::get_dummy_buffer(),
            index: 8 * Self::BLOCK_BYTES_COUNT,
            crypter,
        }
    }

    pub fn from_cipher(cipher: Cipher, key: &[u8], iv: &[u8]) -> Prng {
        let crypter = Crypter::new(cipher, Mode::Encrypt, key, Some(iv)).unwrap();
        Prng::new(crypter)
    }

    pub fn fetch_bit(&mut self) -> bool {
        if self.index == 8 * Self::BLOCK_BYTES_COUNT {
            self.crypter
                .update(&DUMMY_PLAINTEXT, &mut self.buffer)
                .unwrap();
            self.index = 0;
        }
        let result = self.buffer[self.index / 8] & (1u8 << (self.index % 8)) != 0;
        self.index += 1;
        result
    }

    pub fn feed_bytes(&mut self, buffer: &mut [u8]) {
        let mut copied = 0_usize;
        while copied + Prng::BLOCK_BYTES_COUNT < buffer.len() {
            self.crypter.update(
                &DUMMY_PLAINTEXT,
                &mut buffer[copied..=copied + Prng::BLOCK_BYTES_COUNT],
            ).unwrap();
            copied += Prng::BLOCK_BYTES_COUNT;
        }
        let mut tail = Prng::get_dummy_buffer();
        self.crypter.update(&DUMMY_PLAINTEXT, &mut tail)
            .unwrap();
        let len = buffer.len();
        buffer[copied..].clone_from_slice(&tail[..len - copied]);
    }
}

#[cfg(test)]
mod test {
    use super::*;
    use openssl::symm::{Mode};

    #[test]
    fn fetch_bit() {
        // checks that the results from the prng generate the same output as its internal
        // block cipher

        let key = hex::decode("01230123012301230123012301230123").unwrap();
        let iv = hex::decode("00001111222233330000111122223333").unwrap();

        let cipher = Cipher::aes_128_ctr();
        let mut ciphertext: Vec<u8> = vec![0; DUMMY_PLAINTEXT.len() + cipher.block_size()];

        let mut crypter = Crypter::new(cipher, Mode::Encrypt, &key, Some(&iv)).unwrap();

        let mut prng =
            Prng::new(Crypter::new(Cipher::aes_128_ctr(), Mode::Encrypt, &key, Some(&iv)).unwrap());

        for _ in 0..10 {
            let mut prng_buffer: Vec<u8> = vec![0; DUMMY_PLAINTEXT.len()];

            crypter.update(&DUMMY_PLAINTEXT, &mut ciphertext).unwrap();

            for i in 0..prng_buffer.len() * 8 {
                prng_buffer[i / 8] |= (prng.fetch_bit() as u8) << (i % 8);
            }

            assert_eq!(prng_buffer[..], ciphertext[..prng_buffer.len()]);
        }
    }

    #[test]
    fn test_fetch_bit_eq_feed_bytes() {
        let key = hex::decode("01230123012301230123012301230123").unwrap();
        let iv = hex::decode("00001111222233330000111122223333").unwrap();

        let mut prng =
            Prng::new(Crypter::new(Cipher::aes_128_ctr(), Mode::Encrypt, &key, Some(&iv)).unwrap());

        let mut prng2 =
            Prng::new(Crypter::new(Cipher::aes_128_ctr(), Mode::Encrypt, &key, Some(&iv)).unwrap());

        let mut buffer = vec![0 as u8; 1015];
        prng.feed_bytes(&mut buffer);

        let mut buffer2 = vec![0 as u8; 1015];

        for i in 0..buffer2.len() * 8 {
            buffer2[i / 8] |= (prng2.fetch_bit() as u8) << (i % 8);
        }

        assert_eq!(buffer, buffer2);
    }
}
