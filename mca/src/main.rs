


use std::error::Error;
#[path = "agent/model_connection.rs"] mod model_connection;
#[path = "agent/model_structure.rs"] mod model_structure;
#[path = "agent/model_enum.rs"] mod model_enum;

use mca_package::api_backup::{app_connection, app_handler1::AppHandler};
// use mca_package::api::app_handler::{mca_basic::basic_server, mca_model1, AppHandler};
use model_enum::MCAModelEnum;
use model_connection::*;
// use model_structure::*;
use tonic::transport::Server;


// #[async_std::main]
#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {

    println!("register model1");
    let mut model_connection_handler = ModelConnection::new();
    let _result: Result<String, zbus::Error> = model_connection_handler.register_model(MCAModelEnum::Model1).await;

    println!("register model2");
    let _result: Result<String, zbus::Error> = model_connection_handler.register_model(MCAModelEnum::Model2).await;
    println!("Register result {:?}", _result);



    // FIXME: dbus error: 
    // println!("Send Iris data to Model1");
    // let _data = IrisData{col1:3.2, col2:3.2 ,col3:3.2 ,col4:3.2};
    // let serialized = serde_json::to_string(&_data).unwrap();

    // let model = model_connection_handler.get_model(MCAModelEnum::Model1).unwrap().downcast_ref::<Model1Struct>().unwrap();
    // let _reply = model.test(&serialized.to_string()).await?;
    
    
    println!("listening...");
    let address = "0.0.0.0:8080".parse()?;
    
    // Pass the builder and address to the connect method
    let mut builder = Server::builder();
    builder.add_service(AppHandler.service)
           .add_service(ServiceHandler.service1)
           .serve(address)
           .await?;
    
    Ok(())
}