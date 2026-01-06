use strict;
use warnings;
use YAML::XS 'LoadFile';

# Load YAML
my $yaml = LoadFile('format.yaml');
my $gr_ids = $yaml->{"GR ID"};

# Open header file for writing
open my $fh, '>', 'GR_IDS.h' or die "Cannot open GR_IDS.h: $!";

print $fh "// Auto-generated GR ID enum header\n";
print $fh "#ifndef GR_IDS_H\n#define GR_IDS_H\n\n";
print $fh "typedef enum {\n";

# Loop through each device and write enum entries
for my $name (sort keys %$gr_ids) {
    my $const_name = $name;
    $const_name =~ s/[^A-Za-z0-9]/_/g;       # sanitize for C identifiers
    my $id = $gr_ids->{$name};
    print $fh "    $const_name = $id,\n";
}

print $fh "} GR_ID_t;\n\n";
print $fh "#endif // GR_IDS_H\n";

close $fh;

print "Header file GR_IDS.h generated successfully.\n";

