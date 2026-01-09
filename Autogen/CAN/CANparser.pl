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
    
    # SAFETY: Skip if the entry isn't a block of data (prevents "Not a HASH ref" error)
    next unless ref($entry) eq 'HASH';
    
    # SAFETY: Ensure CAN ID exists before printing
    my $can_id = $entry->{'CAN ID'};
    next unless defined $can_id;

    my $enum_name = uc($msg_name);
    $enum_name =~ s/[^A-Z0-9]/_/g;

    print $fh "    ${enum_name}_CAN_ID = $can_id,\n";
}
print $fh "} Custom_CAN_ID_t;\n\n";

# Generate signal enums for each message
for my $msg_name (sort keys %$can_defs) {
    my $entry = $can_defs->{$msg_name};
    
    # SAFETY: Skip if entry isn't a hash
    next unless ref($entry) eq 'HASH';

    # SAFETY: Check if signals exists AND is actually a list (prevents "Not an ARRAY ref" error)
    next unless exists $entry->{'signals'} 
                && ref($entry->{'signals'}) eq 'ARRAY' 
                && @{$entry->{'signals'}};

    my $msg_enum = uc($msg_name);
    $msg_enum =~ s/[^A-Z0-9]/_/g;

    print $fh "// Signals for $msg_name\n";
    print $fh "typedef enum {\n";
    for my $sig (@{$entry->{'signals'}}) {
        # SAFETY: Ensure signal is a hash and has a name
        next unless ref($sig) eq 'HASH' && defined $sig->{'name'};

        my $sig_enum = uc($sig->{'name'});
        $sig_enum =~ s/[^A-Z0-9]/_/g;

        my $start = $sig->{'bit_start'}  // 0;
        my $len   = $sig->{'bit_length'} // 1;

        print $fh "    ${msg_enum}_${sig_enum}_START = $start,\n";
        print $fh "    ${msg_enum}_${sig_enum}_LEN   = $len,\n";
    }
    print $fh "} ${msg_enum}_SIGNALS_t;\n\n";
}

print $fh "#endif // CUSTOM_CAN_ID_H\n";
close $fh;

print "Header file Custom_CAN_ID.h generated successfully.\n";