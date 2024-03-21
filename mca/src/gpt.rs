use std::thread::JoinHandle;

use mca_package::api::{app::Role, mca_model1::{basic_server::BasicServer, iris_inference_server::IrisInferenceServer}, service_enum::{get_service_name, ServiceEnum}, service_handler::GrpcServiceRegistry, services::{basic::BasicService, iris::InferenceService}};
use tonic::transport::Server;

use tokio::task::JoinSet;

#[tokio::main]
async fn main() {
// async fn main() -> Result<(), Box<dyn std::error::Error>> {
    

    let mut registry = GrpcServiceRegistry::new();
    registry.register_service(
        ServiceEnum::Basic(
            BasicServer::<BasicService>::new (
                BasicService::new("Basic", 50050)
            )
        )
    );
    registry.register_service(
        ServiceEnum::Service1(
            IrisInferenceServer::<InferenceService>::new(
                InferenceService::new("iris", 8080, Role::Admin )
            )
        )
    );
    let mut set = JoinSet::new();






    for service in registry.get_services() {
        
        match service {
            Some(svc) => {
                // println!("{:?}", svc);
                set.spawn( 
                    svc
                    // .serve(address)
                );
            }
            None => {
                panic!("invalid service");
            }
        }

        
    }
    while let Some(res) = set.join_next().await {
        
        // error handling
        let _ = res;

        // match res{
        //     Some(out) => {
        //         Ok() => 
        //     }
        // };
        // ...
    }

}

