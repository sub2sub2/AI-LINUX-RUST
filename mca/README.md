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






  