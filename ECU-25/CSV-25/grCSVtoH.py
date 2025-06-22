#!/usr/bin/python3

import csv
import sys
import re

# Check for correct number of command-line arguments
if len(sys.argv) != 3:
    print("Usage: python grCSVtoH.py <input_csv> <output_header>")
    sys.exit(1)

# Input and output file names from command-line arguments
input_csv = sys.argv[1]
output_header = sys.argv[2]


# Open and process the CSV file
with open(input_csv, "r", encoding='us-ascii') as csv_file:
    reader = csv.DictReader(csv_file)
    
    lockname = re.sub('[^0-9a-zA-Z]+', '_', output_header.upper())


    # Open the header file for writing
    with open(output_header, "w", encoding='utf-8') as header_file:
        header_file.write("// Auto-generated header file\n")
        header_file.write("// Define GR IDs\n\n")
        header_file.write(f"#ifndef {lockname}\n");
        header_file.write(f"#define {lockname}\n\n");

        # Iterate through each row in the CSV
        for row in reader:
            gr_id = row["GR ID"].strip()
            node = row["Node"].strip()

            # Only process rows with valid GR ID and Node values
            if gr_id and node:
                define_name = f"GR_{re.sub('[^0-9a-zA-Z]+', '_', node.upper())}"
                header_file.write(f"#define {define_name} {gr_id}\n")
        header_file.write(f"\n#endif // {lockname}\n");

print(f"Header file '{output_header}' generated successfully.")
