#!/usr/bin/env perl
use strict;
use warnings;
use YAML::XS 'LoadFile';

# Load YAML
my $yaml = LoadFile('format.yaml');
my $can_defs = $yaml->{'Custom CAN ID'};

# Open header file
open my $fh, '>', 'Custom_CAN_ID.h'
    or die "Cannot open Custom_CAN_ID.h: $!";

print $fh "// Auto-generated Custom CAN ID header\n";
print $fh "#ifndef CUSTOM_CAN_ID_H\n";
print $fh "#define CUSTOM_CAN_ID_H\n\n";

# Generate CAN message ID enum
print $fh "typedef enum {\n";

for my $msg_name (sort keys %$can_defs) {
    my $entry = $can_defs->{$msg_name};
    
    # SAFETY: Only process if this is a block of data (a Hash)
    next unless ref($entry) eq 'HASH';
    
    # Grab the CAN ID
    my $can_id = $entry->{'CAN ID'};
    
    # SAFETY: Skip if 'CAN ID' is missing or empty for this specific message
    if (!defined $can_id) {
        warn "Warning: No CAN ID found for message '$msg_name'. Skipping.\n";
        next;
    }

    # Format the name for C (Upper case, replace spaces/special chars with underscores)
    my $enum_name = uc($msg_name);
    $enum_name =~ s/[^A-Z0-9]/_/g;

    # Write to file: NAME_CAN_ID = 0x123,
    print $fh "    ${enum_name}_CAN_ID = $can_id,\n";
}

print $fh "} Custom_CAN_ID_t;\n\n";

print $fh "#endif // CUSTOM_CAN_ID_H\n";
close $fh;

print "Successfully generated Custom_CAN_ID.h with Message IDs only.\n";