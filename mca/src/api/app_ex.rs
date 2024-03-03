// app_ex.rs

use std::net::SocketAddr;

use tonic::{transport::Server, Request, Response, Status};
use agent::{IrisInferenceRequest, IrisInferenceResponse, iris_inference_server::{IrisInference, IrisInferenceServer}};

use crate::app_connection::Connectable;


// mod => define module(library)
pub mod agent {
    // compile voting.proto, and make rust module named voting
    tonic::include_proto!("agent");
}



// - impl Struct ... adds some methods to Struct. These methods aren't available to other types or traits.
// - impl Trait for Struct .. implements the trait Trait for the struct Struct. This results in the methods of the trait being available for Struct.
#[tonic::async_trait]
impl IrisInference for AppEx {
    async fn inference(&self, request: Request<IrisInferenceRequest>) -> Result<Response<IrisInferenceResponse>, Status> {
        let r = request.into_inner();
        
        let inference_result = inference(
            r.sepal_length, 
            r.sepal_width, 
            r.petal_length, 
            r.petal_width
        );
        
        Ok(Response::new(agent::IrisInferenceResponse {species: inference_result}))
    }
}



fn inference(sepal_length: f32, sepal_width: f32, petal_length: f32, petal_width: f32) -> String {
    let s = format!("Input features: {} {} {} {}", 
        sepal_length, sepal_width, petal_length, petal_width
    );
    println!("{}", s);
    
    format!("This iris' species is {}", "something")
}




impl Connectable for AppEx {
    type ServerType = IrisInferenceServer<AppEx>;

    async fn connect( mut builder: Server, address: SocketAddr) -> Result<(), Box<dyn std::error::Error>> {
        // Add the IrisInference service to the server
        let server = builder.add_service(Self::ServerType::new(AppEx {}));
        server.serve(address).await?;
        Ok(())
    }
}



// #[derive(Debug, Default)]
pub struct AppEx {}
