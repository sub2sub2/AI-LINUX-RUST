use super::app_base::Role;


// #[derive(Default)]
pub struct ServiceBase {
    name:&'static str,
    port: u32,
    pub role: Role,
    // app_manager: AppManager
    // model: &'static ModelConnection
}

impl ServiceBase {
    pub fn new(name :&'static str, port:u32, role:Role) -> Self {
        Self {
            name: name,
            port: port,
            role: role,
            // model: model
        }
    }
    
}