use super::{mca_model1::{basic_server::BasicServer, iris_inference_server::IrisInferenceServer}, services::{basic::BasicService, iris::InferenceService}};

// #[derive(Eq, Hash, PartialEq, Clone)]
pub enum ServiceEnum {
    Basic(BasicServer<BasicService>),
    Service1(IrisInferenceServer<InferenceService>),
    None
}


pub fn get_service_name(service :ServiceEnum) -> &'static str {
    match service {
        ServiceEnum::Basic(_) => "BasicService",
        ServiceEnum::Service1(_) => "InferenceService",
        _ => "Unknown"
    }
}