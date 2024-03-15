use std::net::SocketAddr;
use tonic::transport::Server;

pub trait Connectable {
    type ServerType;

    async fn connect(builder: Server, address: SocketAddr) -> Result<(), Box<dyn std::error::Error>>;
    // where
    //     Self: Sized;
}

