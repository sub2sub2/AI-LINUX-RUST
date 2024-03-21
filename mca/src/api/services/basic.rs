use std::string;

use tonic::{Request, Response, Status};

use crate::api::{app_handler::{get_app_handler, AppHandler}, mca_model1::{basic_server::Basic, AppInfo, Empty}, service::ServiceBase, service_enum::{get_service_name, ServiceEnum}};



// how to capsulate?
#[derive(Clone, Copy)]
pub struct BasicService {
    name: &'static str,
    port: u32, 
    app_handler: &'static AppHandler

}

impl BasicService {
    pub fn new(name:&'static str, port:u32) -> Self {
        Self{
            name: name,
            port: port,
            app_handler: get_app_handler()
        }
    }
}



#[tonic::async_trait]
impl Basic for BasicService {
    async fn register(
        &self,
        request: Request<AppInfo>,
    ) -> Result<Response<Empty>, Status> {
        // let client_info = request.into_inner();
        // let client_address = client_info.address;

        // let mut clients = self.app_handler.clients.lock();

        // if !clients.iter().any(|client| client.address() == client_address) {

        //     let app = AppBase {
        //         appname: "someapp".to_string(),
        //         address: "192.168.0.1".to_string(),
        //         role: Role::Admin,
        //     };

        //     let client: Box<AppBase> = Box::new( app ); 
        //     // Box the ClientInfo into a trait object and add it to the clients vector
        //     clients.push(client);
            
        //     println!("Client registered: {:?}", client_address);
        // }

        Ok(Response::new(Empty {}))
    }
}

impl ServiceBase for BasicService {
    fn name(&self) -> &'static str {
        "BasicService"
    }
    
    fn process_request(&self, request: &Request<()>) -> Result<Response<()>, Status> {
        Ok(Response::new(()))
    }
    
    fn as_any(&self) -> &dyn std::any::Any {
        self
    }
}
