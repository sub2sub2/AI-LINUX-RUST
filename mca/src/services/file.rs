use tonic::{Request, Response, Status};
use zbus::{Connection};
use zbus::fdo::Error as FdoError;
use std::env;
use std::fs::File;
use std::io::{self, Read, Write};
use std::net::TcpStream;
use tempfile::NamedTempFile;


use crate::{
    agent::{
        model_enum::MCAModelEnum,
        model_structure::{Model2Proxy, Model2Struct, Model2Trait}
    },
    api::service_base::ServiceBase,
};

use super::agent::{file_inference_server::FileInference, FileInferenceRequest, FileInferenceResponse};
use std::sync::Arc;
use tokio::sync::Mutex;

#[tonic::async_trait]
impl FileInference for ServiceBase {
    async fn inference(&self, request: Request<FileInferenceRequest>) -> Result<Response<FileInferenceResponse>, Status> {
        let r = request.into_inner();
        let ip = "127.0.0.1";
        let port = r.port;
        let file_size = r.file_size;
        let is_remote = r.is_remote;
        let mut temp_file = NamedTempFile::new_in(env::temp_dir())?;

        let server_address = format!("{}:{}", ip, port);
        println!("Server addr: {}", server_address);
        let mut stream = TcpStream::connect(server_address)?;

        let mut buffer = [0; 1024];
        while let Ok(bytes_read) = stream.read(&mut buffer) {
            if bytes_read == 0 {
                break;
            }
            println!("receiving");
            temp_file.write_all(&buffer[..bytes_read])?;
        }

        let temp_path = temp_file.path().to_str().unwrap();
        println!("Received file saved to: {}", temp_path);
        Ok(Response::new(FileInferenceResponse { file_path: temp_path.to_string(), ip: ip.to_string(), port: port, file_size: file_size, is_remote: is_remote}))
    }
}