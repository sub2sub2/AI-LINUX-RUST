// rust_library/src/lib.rs

use std::ffi::{c_void, CStr, CString};
use std::os::raw::c_char;

#[repr(C)]
pub struct MCA {
    pub sender: *const c_char,
    pub is_available: bool,
    pub id : i64,
}


pub trait MCAOperations {
    fn create() -> Self;
    fn set_sender(&mut self, name: *const c_char);
    fn get_sender(&mut self) -> *const c_char;
    fn check_state(&mut self) -> bool;
    fn set_id(&mut self, id: i64);
    fn get_id(&mut self) -> i64;
}

impl MCAOperations for MCA {
    fn create() -> Self {
        MCA {
            sender: CString::new("test").unwrap().as_ptr(),
            is_available: false,
            id : 100
        }
    }

    fn set_sender(&mut self, name: *const c_char) {
        self.sender = name;
    }

    fn get_sender(&mut self) -> *const c_char {
        self.sender.clone()
    }

    fn check_state(&mut self) -> bool {
        self.is_available
    }

    fn set_id(&mut self, id: i64) {
        self.id = id;
    }

    fn get_id(&mut self) -> i64 {
        self.id
    }
}

#[no_mangle]
pub extern "C" fn mca_handle_create() -> MCA{
    let point = MCA::create();
    return point;
}

#[no_mangle]
pub extern "C" fn mca_set_sender(handler: *mut MCA, name: *const c_char) {
    unsafe {
        if !handler.is_null() {
            let mca = &mut *handler;
            mca.set_sender(name);
        }
    }
}
#[no_mangle]
pub extern "C" fn mca_get_sender(handler: *mut MCA) -> *const c_char {
    if !handler.is_null() {
        let mca = unsafe { &mut *handler };
        mca.get_sender()
    } else {
        std::ptr::null()
    }
}

#[no_mangle]
pub extern "C" fn mca_check_state(handler: *mut MCA) -> bool {
    if !handler.is_null() {
        let mca = unsafe { &mut *handler };
        mca.check_state()
    } else {
        false
    }
}

#[no_mangle]
pub extern "C" fn mca_get_id(handler: *mut MCA) -> i64 {
    if !handler.is_null() {
        let mca = unsafe { &mut *handler };
        mca.get_id()
    } else {
        -1
    }
}

#[no_mangle]
pub extern "C" fn mca_set_id(handler: *mut MCA, id: i64) {
    unsafe {
        if !handler.is_null() {
            let mca = &mut *handler;
            mca.set_id(id);
        }
    }
}
