use lazy_static::lazy_static;
use std::sync::Arc;
use std::collections::HashMap;
use super::model_enum::MCAModelEnum;
use super::model_structure::*;
use zbus::Connection;

use futures::lock::Mutex;

lazy_static! {
    pub static ref MODEL_CONNECTION: Arc<Mutex<ModelConnection>> = Arc::new(Mutex::new(ModelConnection::new()));
}
pub struct ModelConnection<> {
    connection_map: HashMap<MCAModelEnum, Box<dyn std::any::Any>>,
}

unsafe impl Send for ModelConnection {} // We have to clare this part because lazy_static uses Mutex which needs Send and Sync
unsafe impl Sync for ModelConnection {}

pub trait MCAOperations {
    fn new() -> Self;
    fn register_model(&mut self, model_enum: MCAModelEnum) -> impl std::future::Future<Output = zbus::Result<String>> + Send;
    fn get_model(&mut self, model_enum: MCAModelEnum) -> Result<&Box<dyn std::any::Any >, String>;
    fn clean_registered_model(&mut self);
}

impl MCAOperations for ModelConnection{
    fn new() -> Self{
        let connection_map:HashMap<MCAModelEnum, Box<dyn std::any::Any>> = HashMap::new();
        ModelConnection {connection_map}
    }

    async fn register_model(&mut self, model_enum: MCAModelEnum) -> zbus::Result<String> {
        if self.connection_map.contains_key(&model_enum) {
            Ok("Fail".to_string())
        } else {
            match model_enum {
                MCAModelEnum::Model1 => {
                    let connection = Connection::session().await?;
                    let proxy = Model1Proxy::new(&connection).await?;
                    self.connection_map.insert(model_enum, Box::new(Model1Struct {
                            proxy,
                    }));
                    return Ok("success".to_string());
                }
                MCAModelEnum::Model2 => {
                    let connection = Connection::session().await?;
                    let proxy = Model2Proxy::new(&connection).await?;
                    self.connection_map.insert(model_enum, Box::new(Model2Struct {
                            proxy,
                    }));
                    return Ok("success".to_string());
                }
                MCAModelEnum::Model3 => {
                    let connection = Connection::session().await?;
                    let proxy = Model2Proxy::new(&connection).await?;
                    self.connection_map.insert(model_enum, Box::new(Model2Struct {
                            proxy,
                    }));
                    return Ok("success".to_string());
                }
            }
        }
    }
    fn get_model(&mut self, model_enum: MCAModelEnum) -> Result<&Box<dyn std::any::Any>, String>{
        if let Some(value) = self.connection_map.get(&model_enum) {
            Ok(value)
        } else {
            Err(format!("Model with enum {:?} not found", model_enum))
        }
    }

    fn clean_registered_model(&mut self) {
        self.connection_map.clear();
    }

}