import csv
import re

def clean(s):
    """Aggressively cleans text to prevent YAML structural errors."""
    if not s: return ""
    return " ".join(str(s).replace('\n', ' ').replace('\r', ' ').replace('\\', '').replace('"', "'").split()).strip()

def to_num(val):
    """Converts values to numbers/hex; handles empty cells as 0."""
    s = str(val).strip().lower()
    if not s or s == 'nan': return 0
    # Handle hex strings (e.g., 'a16' or '10d')
    if re.search(r'[a-f]', s):
        s_hex = s.rstrip('h').rstrip('d')
        return f"0x{s_hex}" if not s_hex.startswith('0x') else s_hex
    try:
        return int(float(s))
    except:
        return s

def generate_can_yaml(input_csv, output_yaml):
    messages = []
    current_msg = None

    try:
        with open(input_csv, mode='r', encoding='utf-8-sig') as f:
            reader = csv.DictReader(f)
            for row in reader:
                msg_name = row.get('Message Name')
                if msg_name and msg_name.strip():
                    if current_msg:
                        messages.append(current_msg)
                    current_msg = {
                        'name': clean(msg_name),
                        'id': to_num(row.get('CAN ID')),
                        'len': to_num(row.get('Length')),
                        'sigs': []
                    }
                
                sig_name = row.get('Data Name')
                if sig_name and sig_name.strip() and current_msg:
                    current_msg['sigs'].append({
                        'name': clean(sig_name),
                        'bit': to_num(row.get('Bit Start'))
                    })
            if current_msg:
                messages.append(current_msg)

        # WRITE THE SNIPPET (No Tabs, 2-space parent indent)
        with open(output_yaml, 'w', encoding='utf-8') as out:
            for m in messages:
                # Top level of snippet (2 spaces)
                out.write(f"  \"{m['name']}\":\n")
                # Properties (4 spaces)
                out.write(f"    'CAN ID': {m['id']}\n")
                out.write(f"    'Length': {m['len']}\n")
                out.write(f"    'signals':\n")
                # List items (6 spaces)
                for s in m['sigs']:
                    out.write(f"      - name: \"{s['name']}\"\n")
                    out.write(f"        bit_start: {s['bit']}\n")
                out.write("\n")
        
        # DONE MESSAGE
        print("\n" + "="*40)
        print(" DONE: 'formatted_can_ids.yaml' generated!")
        print(" You can now copy the contents and paste them.")
        print("="*40 + "\n")

    except FileNotFoundError:
        print(f" ERROR: Could not find '{input_csv}'. Make sure it is in this folder.")
    except Exception as e:
        print(f" ERROR: {e}")

# EXECUTE
generate_can_yaml('GR24_25 CAN ID Assignments - GR25 Custom CAN ID.csv', 'formatted_can_ids.yaml')