use std::{mem::MaybeUninit, sync::{Arc, Mutex, Once}};

use super::app::AppBase;

pub fn get_app_handler() ->  &'static AppHandler{
    // Create an uninitialized static
    static mut SINGLETON: MaybeUninit<AppHandler> = MaybeUninit::uninit();
    static ONCE: Once = Once::new();

    unsafe {
        ONCE.call_once(|| {
            // Make it
            let singleton = AppHandler::new();
            // Store it to the static var, i.e. initialize it
            SINGLETON.write(singleton);
        });

        // Now we give out a shared reference to the data, which is safe to use
        // concurrently.
        SINGLETON.assume_init_ref()
    }
}

#[derive(Clone)]
pub struct AppHandler {
    // clients: Arc<Mutex<Vec<Box<dyn AppBase+ Send>>>>,
    pub clients: Arc<Mutex<Vec<Box<AppBase>>>>,
}


impl AppHandler {
        pub fn new() -> Self {
        let vec = Vec::<Box<AppBase>>::new();
        Self {
            clients: Arc::new(Mutex::new(vec)),
        }
    }
}