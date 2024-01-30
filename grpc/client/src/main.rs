use std::io::stdin;
use agent::{IrisInferenceRequest, iris_inference_client::IrisInferenceClient};

pub mod agent{
    tonic::include_proto!("agent");
}





#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let mut client = IrisInferenceClient::connect("http://[::1]:8080").await?;
    let mut id: i32 = 0;

    loop {
        
        println!("#{}", id);
        
        println! ("\nPlease input features for the model, 4 floats, space delimiter ");
        let mut u = String::new();
        // let mut vote = String::new();

        println!("(wrong input: 2.3 4.5 6.7 8.9 ): ");
        stdin().read_line(&mut u).unwrap();
        let u = u.trim();
        // let u = u.split(" ");

        let mut features: Vec<&str> = u.split(" ").collect();
        
        if features.len() != 4 {
            println!("Got wrong input, so send (2.3 4.5 6.7 8.9)\n");
            features = "2.3 4.5 6.7 8.9".split(" ").collect();
        }

        
        let request = tonic::Request::new (IrisInferenceRequest {
            id: id.to_string(),
            sepal_length: features[0].parse::<f32>().unwrap(),
            sepal_width: features[1].parse::<f32>().unwrap(),
            petal_length: features[2].parse::<f32>().unwrap(),
            petal_width: features[3].parse::<f32>().unwrap(),
        });
        let response = client.inference(request).await?;
        println!("Got: '{}' from service\n", response.into_inner().species);
    
    
        id += 1;
    
        
    }

    Ok(())
}