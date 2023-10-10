use crate::crypt::Grafpe;
use crate::graph::Walker;
use std::str::FromStr;


pub fn luhn(purported: u64) -> bool {
    assert!(purported < 10_u64.pow(16));
    let mut purported = purported;

    let mut sum = 0;
    let mut even = 0;
    while purported != 0 {
        let doubled = (purported % 10) * (even + 1);
        sum += if doubled >= 10 { doubled - 9 } else { doubled };
        purported /= 10;
        even = 1 - even;
    }
    sum % 10 == 0
}



/// Credit card number represented as four four-digit numbers.
/// The encryption is performed on a graph with 10,000 vertices
/// and it can be parameterized to perform additional checks
/// (e.g. Luhn checksum validity).
#[derive(Debug, Eq, PartialEq)]
pub struct Digit16Ccn {
    ccn: u64,
}

impl Digit16Ccn {
    pub fn new(ccn: u64) -> Digit16Ccn { Digit16Ccn { ccn } }

    fn ccn_to_quadruples(ccn: u64) -> Vec<u32> {
        (0..4).rev().map(|i| (ccn / 10_u64.pow(i as u32 * 4) % 10_000) as u32).collect()
    }

    fn quadruples_to_ccn(quadruples: &[u32]) -> u64 {
        quadruples.iter().fold(0, |s, c| s * 10_000 + *c as u64)
    }

    pub fn encrypt<W: Walker>(&self, grafpe: &Grafpe<W>) -> Self {
        let quadruples = Digit16Ccn::ccn_to_quadruples(self.ccn);
        let quad_vec = grafpe.encrypt(&quadruples[..], &[]);
        Digit16Ccn { ccn: Digit16Ccn::quadruples_to_ccn(&quad_vec) }
    }

    pub fn decrypt<W: Walker>(&self, grafpe: &Grafpe<W>) -> Self {
        let quadruples = Digit16Ccn::ccn_to_quadruples(self.ccn);
        let quad_vec = grafpe.decrypt(&quadruples[..], &[]);
        Digit16Ccn { ccn: Digit16Ccn::quadruples_to_ccn(&quad_vec) }
    }

    pub fn encrypt_with_luhn<W: Walker>(&self, grafpe: &Grafpe<W>) -> Self {
        let quadruples = Digit16Ccn::ccn_to_quadruples(self.ccn);
        let mut encrypted = grafpe.encrypt(&quadruples[..], &[]);
        while !luhn(Digit16Ccn::quadruples_to_ccn(&encrypted)) {
            encrypted = grafpe.encrypt(&encrypted[..], &[]) ;
        }
        Digit16Ccn { ccn: Digit16Ccn::quadruples_to_ccn(&encrypted) }
    }

    pub fn decrypt_with_luhn<W: Walker>(&self, grafpe: &Grafpe<W>) -> Self {
        let quadruples = Digit16Ccn::ccn_to_quadruples(self.ccn);
        let mut decrypted = grafpe.decrypt(&quadruples[..], &[]);
        while !luhn(Digit16Ccn::quadruples_to_ccn(&decrypted)) {
            decrypted = grafpe.decrypt(&decrypted[..], &[]) ;
        }
        Digit16Ccn { ccn: Digit16Ccn::quadruples_to_ccn(&decrypted) }
    }

    pub fn ccn(&self) -> u64 {
        self.ccn
    }
}

impl FromStr for Digit16Ccn {
    type Err = ();

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if s.len() != 16 || !s.chars().into_iter().all(|c| c.is_numeric()) {
            return Err(());
        }
        let ccn = s.parse::<u64>().unwrap();
        Ok(Digit16Ccn{ ccn })
    }
}

impl ToString for Digit16Ccn {
    fn to_string(&self) -> String {
        format!("{:0>16}", self.ccn)
    }
}


#[cfg(test)]
mod test {
    use crate::fpe::ccn::{Digit16Ccn, luhn};
    use std::str::FromStr;
    use std::string::ToString;
    use crate::irs::Irs;
    use crate::prng::Prng;
    use openssl::symm::Cipher;
    use crate::graph::{ValidGraph, ConstructionMethod};
    use crate::crypt::Grafpe;
    use crate::graph::srw::UndirectedSrw;

    #[test]
    fn test_from_str_success() {
        let ccn = Digit16Ccn::from_str("0123456789876543");
        assert!(ccn.is_ok());
        let ccn = ccn.unwrap();
        assert_eq!(ccn, Digit16Ccn { ccn: 123456789876543});
    }

    #[test]
    fn test_to_str() {
        let num = "0123456789876543";
        assert_eq!(num, &Digit16Ccn::from_str(num).unwrap().to_string());
    }

    #[test]
    fn test_encrypt_decrypt() {
        let key = hex::decode("01230123012301230123012301230123").unwrap();
        let iv = hex::decode("00001111222233330000111122223333").unwrap();

        let mut irs = Irs::new(Prng::from_cipher(Cipher::aes_128_ctr(), &key, &iv));
        let graph = ValidGraph::new(ConstructionMethod::Naive, 10000, 4, &mut irs);

        let grafpe = Grafpe::new(UndirectedSrw::new(&graph), &key, 600);

        let ccn = Digit16Ccn::from_str("1234567890123456").unwrap();
        assert_eq!(ccn, ccn.encrypt(&grafpe).decrypt(&grafpe));
    }

    #[test]
    fn test_encrypt_decrypt_with_luhn() {
        let key = hex::decode("01230123012301230123012301230123").unwrap();
        let iv = hex::decode("00001111222233330000111122223333").unwrap();

        let mut irs = Irs::new(Prng::from_cipher(Cipher::aes_128_ctr(), &key, &iv));
        let graph = ValidGraph::new(ConstructionMethod::Naive, 10000, 4, &mut irs);

        let grafpe = Grafpe::new(UndirectedSrw::new(&graph), &key, 600);

        for i in 0..1000 {
            let i = i as u64 + 10_u64.pow(15);
            if !luhn(i) { continue; }
            let encrypted = Digit16Ccn::new(i).encrypt_with_luhn(&grafpe);
            assert_ne!(i, encrypted.ccn);
            assert_eq!(i, encrypted.decrypt_with_luhn(&grafpe).ccn);
        }
    }
}
