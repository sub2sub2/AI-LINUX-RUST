fn main () -> Result<(), Box<dyn std::error::Error>> {
    tonic_build::compile_protos("../protos/agent.proto")?;
    tonic_build::compile_protos("../protos/mca.proto")?;
    Ok(())
  }
