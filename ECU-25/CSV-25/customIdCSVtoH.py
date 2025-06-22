#!/usr/bin/python3

import csv
import sys
import re

# Check for correct number of command-line arguments
if len(sys.argv) != 3:
    print("Usage: python customIdCSVtoH.py <input_csv> <output_header>")
    sys.exit(1)

# Input and output file names from command-line arguments
input_csv = sys.argv[1]
output_header = sys.argv[2]

# Open and process the CSV file
with open(input_csv, "r", encoding='utf-8') as csv_file:
    reader = csv.DictReader(csv_file)

    lockname = re.sub('[^0-9a-zA-Z]+', '_', output_header.upper())

    # Open the header file for writing
    with open(output_header, "w", encoding='utf-8') as header_file:
        header_file.write("// Auto-generated header file\n")
        header_file.write("// Define message IDs\n\n")
        header_file.write(f"#ifndef {lockname}\n");
        header_file.write(f"#define {lockname}\n\n");

        # Iterate through each row in the CSV
        for row in reader:
            msg_id = row["CAN ID"].strip()
            msg_name = row["Message Name"].strip()

            # Only process rows with valid GR ID and Node values
            if msg_id and msg_name:
                define_name = f"MSG_{re.sub('[^0-9a-zA-Z]+', '_', msg_name.upper())}"
                header_file.write(f"#define {define_name} 0x{msg_id}\n")    # Are these in hexadecimal?
        header_file.write(f"\n#endif // {lockname}\n");

print(f"Header file '{output_header}' generated successfully.")
