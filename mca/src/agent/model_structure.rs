use async_trait::async_trait;
use serde::{Serialize, Deserialize};
use zbus::{dbus_proxy, Result};

#[derive(Serialize, Deserialize, Debug)]
pub struct IrisData {
    pub col1: f32,
    pub col2: f32,
    pub col3: f32,
    pub col4: f32,
}

#[async_trait]
pub trait Model1Trait {
    async fn test(&self, input: String) -> Result<String>;
}

// Model1Struct definition
pub struct Model1Struct<'a> {
    pub proxy: Model1Proxy<'a>,
}

#[async_trait]
impl Model1Trait for Model1Struct<'_> {
    async fn test(&self, input: String) -> Result<String> {
        println!("{:?}", input);
        let reply = self.proxy.predict(&input).await?;
        Ok(reply)
    }
}

// Define the zbus Proxy for Model1
#[dbus_proxy(
    interface = "org.mca.Model1",
    default_service = "org.mca.Model1",
    default_path = "/org/mca/Model1"
)]
pub trait Model1 {
    async fn predict(&self, input: &str) -> Result<String>;
}

#[async_trait]
pub trait Model2Trait {
    async fn test(&self, input: &str) -> Result<String>;
}

// Model2Struct definition
pub struct Model2Struct<'a> {
    pub proxy: Model2Proxy<'a>,
}

#[async_trait]
impl Model2Trait for Model2Struct<'_> {
    async fn test(&self, input: &str) -> Result<String> {
        println!("{:?}", input);
        let reply = self.proxy.predict(input).await?;
        Ok(reply)
    }
}

// Define the zbus Proxy for Model2
#[dbus_proxy(
    interface = "org.mca.Model2",
    default_service = "org.mca.Model2",
    default_path = "/org/mca/Model2"
)]
pub trait Model2 {
    async fn predict(&self, input: &str) -> Result<String>;
}
