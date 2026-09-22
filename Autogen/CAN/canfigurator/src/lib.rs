pub mod emit;
pub mod schema;

use anyhow::{Context, Result};

use schema::CanDo;

pub fn parse(yaml: &str) -> Result<CanDo> {
    serde_yaml::from_str(yaml).context("failed to parse CANdo YAML")
}
