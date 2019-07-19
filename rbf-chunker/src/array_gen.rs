use std::fmt::{self, Debug};
use std::ops::{Deref, DerefMut};

pub struct ArrayGen<T, const N: usize>([T; N]);

// With some modification, ripped from
// https://doc.rust-lang.org/nightly/src/core/array.rs.html#374-381
impl<T: Debug, const N: usize> Debug for ArrayGen<T, { N }> where {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        Debug::fmt(&&self[..], f)
    }
}

impl<T, const N: usize> From<[T; N]> for ArrayGen<T, { N }> {
    fn from(a: [T; N]) -> Self {
        ArrayGen(a)
    }
}

impl<T, const N: usize> Deref for ArrayGen<T, { N }> {
    type Target = [T; N];

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

impl<T, const N: usize> DerefMut for ArrayGen<T, { N }> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.0
    }
}
