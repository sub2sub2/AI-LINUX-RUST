use zbus::{Error, Result, dbus_proxy};
use serde::{Serialize, Deserialize};

#[derive(Serialize, Deserialize, Debug)]
struct IrisData {
    name: String,
    age: i32,
    birthday: i32,
}

// AIModelTrait 특성 정의
pub trait AIModelTrait: Send + Sync{
    fn predict(&self, input: &str) -> Result<String>;
}

// Model1Trait 특성 정의
pub trait Model1Trait {
    fn model1_specific_method(&self) -> Result<()>;
}

// Model2Trait 특성 정의
pub trait Model2Trait {
    fn model2_specific_method(&self) -> Result<()>;
}

// AIModel 구조체 정의
pub struct AIModel {
    name: String,
}

// AIModel에 대한 메서드 구현
impl AIModel {
    pub fn new(name: &str) -> Self {
        AIModel {
            name: name.to_string(),
        }
    }
}

// Model1 구조체 정의
pub struct Model1Struct<'a>{
    pub proxy: Model1Proxy<'a>,
}

// Model1 구조체에 대한 Model1Trait 특성 구현
impl Model1Trait for Model1Struct<'_> {
    fn model1_specific_method(&self) -> Result<()> {
        // Model1에 특화된 작업을 수행
        Ok(())
    }
}

// Model2 구조체 정의
pub struct Model2Struct<'a> {
    pub proxy: Model2Proxy<'a>,
}

// Model2 구조체에 대한 Model2Trait 특성 구현
impl Model2Trait for Model2Struct<'_> {
    fn model2_specific_method(&self) -> Result<()> {
        // Model2에 특화된 작업을 수행
        Ok(())
    }
}

// AIModelTrait을 Model1과 Model2에 대한 특성으로 구현
impl AIModelTrait for Model1Struct<'_> {
    fn predict(&self, input: &str) -> Result<String> {
        println!("{:?}", input);
        let reply = self.proxy.predict(input);
        // println!("{:?}", reply.unwrap);z
        Ok("ok".to_string())
    }
}

impl AIModelTrait for Model2Struct<'_> {
    fn predict(&self, input: &str) -> Result<String> {
        let reply = self.proxy.predict(input);
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

#[dbus_proxy(
    interface = "org.mca.Model2",
    default_service = "org.mca.Model2",
    default_path = "/org/mca/Model2"
)]
pub trait Model2 {
    async fn predict(&self, input: &str) -> Result<String>;
}