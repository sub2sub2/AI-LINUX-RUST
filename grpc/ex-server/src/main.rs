use tonic::{transport::Server, Request, Response, Status};
use voting::{VotingRequest, VotingResponse, voting_server::{Voting, VotingServer}};


// mod => define module(library)
pub mod voting {
    // compile voting.proto, and make rust module named voting
    tonic::include_proto!("voting");
}

// #[] Atttribute: umeta data (c.f., C#)
// e.g., #[derive(...)]: automatic implementation basic impl for the inside (...)
// pub = public
#[derive(Debug, Default)]
pub struct VotingService {}

// - impl Struct ... adds some methods to Struct. These methods aren't available to other types or traits.
// - impl Trait for Struct .. implements the trait Trait for the struct Struct. This results in the methods of the trait being available for Struct.
#[tonic::async_trait]
impl Voting for VotingService {
    async fn vote(&self, request: Request<VotingRequest>) -> Result<Response<VotingResponse>, Status> {
        let r = request.into_inner();
        // match = switch 
        match r.vote {
            0 => Ok(Response::new(voting::VotingResponse {confirmation: {
                format!("Happy to confirm that you updvoted for {}", r.url)
            }})),
            1 => Ok(Response::new(voting::VotingResponse {confirmation : {
                format!("Confirmation that you downvoted for {}", r.url)
            }})),
            _ => Err(Status::new(tonic::Code::OutOfRange, "Invalide vote provided")),
        }
    }
}



// tokio: rust async library
#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let address = "[::1]:8080".parse().unwrap();
    let voting_service = VotingService::default();

    Server::builder().add_service(VotingServer::new(voting_service))
        .serve(address)
        .await?;
    Ok(())
}
