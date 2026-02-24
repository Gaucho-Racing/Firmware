#!/usr/bin/env perl
use strict;
use warnings;
use YAML::XS 'LoadFile';
use File::Basename;

# --- Configuration ---
my $yaml_path   = $ARGV[0] // 'format.CANdo';
my $output_path = $ARGV[1] // 'Custom_CAN_ID.h';

# 1. Load the data
if ( !-e $yaml_path ) { die "Error: $yaml_path not found.\n"; }
my $yaml     = LoadFile($yaml_path);
my $can_defs = $yaml->{'Custom CAN ID'};

# 2. Open the file in WRITE mode ('>')
# This updates the existing file by overwriting it with fresh data.
open my $fh, '>', $output_path or die "Error: Cannot open $output_path: $!";

print {$fh} "// Auto-generated Custom CAN ID header\n";
print {$fh} "#ifndef CUSTOM_CAN_ID_H\n";
print {$fh} "#define CUSTOM_CAN_ID_H\n\n";

print {$fh} "typedef enum {\n";

# Sort to keep the header organized
for my $msg_name ( sort keys %$can_defs ) {
	my $entry = $can_defs->{$msg_name};
	next unless ref($entry) eq 'HASH';

	my $can_id = $entry->{'CAN ID'};
	next unless defined $can_id;

	# Clean the name: spaces to underscores, uppercase
	my $enum_name = uc($msg_name);
	$enum_name =~ s/[^A-Z0-9]/_/g;
	$enum_name =~ s/_+/_/g;
	$enum_name =~ s/^_|_$//g;

	# Format the ID (Decimal stays decimal, Hex gets 0x)
	my $val = $can_id;
	if ( $val =~ /^[0-9A-Fa-f]+$/ && $val !~ /^\d+$/ ) {
		$val = "0x" . lc($val);    # Handles 'A16'
	}
	elsif ( $val =~ /^([0-9A-Fa-f]+)d$/ ) {
		$val = "0x" . lc($1);      # Handles '10d'
	}

	print {$fh} "    ${enum_name}_CAN_ID = $val,\n";
}

print {$fh} "} Custom_CAN_ID_t;\n\n";
print {$fh} "#endif // CUSTOM_CAN_ID_H\n";
close $fh;

print "Successfully updated $output_path\n";

# #!/usr/bin/env perl
# use strict;
# use warnings;
# use YAML::XS 'LoadFile';
# use File::Basename;
# use File::Path qw(make_path);
# my $yaml_path   = $ARGV[0] // 'format.CANdo';
# my $output_path = $ARGV[1] // 'Custom_CAN_ID.h';
# my $dir         = dirname($output_path);
# make_path($dir) if $dir && $dir ne '.' && !-d $dir;

# # Check if YAML exists to avoid confusing Perl errors
# if ( !-e $yaml_path ) {
# 	die "CANfigurator Error: Could not find YAML file at: $yaml_path\n";
# }

# # Load YAML
# my $yaml     = LoadFile($yaml_path);
# my $can_defs = $yaml->{'Custom CAN ID'};

# # Open header file using the dynamic output path
# open my $fh, '>', $output_path
#   or die "CANfigurator Error: Cannot open $output_path: $!";

# print $fh "// Auto-generated Custom CAN ID header\n";
# print $fh "#ifndef CUSTOM_CAN_ID_H\n";
# print $fh "#define CUSTOM_CAN_ID_H\n\n";

# # Generate CAN message ID enum
# print $fh "typedef enum {\n";

# for my $msg_name ( sort keys %$can_defs ) {
# 	my $entry = $can_defs->{$msg_name};

# 	# SAFETY: Only process if this is a block of data (a Hash)
# 	next unless ref($entry) eq 'HASH';

# 	# Grab the CAN ID
# 	my $can_id = $entry->{'CAN ID'};

# 	# SAFETY: Skip if 'CAN ID' is missing
# 	if ( !defined $can_id ) {
# 		warn "Warning: No CAN ID found for message '$msg_name'. Skipping.\n";
# 		next;
# 	}

# 	# Format the name for C
# 	my $enum_name = uc($msg_name);
# 	$enum_name =~ s/[^A-Z0-9]/_/g;

# 	# Write to file: NAME_CAN_ID = 0x123,
# 	print $fh "    ${enum_name}_CAN_ID = $can_id,\n";
# }

# print $fh "} Custom_CAN_ID_t;\n\n";

# print $fh "#endif // CUSTOM_CAN_ID_H\n";
# close $fh;

# print "CANfigurator: Successfully generated $output_path\n";
