use tonic::service::interceptor::InterceptedService;

use crate::services::agent::iris_inference_server::IrisInferenceServer;
use crate::services::agent::mnist_inference_server::MnistInferenceServer;

use super::service_base::ServiceBase;


// #[derive(Eq, Hash, PartialEq, Clone)]
pub enum ServiceEnum {
    IrisService(
        InterceptedService< // This will call whenever gRPC call occurs.
            IrisInferenceServer<ServiceBase>, 
            fn(tonic::Request<()>) -> Result<tonic::Request<()>, tonic::Status>
            >
    ),
    MnistService(
        InterceptedService< // This will call whenever gRPC call occurs.
            MnistInferenceServer<ServiceBase>, 
            fn(tonic::Request<()>) -> Result<tonic::Request<()>, tonic::Status>
            >
    ),
    None
}
