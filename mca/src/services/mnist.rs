use tonic::{Request, Response, Status};
use zbus::{Connection};
use zbus::fdo::Error as FdoError;

use crate::{
    agent::{
        model_enum::MCAModelEnum,
        model_structure::{Model2Proxy, Model2Struct, Model2Trait}
    },
    api::service_base::ServiceBase,
};

use super::agent::{mnist_inference_server::MnistInference, MnistInferenceRequest, MnistInferenceResponse};
use std::sync::Arc;
use tokio::sync::Mutex;

#[tonic::async_trait]
impl MnistInference for ServiceBase {
    async fn inference(&self, request: Request<MnistInferenceRequest>) -> Result<Response<MnistInferenceResponse>, Status> {
        let r = request.into_inner();

        match inference(self.model.clone(), r.file_path).await {
            Ok(inference_result) => Ok(Response::new(MnistInferenceResponse { file_path: inference_result })),
            Err(_) => Err(Status::new(tonic::Code::Unavailable, "Unavailable")),
        }
    }
}

async fn inference(
    model: Arc<Mutex<Box<dyn std::any::Any + Send + Sync>>>,
    file_path: String
) -> Result<String, FdoError> {
    dbg!("Input features", file_path.clone());


    // Lock the model and perform the inference
    let model = model.lock().await;
    if let Some(model) = model.downcast_ref::<Model2Struct>() {
        let reply = model.test(&file_path).await?;
        dbg!(&reply);
        Ok(reply)
    } else {
        Err(FdoError::Failed("Failed to downcast model".into()))
    }
}
