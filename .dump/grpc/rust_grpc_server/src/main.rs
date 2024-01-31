use tonic::{transport::Server, Request, Response, Status};
use agent::{IrisInferenceRequest, IrisInferenceResponse, iris_inference_server::{IrisInference, IrisInferenceServer}};


// mod => define module(library)
pub mod agent {
    // compile voting.proto, and make rust module named voting
    tonic::include_proto!("agent");
}

// #[] Atttribute: umeta data (c.f., C#)
// e.g., #[derive(...)]: automatic implementation basic impl for the inside (...)
// pub = public
#[derive(Debug, Default)]
pub struct TestServer {}

fn inference(sepal_length:f32, sepal_width:f32, petal_length:f32, petal_width:f32) -> String {
    let s = format!("Input features: {} {} {} {}", 
        sepal_length, sepal_width, petal_length, petal_width
    );
    println!("{}", s);
    
    format!("This iris' species is {}", "something")
}



// - impl Struct ... adds some methods to Struct. These methods aren't available to other types or traits.
// - impl Trait for Struct .. implements the trait Trait for the struct Struct. This results in the methods of the trait being available for Struct.
#[tonic::async_trait]
impl IrisInference for TestServer {
    async fn inference(&self, request: Request<IrisInferenceRequest>) -> Result<Response<IrisInferenceResponse>, Status> {
        let r = request.into_inner();
        
        let inference_result = inference(
            r.sepal_length, 
            r.sepal_width, 
            r.petal_length, 
            r.petal_width
        );
        
        Ok(Response::new(agent::IrisInferenceResponse {species: {
            inference_result
        }}))

        // error handling using the follwing
        // _ => Err(Status::new(tonic::Code::OutOfRange, "Invalide vote provided")),
    }
}


// tokio: rust async library
#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let address = "[::1]:8080".parse().unwrap();
    let iris_inference = TestServer::default();

    Server::builder().add_service(IrisInferenceServer::new(iris_inference))
        .serve(address)
        .await?;
    Ok(())
}
