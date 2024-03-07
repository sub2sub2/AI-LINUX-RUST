


use std::{error::Error, io};
#[path = "agent/model_connection.rs"] mod model_connection;
#[path = "agent/model_structure.rs"] mod model_structure;
#[path = "agent/model_enum.rs"] mod model_enum;

use model_enum::MCAModelEnum;
use model_connection::*;
use model_structure::*;

fn print_description() {
    println!("-----------------------------");
    println!("r1: register model1");
    println!("r2: register model2");
    println!("s1: request inference to model1");
    println!("s2: request inference to model2");
    println!("q: quit");
}

#[async_std::main]
async fn main() -> Result<(), Box<dyn Error>> {
    // let _conn = ConnectionBuilder::session()?
    //     .name("org.zbus.DataIPC")?
    //     .serve_at("/org/zbus/DataIPC")?
    //     .build()
    //     .await?;

    // // Do other things or go to wait forever
    // pending::<()>().await;
    let mut model_connection_handler = ModelConnection::new();
    loop {
        print_description();
        let mut input = String::new();

        io::stdin().read_line(&mut input)
            .expect("Fail to read input");

            let trimmed_input = input.trim();
            
            match trimmed_input.to_lowercase().as_str() {
                "r1" => {
                    println!("Register Model1 ");
                    // let mut send_input = String::new();
                    // io::stdin().read_line(&mut send_input)
                    //     .expect("Fail to read input");

                    let _result: Result<String, zbus::Error> = model_connection_handler.register_model(MCAModelEnum::Model1).await;
                    println!("Register resut {:?}", _result);
                }
                "r2" => {
                    println!("Register Model2 ");
                    // let mut send_input = String::new();
                    // io::stdin().read_line(&mut send_input)
                    //     .expect("Fail to read input");

                    let _result: Result<String, zbus::Error> = model_connection_handler.register_model(MCAModelEnum::Model2).await;
                    println!("Register resut {:?}", _result);

                }
                "s1" => {
                    println!("Send Iris data to Model1");
                    let _data = IrisData{col1:3.2, col2:3.2 ,col3:3.2 ,col4:3.2};
                    let serialized = serde_json::to_string(&_data).unwrap();

                    let model = model_connection_handler.get_model(MCAModelEnum::Model1).unwrap().downcast_ref::<Model1Struct>().unwrap();
                    let _reply = model.test(&serialized.to_string()).await?;
                    println!("Output is {_reply}");
                }
                "s2" => {
                    println!("Send Iris data to Model2");
                    let _data = IrisData{col1:3.2, col2:3.2 ,col3:3.2 ,col4:3.2};
                    let serialized = serde_json::to_string(&_data).unwrap();

                    let model = model_connection_handler.get_model(MCAModelEnum::Model2).unwrap().downcast_ref::<Model2Struct>().unwrap();
                    let _reply = model.test(&serialized.to_string()).await?;
                    println!("Output is {_reply}");
                }

                

                "q" => {
                    println!("Quit");
                    break;
                }
                _ => {
                    println!("Invalid input");
                }
            }
    }
    Ok(())
}