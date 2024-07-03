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
        let file_size = r.file_size as i32;
        let is_remote = r.is_remote;
        let mut temp_file = NamedTempFile::new_in(env::temp_dir())?;

        let server_address = format!("{}:{}", ip, port);
        println!("Server addr: {} file size {}", server_address, file_size);
        let mut stream = TcpStream::connect(server_address)?;

        let mut buffer = [0; 1024];
        let mut total_received = 0;

        while total_received < file_size {
            let bytes_read = stream.read(&mut buffer)?;
            if bytes_read == 0 {
                break;
            }
            temp_file.write_all(&buffer[..bytes_read])?;
            total_received += bytes_read as i32;
        }

        let temp_path = temp_file.path().to_str().unwrap();
        println!("Received file saved to: {}", temp_path);
        // Ok(Response::new(FileInferenceResponse { file_path: temp_path.to_string(), ip: ip.to_string(), port: port, file_size: file_size as f32, is_remote: is_remote}))


        match inference(self.model.clone(), temp_path.to_string()).await {
            Ok(inference_result) => 
            {
                let mut file = File::open(inference_result.clone())?;
                let mut buffer = [0; 1024];
                loop {
                    let bytes_read = file.read(&mut buffer)?;
                    if bytes_read == 0 {
                        break;
                    }
                    

                    stream.write_all(&buffer[..bytes_read])?;
                }
                println!("Done");
                stream.flush()?;
                Ok(Response::new(FileInferenceResponse { file_path: inference_result.to_string(), ip: ip.to_string(), port: port, file_size: file_size as f32, is_remote: is_remote}))

            }
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