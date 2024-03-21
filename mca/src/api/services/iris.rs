use std::any::Any;

use tonic::{transport::Error, Request, Response, Status};

use crate::api::{app::{AppBase, Resource, Role}, mca_model1::{iris_inference_server::IrisInference, IrisInferenceRequest, IrisInferenceResponse}, service::ServiceBase, service_enum::{get_service_name, ServiceEnum}};


// Define a simple gRPC service
// #[derive(Default)]
pub struct InferenceService{
    name:&'static str,
    port: u32,
    role: Role
}

impl InferenceService {
    pub fn new(name :&'static str, port:u32, role:Role) -> Self {
        Self {
            name: name,
            port: port,
            role: role
        }
    }
    
}

#[tonic::async_trait]
impl IrisInference for InferenceService {
    async fn inference(&self, request: Request<IrisInferenceRequest>) -> Result<Response<IrisInferenceResponse>, Status> {
        

        let app = AppBase::new (
            "someapp".to_string(), "192.168.0.1".to_string(), Role::Admin
        );

        // let r: IrisInferenceRequest = .into_inner();

        println! ("Iris Inference {:?} {:?}", app.appname(), app.address());

        let inference_result = self.perform_action(&app, request);
        
        match inference_result {
            Result::Ok(species) => {
                Ok(Response::new(IrisInferenceResponse {species: species}))
            }
            Result::Err(err) => {
                Result::Err(
                    Status::new(tonic::Code::PermissionDenied, "Permission Denied")
                )
            }
        }
        
        // let inference_result = inference(
        //     r.sepal_length, 
        //     r.sepal_width, 
        //     r.petal_length, 
        //     r.petal_width
        // );
        
    }
}
impl ServiceBase for InferenceService{
    fn name(&self) -> &'static str {
        "to do something in name"
        // get_service_name(ServiceEnum::Service1(_))
    }
    
    fn as_any(&self) -> &dyn Any {
        self
    }

    fn process_request(&self, request: &Request<()>) -> Result<Response<()>, Status> {
        // self.inference(request)
        Ok(Response::new(()))
    }
}



impl Resource<IrisInferenceRequest, String> for InferenceService {
    fn perform_action(
        &self, 
        app: &AppBase,
        req: Request<IrisInferenceRequest>
    ) -> Result<String, Error> 
    {   
        let r = req.into_inner();
        match app.has_permission(self.role) {
            true => {
                let inference_result = inference(
                    r.sepal_length, 
                    r.sepal_width, 
                    r.petal_length, 
                    r.petal_width
                );
                
                Ok(inference_result)
            }
            false => {
                panic!("User '{}' does not have permission to perform action on service '{}'.", app.appname(), self.name);
            }
        }
    }
}




fn inference(sepal_length: f32, sepal_width: f32, petal_length: f32, petal_width: f32) -> String {
    let s = format!("Input features: {} {} {} {}", 
        sepal_length, sepal_width, petal_length, petal_width
    );
    println!("{}", s);
    
    format!("This iris' species is {}", "something")
}
