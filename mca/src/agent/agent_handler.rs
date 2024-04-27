use std::{error::Error, future::pending};
use zbus::{ConnectionBuilder, dbus_interface};

use crate::agent::model_structure::IrisData;
struct DataStruct {
}

#[dbus_interface(name = "org.zbus.DataIPC")]
impl DataStruct {
    // Can be `async` as well.
    fn send_string(&mut self, data: &str) -> String {
        println!("String data has been received!. {data}");
        format!("Receive string: {}", data)
    }

    // fn send_data(&mut self, serialized: &str) -> String {
    //     let deserialized: IrisData = serde_json::from_str(&serialized).unwrap();
    //     println!("deserialized = {:?}", deserialized);
    //     format!("Receive json string")
    // }
}