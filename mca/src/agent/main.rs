use std::{error::Error, io};
mod model_connection;
mod model_enum;
mod model_structure;

use model_enum::MCA_MODEL_ENUM;
use model_connection::*;
use model_structure::*;

fn print_description() {
    println!("-----------------------------");
    println!("s: send string");
    println!("d: send json string data");
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

    loop {
        print_description();
        let mut input = String::new();

        io::stdin().read_line(&mut input)
            .expect("Fail to read input");

            let trimmed_input = input.trim();

            match trimmed_input.to_lowercase().as_str() {
                "r" => {
                    println!("Register Model1 ");
                    // let mut send_input = String::new();
                    // io::stdin().read_line(&mut send_input)
                    //     .expect("Fail to read input");
                    let mut model_connection_handler = ModelConnection::new();
                    
                    let _result: Result<String, zbus::Error> = model_connection_handler.register_model(MCA_MODEL_ENUM::MODEL_1).await;
                    let model = model_connection_handler.get_model(MCA_MODEL_ENUM::MODEL_1);
                    let _reply = model.test("testset").await?;

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