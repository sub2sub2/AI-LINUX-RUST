use lazy_static::lazy_static;
use std::sync::Mutex;
use std::collections::HashMap;
use crate::model_enum::MCA_MODEL_ENUM;
use crate::model_structure::*;
use zbus::{Connection, Result};

pub struct ModelConnection {
    connection_map: HashMap<MCA_MODEL_ENUM, Box<dyn AIModelTrait>>,
}


pub trait MCAOperations {
    fn new() -> Self;
    async fn register_model(&mut self, model_enum: MCA_MODEL_ENUM) -> Result<String>;
    fn get_model(&mut self, model_enum: MCA_MODEL_ENUM) -> &Box<dyn AIModelTrait + 'static>;
}

impl MCAOperations for ModelConnection {
    fn new() -> Self{
        let connection_map:HashMap<MCA_MODEL_ENUM, Box<dyn AIModelTrait>> = HashMap::new();
        ModelConnection {connection_map}
    }

    async fn register_model(&mut self, model_enum: MCA_MODEL_ENUM) -> Result<String> {
        if self.connection_map.contains_key(&model_enum) {
            Ok("Fail".to_string())
        } else {
            match model_enum {
                MCA_MODEL_ENUM::MODEL_1 => {
                    let connection = Connection::session().await?;
                   let proxy = Model1Proxy::new(&connection).await?;
                   self.connection_map.insert(model_enum, Box::new(Model1Struct {
                        proxy,
                    }) as Box<dyn AIModelTrait>);
                    return Ok("success".to_string());
                }
                MCA_MODEL_ENUM::MODEL_2 => {
                    let connection = Connection::session().await?;
                    let proxy = Model2Proxy::new(&connection).await?;
                    self.connection_map.insert(model_enum, Box::new(Model2Struct {
                        proxy,
                    }) as Box<dyn AIModelTrait>);
                    return Ok("success".to_string());
                }
                _ => panic!("Invalid model type"),
            }
        }
    }
    fn get_model(&mut self, model_enum: MCA_MODEL_ENUM) -> &Box<dyn AIModelTrait + 'static> {
        self.connection_map.get(&model_enum).unwrap()
    }

}