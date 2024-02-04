use crate::mca_dbus::MCADBUSProxy;
use crate::mca_dbus::DBUS_HANDLER;

use std::ffi::{c_void, CStr, CString};
use std::os::raw::c_char;
use std::io::{stdout, Write};
#[repr(C)]
pub struct MCA {
    pub sender: *const c_char,
    pub is_available: bool,
    pub id: i64,
}


pub trait MCAOperations {
    fn create() -> MCA;
    async fn set_sender(&mut self, name: *const c_char);
    fn get_sender(&mut self) -> *const c_char;
    fn check_state(&mut self) -> bool;
    fn set_id(&mut self, id: i64);
    fn get_id(&mut self) -> i64;
}

impl MCAOperations for MCA {
    fn create() -> MCA {
        let mca = MCA {
            sender: CString::new("test").unwrap().as_ptr(),
            is_available: false,
            id : 100,
        };

        mca

    }

    async fn set_sender(&mut self, name: *const c_char) {
        self.sender = name;
        let mut lock = stdout().lock();
        writeln!(lock, "{name:?}").unwrap();
        let dbus_handler = DBUS_HANDLER.lock().unwrap();
        let ret = dbus_handler.send_string("testtest").await;
        let mut lock = stdout().lock();
        writeln!(lock, "{ret:?}").unwrap();

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