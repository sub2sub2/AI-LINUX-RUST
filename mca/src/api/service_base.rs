use super::app_base::Role;
use crate::agent::model_enum::MCAModelEnum;
use zbus::Connection;
use crate::agent::model_structure::*;
use std::sync::Arc;
use tokio::sync::Mutex;

pub struct ServiceBase {
    name: &'static str,
    port: u32,
    pub role: Role,
    pub model: Arc<Mutex<Box<dyn std::any::Any + Send + Sync>>>,
}

unsafe impl Send for ServiceBase {}
unsafe impl Sync for ServiceBase {}

impl ServiceBase {
    pub async fn new(name: &'static str, port: u32, role: Role, model_enum: MCAModelEnum) -> Self {
        let model: Arc<Mutex<Box<dyn std::any::Any + Send + Sync>>> = match model_enum {
            MCAModelEnum::Model1 => {
                let connection = Connection::session().await.expect("Failed to connect to session");
                let proxy = Model1Proxy::new(&connection).await.expect("Failed to create proxy");
                Arc::new(Mutex::new(Box::new(Model1Struct { proxy }) as Box<dyn std::any::Any + Send + Sync>))
            }
            MCAModelEnum::Model2 => {
                let connection = Connection::session().await.expect("Failed to connect to session");
                let proxy = Model2Proxy::new(&connection).await.expect("Failed to create proxy");
                Arc::new(Mutex::new(Box::new(Model2Struct { proxy }) as Box<dyn std::any::Any + Send + Sync>))
            }
            MCAModelEnum::Model3 => {
                let connection = Connection::session().await.expect("Failed to connect to session");
                let proxy = Model2Proxy::new(&connection).await.expect("Failed to create proxy");
                Arc::new(Mutex::new(Box::new(Model2Struct { proxy }) as Box<dyn std::any::Any + Send + Sync>))
            }
        };
        Self {
            name,
            port,
            role,
            model,
        }
    }
}
