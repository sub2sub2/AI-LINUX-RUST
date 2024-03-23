// use std::sync::Arc;
use std::{any::Any, convert::Infallible};

use futures_util::{stream::select_all::Iter, Future};
use tonic::{body::BoxBody, client::GrpcService, server::NamedService, transport::{server::Router, Body, Server}, Request, Response, Status};
use crate::api::mca_model1::iris_inference_server::IrisInference;

use super::{mca_model1::{basic_server::BasicServer, iris_inference_server::IrisInferenceServer}, service::ServiceBase, service_enum::{self, ServiceEnum}, services::{basic::BasicService, iris::InferenceService}};


// Define a strucServiceture to store multiple gRPC services
pub struct GrpcServiceRegistry {
    services: Vec<Box<ServiceEnum>>,
    // services: Vec<Arc<dyn ServiceBase>>,
}

impl GrpcServiceRegistry {
    pub fn new() -> Self {
        GrpcServiceRegistry {
            services: Vec::new(),
        }
    }
    
    pub fn register_service(&mut self, service: ServiceEnum){
        self.services.push(
            Box::new(service)
        )
    }

    pub fn get_services(&mut self) -> Vec<Option<impl Future<Output = Result<(), tonic::transport::Error>>>> {
    // pub fn get_services(&mut self) -> Vec::<Option<Router>> {
        let mut services = Vec::new();
        // let mut services = Vec::<Option<Router>>::new();

        for svc in &self.services {

            match svc.as_ref() {
                ServiceEnum::Basic(svc) => {
                    
                    services.push(
                        Some(
                            Server::builder().add_service(svc.clone())
                            .serve("[::1]:50050".parse().unwrap())
                        )
                    );
                }
                ServiceEnum::Service1(svc) => {
                    services.push(
                        Some(
                            Server::builder().add_service(svc.clone())
                            .serve("[::1]:8080".parse().unwrap())
                        )
                    );
                }
                ServiceEnum::None => {}
            }

        }
        services
    }


    // pub fn register_service<S>(&mut self, service: S)
    // where
    //     S: ServiceBase + Send + Sync + 'static,
    // {
    //     println!("Service {:?} registered!", service.name());
    //     // self.services.push(Arc::new(service));
    //     self.services.push(
    //         ServiceEnum::Service1
    //     );


    // }

    // pub async fn process_request(&self, request: Request<()>) -> Result<Response<()>, Status> {
    //     for service in &self.services {
    //         let response = service.process_request(&request);
    //         if response.is_ok() {
    //             return response;
    //         }
    //     }
    //     Err(Status::unimplemented("No service found to handle the request"))
    // }

    // fn get_service(&self, service:&Box<dyn Any>) -> ServiceEnum {
        
    //     let mut server = Server::builder();


    //     let svc = match service.downcast_ref::<InferenceService>(){

    //         Some(s) => {

    //             let name= s.name();
    //             match name { 
    //                 "InferenceService" => {
    //                     let svc = IrisInferenceServer::new (*s);
    //                     return ServiceEnum::Service1(svc);
    //                 }
    //                 "BasicService" => {
    //                     let svc = BasicServer::new (*s);
    //                     router = Option::Some(server.add_service(service));

    //                 }
    //                 _ => {
    //                     panic!("Unknown service name: {}", name);
                        
    //                 }
    //             }
    //         };
    //         None => {
    //             panic!("Something wrong");
    //         }
    //     }
    // }

    // pub fn get_service_route(&self, addr: &str, svc:ServiceEnum) -> Vec::<Option<Router>> {
        
    //     let mut services = Vec::<Router>::new();
        
    //     println!("serve on addr: {:?}", addr);
    //     for service in &self.services {
            
    //         let router = self.get_service(service);
    //         services.append(router);
            
    //     }

    //     match router {
    //         Some(r) => {
    //             Ok(r)
    //         }
    //         None => {
    //             panic!();
    //         }
    //     }
        
    // }
}
