use std::io::Write;

use anyhow::Result;

use crate::schema::CanDo;

pub fn emit(cando: &CanDo, out: &mut impl Write) -> Result<()> {
    let mut buses: Vec<_> = cando.buses.iter().collect();
    buses.sort_by_key(|(_, def)| def.id);

    writeln!(out, "// Auto-generated Bus ID enum header")?;
    writeln!(out, "#ifndef GRCAN_BUS_ID_H")?;
    writeln!(out, "#define GRCAN_BUS_ID_H")?;
    writeln!(out)?;
    writeln!(out, "/** GR CAN Bus IDs */")?;
    writeln!(out, "typedef enum {{")?;

    for (name, def) in &buses {
        let const_name: String = name
            .chars()
            .map(|c| if c.is_ascii_alphanumeric() { c } else { '_' })
            .collect::<String>()
            .to_ascii_uppercase();

        if let Some(comment) = &def.comment {
            writeln!(out, "\t/** {} */", comment)?;
        }
        writeln!(out, "\tGRCAN_BUS_{} = {},", const_name, def.id)?;
    }

    writeln!(out, "}} GRCAN_BUS_ID;")?;
    writeln!(out)?;
    write!(out, "#endif // GRCAN_BUS_ID_H\n")?;

    Ok(())
}
