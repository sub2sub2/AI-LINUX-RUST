use tokio::task::JoinSet;
use mca_package::{api::{app_base::Role, app_manager::{check_auth, AppManager}, service_base::ServiceBase, service_enum::ServiceEnum, service_manager::ServiceManager}, services::agent::iris_inference_server::IrisInferenceServer};
use mca_package::agent::{model_connection::{MCAOperations, MODEL_CONNECTION}, model_enum::MCAModelEnum};
#[tokio::main]
async fn main() {
// async fn main() -> Result<(), Box<dyn std::error::Error>> {
    {
        let mut conn = MODEL_CONNECTION.lock().await;
        (*conn).clean_registered_model();
        
        let _result = (*conn).register_model(MCAModelEnum::Model1).await;
        println!("Register resut {:?}", _result);
    }
    let mut service_manager = ServiceManager::new();

    let service_model_1 = ServiceBase::new("iris", 8080, Role::Admin, MCAModelEnum::Model1).await;

    // ServiceBase instance is successfully created, proceed with registering the service
    service_manager.register_service(ServiceEnum::IrisService(IrisInferenceServer::with_interceptor(
            service_model_1,
            check_auth,
        )));

    let mut set = JoinSet::new(); // 서비스 별로(?) 스레드 풀이다.
    for service in service_manager.get_services() {
        
        match service {
            Some(svc) => {
                // println!("{:?}", svc);
                set.spawn( // 다른 스레드로 돔
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

