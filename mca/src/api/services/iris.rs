use std::{any::Any, future::IntoFuture};

use futures_util::Future;
use tonic::{transport::Error, Request, Response, Status};
use tokio::runtime::Runtime;
// use zbus::Error; // 0.3.5

use crate::{agent::{model_connection::{self, *}, model_enum::MCAModelEnum, model_structure::{IrisData, Model1Struct, Model1Trait}}, api::{app::{AppBase, Resource, Role}, mca_model1::{iris_inference_server::IrisInference, IrisInferenceRequest, IrisInferenceResponse}, service::ServiceBase, service_enum::{get_service_name, ServiceEnum}}};
use std::pin::Pin;


// Define a simple gRPC service
// #[derive(Default)]
pub struct InferenceService{
    name:&'static str,
    port: u32,
    role: Role,
    // model: &'static ModelConnection
}

impl InferenceService {
    pub fn new(name :&'static str, port:u32, role:Role) -> Self {
        Self {
            name: name,
            port: port,
            role: role,
            // model: model
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

        dbg! ("Iris Inference {:?} {:?}", app.appname(), app.address());

        let inference_result = self.perform_action(&app, request).await;

        // Ok(Response::new(IrisInferenceResponse { species }))
        // match self.perform_action(&app, request).await {
        //     Ok(species) => Ok(Response::new(IrisInferenceResponse { species })),
        //     Err(err) => {
        //         let status = Status::internal("D-Bus error occurred");
        //         Err(status)
        //     }
        // }

        // let inference_result = self.perform_action(&app, request).await?;
        // Ok(Response::new(IrisInferenceResponse {species: inference_result}))

        match inference_result {
            Ok(species) => {
                Ok(Response::new(IrisInferenceResponse {species: species}))
            }
            Err(err) => {
                Result::Err(
                    Status::new(tonic::Code::PermissionDenied, "Permission Denied")
                )
            }
        }
        
        
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



// XXX: Should return tonic:transport::Error
impl Resource<IrisInferenceRequest, String> for InferenceService {
    async fn perform_action(
        &self, 
        app: &AppBase,
        req: Request<IrisInferenceRequest> )
    -> Result<String, Error>
    {   

        let r = req.into_inner();
        match app.has_permission(self.role) {
            true => {
                
                dbg!("try inference");
                
                let inference_result = inference(
                    r.sepal_length, 
                    r.sepal_width, 
                    r.petal_length, 
                    r.petal_width
                )
                .await?;
                
                Ok(inference_result)
            }
            false => {
                panic!("User '{}' does not have permission to perform action on service '{}'.", app.appname(), self.name);
            }
        }

    }
}



async fn inference(
    sepal_length: f32, sepal_width: f32, petal_length: f32, petal_width: f32) 
-> Result<String, Error>
// -> impl Future<Output=Result<String, zbus::Error>>
{
    dbg!("Input features", 
        sepal_length, sepal_width, petal_length, petal_width
    );

    dbg! ("Please add the model here!");
    // let mut instance: std::sync::MutexGuard<'_, ModelConnection> = MODEL_CONNECTION.lock().unwrap(); // We should separate getting model and call method in this case(?)

    // let model = instance.get_model(MCAModelEnum::Model1).unwrap().downcast_ref::<Model1Struct>().unwrap();


    // let _data = IrisData{col1:3.2, col2:3.2 ,col3:3.2 ,col4:3.2};
    // let serialized = serde_json::to_string(&_data).unwrap();

    // let reply = model.test(&serialized.to_string()).await;
    // match reply {
    //     Ok(model_return) => {
    //         Ok(model_return)
    //     }
    //     Err(err) => {
    //         panic!("Dbus Error: {}", err)
    //     }
    // }

    Ok("Good".to_string())

}
