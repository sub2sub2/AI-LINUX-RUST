mod app_ex;
mod app_connection;

use std::net::SocketAddr;
use tonic::transport::Server;
// use appEx::AppEx;
use crate::app_connection::Connectable;

// tokio: rust async library
#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    
    let address:SocketAddr = "[::1]:8080".parse().unwrap();

    // Instantiate TestServer directly since it implements Connectable
    // let test_server = AppEx {};

    // Pass the builder and address to the connect method
    app_ex::AppEx::connect(Server::builder(), address).await?;

    Ok(())
}
