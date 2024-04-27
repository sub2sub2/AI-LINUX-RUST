use tokio::task::JoinSet;
use mca_package::{api::{app_base::Role, app_manager::AppManager, service_base::ServiceBase, service_enum::ServiceEnum, service_manager::ServiceManager}, services::agent::iris_inference_server::IrisInferenceServer};

#[tokio::main]
async fn main() {
// async fn main() -> Result<(), Box<dyn std::error::Error>> {
    
    // MODEL_CONNECTION.lock().unwrap().clean_registered_model();

    // let _result: Result<String, zbus::Error> = MODEL_CONNECTION.lock().unwrap().register_model(MCAModelEnum::Model1).await;
    // println!("Register resut {:?}", _result);
    
    let app_manager = AppManager::new();

    let mut service_manager = ServiceManager::new();
    
    // service_manager.register_service(
    //     ServiceEnum::Basic(
    //         BasicServer::<BasicService>::new (
    //             BasicService::new("Basic", 50050)
    //         )
    //     )
    // );

    // XXX: How to capsulate the IrisInferenceServer
    service_manager.register_service(
        ServiceEnum::IrisService(
            IrisInferenceServer::with_interceptor(
                IrisInferenceServer::<ServiceBase>::new(
                ServiceBase::new("iris", 8080, Role::Admin )
            ), check_auth)
                
        )
    );
    let mut set = JoinSet::new();




    for service in service_manager.get_services() {
        
        match service {
            Some(svc) => {
                // println!("{:?}", svc);
                set.spawn( 
                    svc
                    // .serve(address)
                );
            }
            None => {
                panic!("invalid service");
            }
        }

        
    }
    while let Some(res) = set.join_next().await {
        
        // error handling
        let _ = res;

        // match res{
        //     Some(out) => {
        //         Ok() => 
        //     }
        // };
        // ...
    }

}