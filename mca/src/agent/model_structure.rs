use zbus::{Result, dbus_proxy};
use serde::{Serialize, Deserialize};

#[derive(Serialize, Deserialize, Debug)]
pub struct IrisData {
    pub col1: f32,
    pub col2: f32,
    pub col3: f32,
    pub col4: f32,
}

// Model1Trait 특성 정의
pub trait Model1Trait {
    // async fn test(&self, input: String) -> Result<String>;
    fn test(&self, input: String) -> impl std::future::Future<Output = Result<String>> + Send;
}


// Model1 구조체 정의
pub struct Model1Struct<'a>{
    pub proxy: Model1Proxy<'a>,
}

// AIModelTrait을 Model1과 Model2에 대한 특성으로 구현
impl Model1Trait for Model1Struct<'_> {
    async fn test(&self, input: String) -> Result<String> {
        println!("{:?}", input);
        let _reply = self.proxy.predict(&input).await?;
        Ok(_reply)
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

pub trait Model2Trait {
    // async fn test(&self, input: &str) -> Result<String>;
    fn test(&self, input: &str) -> impl std::future::Future<Output = Result<String>> + Send;
}

// Model2 구조체 정의
pub struct Model2Struct<'a>{
    pub proxy: Model2Proxy<'a>,
}

// AIModelTrait을 Model1과 Model2에 대한 특성으로 구현
impl Model2Trait for Model2Struct<'_> {
    async fn test(&self, input: &str) -> Result<String> {
        println!("{:?}", input);
        let _reply = self.proxy.predict(input).await?;
        Ok(_reply)
    }
}

// zbus Proxy 구조체 정의
#[dbus_proxy(
    interface = "org.mca.Model2",
    default_service = "org.mca.Model2",
    default_path = "/org/mca/Model2"
)]
pub trait Model2 {
    async fn predict(&self, input: &str) -> Result<String>;
}