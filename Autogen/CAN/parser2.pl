#!/usr/bin/env perl
use strict;
use warnings;
use YAML::XS 'LoadFile';

my $yaml = LoadFile('format.yaml');
my $can_defs = $yaml->{'Custom CAN ID'};

die "ERROR: No CAN messages found\n"
    unless defined $can_defs && ref $can_defs eq 'HASH' && keys %$can_defs;

open my $fh, '>', 'Custom_CAN_ID.h'
    or die "Cannot open Custom_CAN_ID.h: $!";

print $fh "// Auto-generated Custom CAN ID header\n";
print $fh "#ifndef CUSTOM_CAN_ID_H\n";
print $fh "#define CUSTOM_CAN_ID_H\n\n";

print $fh "typedef enum {\n";

for my $msg_name (sort keys %$can_defs) {
    my $entry = $can_defs->{$msg_name};

    my $enum_name = uc($msg_name);
    $enum_name =~ s/[^A-Z0-9]/_/g;

    my $can_id = $entry->{'CAN ID'};
    print $fh "    ${enum_name}_CAN_ID = $can_id,\n";
}

print $fh "} CUSTOM_CAN_ID_t;\n\n";
print $fh "#endif // CUSTOM_CAN_ID_H\n";

close $fh;

print "Header file Custom_CAN_ID.h generated successfully.\n";

