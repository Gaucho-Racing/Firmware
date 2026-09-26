use std::collections::BTreeMap;

use serde::Deserialize;

#[derive(Deserialize)]
pub struct CanDo {
    #[serde(rename = "Bus ID")]
    pub buses: BTreeMap<String, BusDef>,
}

#[derive(Deserialize)]
pub struct BusDef {
    pub id: u32,
    #[allow(dead_code)]
    pub max_dlc: Option<u32>,
    pub comment: Option<String>,
    #[allow(dead_code)]
    pub nodes: Option<Vec<String>>,
}
