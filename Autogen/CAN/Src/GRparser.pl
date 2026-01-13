#!/usr/bin/env perl
use strict;
use warnings;
use YAML::XS 'LoadFile';

my $yaml_path   = $ARGV[0] // 'format.CANdo';
my $output_path = $ARGV[1] // 'GR_IDS.h';

# Safety check for input file
if (!-e $yaml_path) {
    die "CANfigurator Error: Could not find YAML file at: $yaml_path\n";
}

# Load YAML
my $yaml = LoadFile($yaml_path);
my $gr_ids = $yaml->{"GR ID"};

# Open header file for writing using the dynamic output path
open my $fh, '>', $output_path or die "CANfigurator Error: Cannot open $output_path: $!";

print $fh "// Auto-generated GR ID enum header\n";
print $fh "#ifndef GR_IDS_H\n#define GR_IDS_H\n\n";
print $fh "typedef enum {\n";

# Loop through each device and write enum entries
for my $name (sort keys %$gr_ids) {
    my $const_name = $name;
    $const_name =~ s/[^A-Za-z0-9]/_/g;       # sanitize for C identifiers
    my $id = $gr_ids->{$name};
    
    # Check if ID exists to prevent empty enum values
    if (defined $id) {
        print $fh "    $const_name = $id,\n";
    }
}

print $fh "} GR_ID_t;\n\n";
print $fh "#endif // GR_IDS_H\n";

close $fh;

print "CANfigurator: Successfully generated $output_path\n";