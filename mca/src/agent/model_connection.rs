use lazy_static::lazy_static;
use std::sync::Mutex;
use std::collections::HashMap;
use crate::model_enum::MCA_MODEL_ENUM;
use crate::model_structure::*;
use zbus::{Connection};

pub struct ModelConnection<> {
    connection_map: HashMap<MCA_MODEL_ENUM, Box<dyn std::any::Any>>,
}


pub trait MCAOperations {
    fn new() -> Self;
    async fn register_model(&mut self, model_enum: MCA_MODEL_ENUM) -> zbus::Result<String>;
    fn get_model(&mut self, model_enum: MCA_MODEL_ENUM) -> Result<&Box<dyn std::any::Any>, String>;
    fn clean_registered_model(&mut self);
}

impl MCAOperations for ModelConnection{
    fn new() -> Self{
        let connection_map:HashMap<MCA_MODEL_ENUM, Box<dyn std::any::Any>> = HashMap::new();
        ModelConnection {connection_map}
    }

    async fn register_model(&mut self, model_enum: MCA_MODEL_ENUM) -> zbus::Result<String> {
        if self.connection_map.contains_key(&model_enum) {
            Ok("Fail".to_string())
        } else {
            match model_enum {
                MCA_MODEL_ENUM::MODEL_1 => {
                    let connection = Connection::session().await?;
                    let proxy = Model1Proxy::new(&connection).await?;
                    self.connection_map.insert(model_enum, Box::new(Model1Struct {
                            proxy,
                    }));
                    return Ok("success".to_string());
                }
                MCA_MODEL_ENUM::MODEL_2 => {
                    let connection = Connection::session().await?;
                    let proxy = Model2Proxy::new(&connection).await?;
                    self.connection_map.insert(model_enum, Box::new(Model2Struct {
                            proxy,
                    }));
                    return Ok("success".to_string());
                }
                _ => panic!("Invalid model type"),
            }
        }
    }
    fn get_model(&mut self, model_enum: MCA_MODEL_ENUM) -> Result<&Box<dyn std::any::Any>, String>{
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