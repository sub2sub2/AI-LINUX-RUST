use std::sync::atomic::{AtomicUsize, Ordering};



static COUNTER: AtomicUsize = AtomicUsize::new(1);
pub fn get_id() -> usize { COUNTER.fetch_add(1, Ordering::Relaxed) }
