use tonic::service::interceptor::InterceptedService;

use crate::services::agent::iris_inference_server::IrisInferenceServer;
use super::service_base::ServiceBase;


// #[derive(Eq, Hash, PartialEq, Clone)]
pub enum ServiceEnum {
    // Basic(BasicServer<BasicService>),
    // IrisService(IrisInferenceServer<ServiceBase>),
    IrisService(
        InterceptedService< // This will call whenever gRPC call occurs.
            IrisInferenceServer<ServiceBase>, 
            fn(tonic::Request<()>) -> Result<tonic::Request<()>, tonic::Status>
            >
    ),
    None
}
