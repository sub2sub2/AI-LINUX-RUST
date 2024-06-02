use tonic::{Request, Response, Status};
use zbus::{Connection, fdo};
use zbus::fdo::Error as FdoError;

use crate::{
    agent::{
        model_connection::{MCAOperations, MODEL_CONNECTION},
        model_enum::MCAModelEnum,
        model_structure::{IrisData, Model1Proxy, Model1Struct, Model1Trait}
    },
    api::service_base::ServiceBase,
};

use super::agent::{iris_inference_server::IrisInference, IrisInferenceRequest, IrisInferenceResponse};
use std::sync::Arc;
use tokio::sync::Mutex;

#[tonic::async_trait]
impl IrisInference for ServiceBase {
    async fn inference(&self, request: Request<IrisInferenceRequest>) -> Result<Response<IrisInferenceResponse>, Status> {
        let r = request.into_inner();

        match inference(self.model.clone(), r.sepal_length, r.sepal_width, r.petal_length, r.petal_width).await {
            Ok(inference_result) => Ok(Response::new(IrisInferenceResponse { species: inference_result })),
            Err(_) => Err(Status::new(tonic::Code::Unavailable, "Unavailable")),
        }
    }
}

async fn inference(
    model: Arc<Mutex<Box<dyn std::any::Any + Send + Sync>>>,
    sepal_length: f32,
    sepal_width: f32,
    petal_length: f32,
    petal_width: f32
) -> Result<String, FdoError> {
    dbg!("Input features", sepal_length, sepal_width, petal_length, petal_width);

    // Construct the input data for the model
    let data = IrisData {
        col1: sepal_length,
        col2: sepal_width,
        col3: petal_length,
        col4: petal_width,
    };
    let serialized = serde_json::to_string(&data).unwrap();

    // Lock the model and perform the inference
    let model = model.lock().await;
    if let Some(model) = model.downcast_ref::<Model1Struct>() {
        let reply = model.test(serialized).await?;
        dbg!(&reply);
        Ok(reply)
    } else {
        Err(FdoError::Failed("Failed to downcast model".into()))
    }
}
