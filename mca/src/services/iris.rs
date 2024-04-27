use tonic::{Request, Response, Status};

use crate::api::{app_base::{AppBase, Role}, service_base::ServiceBase};

use super::agent::{iris_inference_server::IrisInference, IrisInferenceRequest, IrisInferenceResponse};


#[tonic::async_trait]
impl IrisInference for ServiceBase {
    async fn inference(&self, request: Request<IrisInferenceRequest>) -> Result<Response<IrisInferenceResponse>, Status> {
        
        let app = AppBase::new (
            "someapp".to_string(), "192.168.0.1".to_string(), Role::Admin
        );

        // let r: IrisInferenceRequest = .into_inner();

        dbg! ("Iris Inference {:?} {:?}", app.appname(), app.address());


        let inference_result;
        match app.has_permission(self.role) {
            true => {
                let r = request.into_inner();
                inference_result = inference(r.sepal_length, r.sepal_width, r.petal_length, r.petal_width).await;
            }
            false => {
                return Result::Err(
                    Status::new(tonic::Code::PermissionDenied, "Permission Denied")
                );
            }
        }

        match inference_result {
            Ok(species) => {
                Ok(Response::new(IrisInferenceResponse {species: species}))
            }
            Err(_) => {
                Result::Err(
                    Status::new(tonic::Code::Unavailable, "UnAvailable")
                )
            }
        }
    }
}



// XXX: Error should be d-bus error. How to define?
async fn inference(
    sepal_length: f32, sepal_width: f32, petal_length: f32, petal_width: f32) 
-> Result<String, ()>
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