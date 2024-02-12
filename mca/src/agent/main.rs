use std::{error::Error, io};

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
        let mut input = String::new();

        io::stdin().read_line(&mut input)
            .expect("Fail to read input");

            let trimmed_input = input.trim();

            match trimmed_input.to_lowercase().as_str() {
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