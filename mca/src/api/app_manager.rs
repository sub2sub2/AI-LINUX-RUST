
use std::{ collections::HashMap, sync::{Arc, Mutex}};

use tonic::{metadata::MetadataValue, Request, Status};

use crate::common::utils::get_id;

use super::app_base::{AppBase, Role};

use lazy_static::lazy_static;

pub struct AppManager {
    pub app_list:HashMap<usize, Arc<Mutex<AppBase>>>,
    pub n_apps: u8
}

impl AppManager {
    pub fn new() -> Self {
        AppManager { app_list: HashMap::new(), n_apps:0 }
    }

    fn register(&mut self, name: String, addr:String, role: Role) -> Arc<Mutex<AppBase>> {
        let app_point = Arc::new(Mutex::new(AppBase::new(name, addr, role)));
        let id = get_id();

        self.app_list.insert(
            id.clone(),
            app_point
        );
        self.app_list[&id].clone()
    }

    fn change_role(&self, id:usize, role:Role) {
        let mut data = self.app_list[&id].lock().unwrap();
        data.set_role(role);
    }
    

    fn check_auth(&self) -> MetadataValue<tonic::metadata::Ascii>{
        // todo!("Make a authentification process ");
        "Bearer some-secret-token".parse().unwrap()
    }
}

lazy_static! {
    static ref APP_MANAGER: AppManager = AppManager::new();
}
pub fn check_auth(req: Request<()>) -> Result<Request<()>, Status>{

    let token = APP_MANAGER.check_auth();
    match req.metadata().get("authorization") {
        Some(t) if token == t => {
            dbg!(t);
            Ok(req)
        },
        _ => Err(Status::unauthenticated("No valid auth token")),
    }
}