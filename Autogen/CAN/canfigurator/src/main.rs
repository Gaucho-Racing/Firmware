use std::fs;
use std::io::BufWriter;
use std::path::Path;

use anyhow::{bail, Context, Result};

fn main() -> Result<()> {
    let args: Vec<String> = std::env::args().collect();

    if args.len() < 2 {
        bail!("usage: canfigurator <command> <cando-path> [options]\n  commands: bus-id, all, validate");
    }

    let command = &args[1];
    let cando_path = args.get(2).map(String::as_str).unwrap_or("Doc/GRCAN.CANdo");

    let yaml = fs::read_to_string(cando_path)
        .with_context(|| format!("could not read {}", cando_path))?;
    let cando = canfigurator::parse(&yaml)?;

    match command.as_str() {
        "bus-id" => {
            let out_path = args.get(3).map(String::as_str).unwrap_or("Inc/GRCAN_BUS_ID.h");
            let dir = Path::new(out_path).parent().unwrap();
            if !dir.as_os_str().is_empty() {
                fs::create_dir_all(dir)?;
            }
            let file = fs::File::create(out_path)?;
            let mut writer = BufWriter::new(file);
            canfigurator::emit::bus_id::emit(&cando, &mut writer)?;
            eprintln!("CANfigurator: Successfully generated {}", out_path);
        }
        _ => bail!("unknown command: {}", command),
    }

    Ok(())
}
