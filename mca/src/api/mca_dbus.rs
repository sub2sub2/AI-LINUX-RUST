use std::sync::Mutex;
use std::sync::Arc;
use zbus::{Connection, Result, dbus_proxy};
use lazy_static::lazy_static;
use std::io::{stdout, Write};

#[dbus_proxy(
    interface = "org.zbus.DataIPC",
    default_service = "org.zbus.DataIPC",
    default_path = "/org/zbus/DataIPC"
)]
trait MCADBUS {
    // send_string 메서드 구현
    async fn send_string(&self, name: &str) -> Result<String>;

    // send_data 메서드 구현
    async fn send_data(&self, data: &str) -> Result<String>;
}


pub struct DBusHandler<'a> {
    proxy: MCADBUSProxy<'a>
}


impl<'a> DBusHandler<'a> {
    async fn new() -> Result<Self> {
        let conn = Connection::session().await?;
        let proxy: MCADBUSProxy<'a> = MCADBUSProxy::new(&conn).await?;
        let mut lock = stdout().lock();
        writeln!(lock, "set_sender").unwrap();
        Ok(DBusHandler { proxy})

    }

    pub async fn send_string(&self, name: &str) -> Result<String> {
        let reply = self.proxy.send_string(name).await?;
        Ok(reply)
    }

}
lazy_static! {
    pub static ref DBUS_HANDLER: Arc<Mutex<DBusHandler<'static>>> = {
        let handler = tokio::runtime::Runtime::new().unwrap().block_on(DBusHandler::new()).expect("Failed to create D-Bus handler");
        Arc::new(Mutex::new(handler))
    };
}