### How to test MCA <-> AI model
1. Please enter the following commands 
```
$cd mca
$cargo build
$cargo run --bin mca-agent-test
r1: register model1
r2: register model2
s1: request inference to model1
s2: request inference to model2
q: quit
```
2. Run AI model python files in different shells
$cd model
$python3 ./model/model_1.py
$python3 ./model_model_2.py

3. Register model and request inference (Check the shells of the AI models also)
```
r1: register model1
r2: register model2
s1: request inference to model1
s2: request inference to model2
q: quit
r1
Register Model1 
Register resut Ok("success")
-----------------------------
r1: register model1
r2: register model2
s1: request inference to model1
s2: request inference to model2
q: quit
s1
Send Iris data to Model1
"{\"col1\":3.2,\"col2\":3.2,\"col3\":3.2,\"col4\":3.2}"
Output is [2]
-----------------------------
r1: register model1
r2: register model2
s1: request inference to model1
s2: request inference to model2
q: quit
```






# Rust APIs

## Build


First, build the Cargo project with the following command:
```bash
$ cd AI-LINUX-RUST/mca
$ cargo build
```

## Run

After the build, run the project with the following command:
```bash
$ cargo run --bin mca-app-test
```

Then, you can run the client app as follows:

(You are suppoosed to build app, before)

```bash
$ cd AI-LINUX-RUST/app/build
$ ./MyExecutalbe

```



# (24.4.27) Comment for the async issue 

So far, we had hard time with the following error:
...
future created by async block is not `Send`

= help: within `{async block@src/services/iris.rs:15:122: 49:6}`, the trait `std::marker::Send` is not implemented for `std::sync::MutexGuard<'_, ModelConnection>`
So I guess the issue has occurred from accessing the singleton-based struct ModelConnection.

When a thread accesses the struct ModelConnection (In our case, a thread is for the gRPC function inference),
the struct must implement the traits Sync and Send traits (to my understanding).
the trait Sync and Send is for moving the property to another thread safely.

Yes, you already implemented them for ModelConnection,
but, for some unknown reason, the ModelConnection is called the type MutexGuard<'_, ModelConnection>.
Therefore, the error indicates we don't implement the trait Send.

To deal with it temporarily, I call Model1Struct not via ModelConnection in the method inference as follows:

async fn inference(
    sepal_length: f32, sepal_width: f32, petal_length: f32, petal_width: f32) 

-> Result<String, Error>  
{
 
    // =================================
    // FIXME: replace this part with `ModelConnection`
    let connection = Connection::session().await?;

    let proxy = Model1Proxy::new(&connection).await?;
    let model = Model1Struct{proxy};
    // =================================

    let _data = IrisData{col1:sepal_length, col2:sepal_width ,col3:petal_length ,col4:petal_width};
    let serialized = serde_json::to_string(&_data).unwrap();

    let reply = model.test(serialized.to_string()).await;
    reply
    
}