pub mod services;

pub mod service_handler;
pub mod service;
pub mod service_enum;

pub mod app_handler;
pub mod app;



pub mod mca_model1 {
    tonic::include_proto!("agent");
    tonic::include_proto!("mca");
}
