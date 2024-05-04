use futures_util::Future;
use tonic::transport::{Error, Server};

use super::service_enum::ServiceEnum;


// Define a strucServiceture to store multiple gRPC services
pub struct ServiceManager {
    services: Vec<Box<ServiceEnum>>,
    // services: Vec<Arc<dyn ServiceBase>>,
}

impl ServiceManager {
    pub fn new() -> Self {
        ServiceManager {
            services: Vec::new(),
        }
    }
    
    pub fn register_service(&mut self, service: ServiceEnum){
        self.services.push(
            Box::new(service)
        )
    }

    pub fn get_services(&mut self) -> Vec<Option<impl Future<Output = Result<(), Error>>>> {
    // pub fn get_services(&mut self) -> Vec::<Option<Router>> {
        let mut services = Vec::new();

        for svc in &self.services {

            match svc.as_ref() {
                ServiceEnum::IrisService(svc) => {
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

}