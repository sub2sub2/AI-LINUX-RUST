use tonic::{Request, Response, Status};
use zbus::{Connection, Error};

use crate::{agent::{model_connection::{MCAOperations, MODEL_CONNECTION}, model_enum::MCAModelEnum, model_structure::{IrisData, Model1Proxy, Model1Struct, Model1Trait}}, api::service_base::ServiceBase};

use super::agent::{iris_inference_server::IrisInference, IrisInferenceRequest, IrisInferenceResponse};




#[tonic::async_trait]
impl IrisInference for ServiceBase {
    async fn inference(&self, request: Request<IrisInferenceRequest>) -> Result<Response<IrisInferenceResponse>, Status> {
        
        let r = request.into_inner();
        
        match inference(r.sepal_length, r.sepal_width, r.petal_length, r.petal_width).await
        {
            Ok(inference_result) =>{
                return Ok(Response::new(IrisInferenceResponse {species: inference_result}));
            }
            Err(_) => {
                return Result::Err(
                        Status::new(tonic::Code::Unavailable, "UnAvailable")
                    );
            }
        };
        
    }
}



// XXX: Error should be d-bus error. How to define?
async fn inference(
    sepal_length: f32, sepal_width: f32, petal_length: f32, petal_width: f32)
-> Result<String, Error>  
{
    dbg!("Input features", 
        sepal_length, sepal_width, petal_length, petal_width
    );

    dbg! ("Please add the model here!");
    
    let mut  conn = MODEL_CONNECTION.lock().await;
    let model = (*conn).get_model(MCAModelEnum::Model1).unwrap().downcast_ref::<Model1Struct>().unwrap();;
    // let model = (*conn).get_model(MCAModelEnum::Model1).unwrap().downcast_ref::<Model1Struct>().unwrap(); // The Hashmap has std::any::Any type as a value, so we need to cast it into Model Structure
    
    // let connection = Connection::session().await?;

    // let proxy = Model1Proxy::new(&connection).await?;
    // let model = Model1Struct{proxy};

    let _data = IrisData{col1:sepal_length, col2:sepal_width ,col3:petal_length ,col4:petal_width};
    let serialized = serde_json::to_string(&_data).unwrap();

    let reply = model.test(serialized.to_string()).await;
    
    dbg!(&reply);
    reply
    
}