use zbus::{Result, dbus_proxy, Error};
use serde::{Serialize, Deserialize};

#[derive(Serialize, Deserialize, Debug)]
struct IrisData {
    name: String,
    age: i32,
    birthday: i32,
}

// Model1Trait 특성 정의
pub trait Model1Trait {
    async fn test(&self, input: &str) -> Result<String>;
}

// Model1 구조체 정의
pub struct Model1Struct<'a>{
    pub proxy: Model1Proxy<'a>,
}

// AIModelTrait을 Model1과 Model2에 대한 특성으로 구현
impl Model1Trait for Model1Struct<'_> {
    async fn test(&self, input: &str) -> Result<String> {
        println!("{:?}", input);
        let _reply = self.proxy.predict(input).await?;
        Ok("ok".to_string())
    }
}

// zbus Proxy 구조체 정의
#[dbus_proxy(
    interface = "org.mca.Model1",
    default_service = "org.mca.Model1",
    default_path = "/org/mca/Model1"
)]
pub trait Model1 {
    async fn predict(&self, input: &str) -> Result<String>;
}
