//! This horrid monstrosity provides a 5% speed up.
//! Copied from the stdlib, then modified ever so slightly.
//!
//! Presumably, rustc fails to inline/optimize the stdlib, so moving it here
//! makes stuff faster... somehow?

use std::io::prelude::*;
use std::mem::MaybeUninit;
use std::ops::{Div, Rem, Sub};

trait Int: PartialEq + PartialOrd + Div<Output=Self> + Rem<Output=Self> +
           Sub<Output=Self> + Copy {
    fn to_u64(&self) -> u64;
}

macro_rules! doit {
    ($($t:ident)*) => ($(impl Int for $t {
        #[inline]
        fn to_u64(&self) -> u64 { *self as u64 }
    })*)
}
doit! { i8 i16 i32 i64 i128 isize u8 u16 u32 u64 u128 usize }

pub trait WriteInt {
    fn write_int<T: Write>(n: Self, w: T) -> std::io::Result<()>;
}

static DEC_DIGITS_LUT: &[u8; 200] =
    b"0001020304050607080910111213141516171819\
      2021222324252627282930313233343536373839\
      4041424344454647484950515253545556575859\
      6061626364656667686970717273747576777879\
      8081828384858687888990919293949596979899";

macro_rules! impl_WriteInt {
    ($($t:ident),* as $u:ident via $conv_fn:ident named $name:ident) => {
        #[inline]
        fn $name<T: Write>(mut n: $u, is_nonnegative: bool, mut w: T) -> std::io::Result<()> {
            let mut buf = uninitialized_array![u8; 40];
            let mut curr = buf.len() as isize;
            let buf_ptr = MaybeUninit::first_ptr_mut(&mut buf);
            let lut_ptr = DEC_DIGITS_LUT.as_ptr();

            unsafe {
                // need at least 16 bits for the 4-characters-at-a-time to work.
                assert!(std::mem::size_of::<$u>() >= 2);

                // eagerly decode 4 characters at a time
                while n >= 10000 {
                    let rem = (n % 10000) as isize;
                    n /= 10000;

                    let d1 = (rem / 100) << 1;
                    let d2 = (rem % 100) << 1;
                    curr -= 4;
                    std::ptr::copy_nonoverlapping(lut_ptr.offset(d1), buf_ptr.offset(curr), 2);
                    std::ptr::copy_nonoverlapping(lut_ptr.offset(d2), buf_ptr.offset(curr + 2), 2);
                }

                // if we reach here numbers are <= 9999, so at most 4 chars long
                let mut n = n as isize; // possibly reduce 64bit math

                // decode 2 more chars, if > 2 chars
                if n >= 100 {
                    let d1 = (n % 100) << 1;
                    n /= 100;
                    curr -= 2;
                    std::ptr::copy_nonoverlapping(lut_ptr.offset(d1), buf_ptr.offset(curr), 2);
                }

                // decode last 1 or 2 chars
                if n < 10 {
                    curr -= 1;
                    *buf_ptr.offset(curr) = (n as u8) + b'0';
                } else {
                    let d1 = n << 1;
                    curr -= 2;
                    std::ptr::copy_nonoverlapping(lut_ptr.offset(d1), buf_ptr.offset(curr), 2);
                }
            }

            if !is_nonnegative {
                curr -= 1;
                unsafe {
                    *buf_ptr.offset(curr) = b'-';
                }
            }

            let buf_slice = unsafe {
                    std::slice::from_raw_parts(buf_ptr.offset(curr), buf.len() - curr as usize)
            };
            w.write_all(buf_slice)
        }

        $(
            impl WriteInt for $t {
                #[allow(unused_comparisons)]
                #[inline]
                fn write_int<T: Write>(n: $t, w: T) -> std::io::Result<()> {
                    let is_nonnegative = n >= 0;
                    let n = if is_nonnegative {
                        n.$conv_fn()
                    } else {
                        // convert the negative num to positive by summing 1 to it's 2 complement
                        (!n.$conv_fn()).wrapping_add(1)
                    };
                    $name(n, is_nonnegative, w)
                }
            }
        )*
    };
}

#[cfg(any(target_pointer_width = "64"))]
mod imp {
    use super::*;
    impl_WriteInt!(
        i8, i16, i32, i64, isize, u8, u16, u32, u64, usize
            as u64 via to_u64 named fmt_u64
    );
}

