use std::any::Any;

use tonic::{Request, Response, Status};


// Define a trait for gRPC services
pub trait ServiceBase {
    fn name(&self) -> &'static str;
    fn process_request(&self, request: &Request<(), >) -> Result<Response<()>, Status>;
    fn as_any(&self) -> &dyn Any;
}

