mod mca_operation;
mod mca_dbus;
use std::io::{stdout, Write};
use mca_operation::{MCA, MCAOperations};
use std::ffi::{c_void, CStr, CString};
use std::os::raw::c_char;
use std::future::Future;

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
            let mut lock = stdout().lock();
            writeln!(lock, "set_sender").unwrap();
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
