#!/usr/bin/env perl
use strict;
use warnings;
use YAML::XS 'LoadFile';

# Load YAML
my $yaml = LoadFile('format.yaml');
my $can_defs = $yaml->{'CAN ID'};

# Open header file
open my $fh, '>', 'Custom_CAN_ID.h'
    or die "Cannot open Custom_CAN_ID.h: $!";

print $fh "// Auto-generated Custom CAN ID header\n";
print $fh "#ifndef CUSTOM_CAN_IDS_H\n";
print $fh "#define CUSTOM_CAN_IDS_H\n\n";

print $fh "typedef enum {\n";

# Sort for deterministic output
for my $msg_name (sort keys %$can_defs) {
    my $entry = $can_defs->{$msg_name};

    my $enum_name = uc($msg_name);
    $enum_name =~ s/[^A-Z0-9]/_/g;

    my $can_id = $entry->{'CAN ID'};

    print $fh "    ${enum_name}_CAN_ID = $can_id,\n";
}

print $fh "} CUSTOM_CAN_ID_t;\n\n";

# Emit DLC defines
for my $msg_name (sort keys %$can_defs) {
    my $entry = $can_defs->{$msg_name};

    my $macro_name = uc($msg_name);
    $macro_name =~ s/[^A-Z0-9]/_/g;

    my $dlc = $entry->{'Length'};

    print $fh "#define ${macro_name}_DLC $dlc\n";
}

print $fh "\n#endif // Custom_CAN_IDS_H\n";

close $fh;

print "Header file Custom_CAN_ID.h generated successfully.\n";
