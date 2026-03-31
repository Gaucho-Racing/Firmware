#!/usr/bin/env perl
use strict;
use warnings;
use English qw(-no_match_vars);
use File::Basename;
use File::Path qw(make_path);

# --- 1. Configuration ---
my $yaml_path   = $ARGV[0] // 'Doc/GRCAN.CANdo';
my $output_path = $ARGV[1] // 'Inc/GRCAN_CUSTOM_ID.h';

if ( !-e $yaml_path ) { die "Error: $yaml_path not found.\n"; }

# --- 2. Gatekeepers (Prevents Duplicates) ---
my %seen_ids;
my %seen_names;

# --- 3. Manual Parsing Logic ---
my $content = "// Auto-generated GRCAN ID header (Manual Regex Parser)\n";
$content .= "#ifndef GRCAN_CUSTOM_ID_H\n#define GRCAN_CUSTOM_ID_H\n\n";
$content .= "typedef enum {\n";

open my $fh, '<', $yaml_path or die "Could not open $yaml_path: $OS_ERROR";

my $in_section   = 0;
my $current_name = '';

while ( my $line = <$fh> ) {
	chomp $line;

	# Identify Section (Looking for "Custom CAN ID:")
	if ( $line =~ /^Custom\s+CAN\s+ID:/i ) {
		$in_section = 1;
		next;
	}

	# Exit section if we hit a new top-level category (no leading whitespace)
	if ( $in_section && $line =~ /^\S/ && $line !~ /^Custom/i ) {
		$in_section = 0;
	}

	next unless $in_section;

	# Capture Message Name (Indented by 2 spaces)
	if ( $line =~ /^\s{2}([^:#\s][^:]+):/x ) {
		$current_name = $1;
		$current_name =~ s/^\s+|\s+$//g;    # Trim whitespace
	}

	# Capture CAN ID (Indented by 4 spaces)
	if ( $current_name && $line =~ /^\s{4}CAN\s+ID:\s*([x[:xdigit:]]+d?|\d+)/i ) {
		my $raw_id = $1;

		# Clean the ID (Convert 10d or raw hex to 0x format)
		my $clean_id = $raw_id;
		if    ( $clean_id =~ /^([[:xdigit:]]+)d$/i )                     { $clean_id = "0x" . lc($1); }
		elsif ( $clean_id =~ /^[[:xdigit:]]+$/ && $clean_id !~ /^\d+$/ ) { $clean_id = "0x" . lc($clean_id); }

		# Clean the Enum Name (Standardize to GRCAN_..._CAN_ID)
		my $clean_label = uc $current_name;
		$clean_label =~ s/[^A-Z0-9]/_/g;
		$clean_label =~ s/_+/_/g;
		$clean_label =~ s/^_|_$//g;
		my $enum_name = "${clean_label}_CAN_ID";

		# GATEKEEPER CHECK: Duplicate ID or Name
		if ( $seen_ids{$clean_id} ) {
			warn "Skipping duplicate ID $clean_id ($current_name)\n";
		}
		elsif ( $seen_names{$enum_name} ) {
			warn "Skipping duplicate Name $enum_name\n";
		}
		else {
			$content .= "    $enum_name = $clean_id,\n";
			$seen_ids{$clean_id}    = 1;
			$seen_names{$enum_name} = 1;
		}

		$current_name = '';    # Reset for next message
	}
}
close $fh;

$content .= "} GRCAN_CUSTOM_ID_t;\n\n";
$content .= "#endif // GRCAN_CUSTOM_ID_H\n";

# --- 4. Write Output ---
my $dir = dirname($output_path);
make_path($dir) if ( $dir && !-d $dir );

open my $out, '>', $output_path or die "Cannot write to $output_path: $OS_ERROR";
print {$out} $content;
close $out;

print "Successfully parsed $yaml_path and updated $output_path (No YAML::XS used).\n";

# #!/usr/bin/env perl
# use strict;
# use warnings;
# use English qw(-no_match_vars);
# use autodie qw(open close);
# use File::Basename;
# use File::Path qw(make_path);

# main();

# sub main {
# 	my $yaml_path   = $ARGV[0] // 'format.CANdo';
# 	my $output_path = $ARGV[1] // 'Custom_CAN_ID.h';

# 	if ( !-e $yaml_path ) {
# 		die "Error: $yaml_path not found.\n";
# 	}

# 	# 1. Parse the data manually (No YAML::XS dependency needed)
# 	my $can_defs = parse_custom_ids($yaml_path);

# 	# 2. Generate the C Header
# 	my $content = "// Auto-generated Custom CAN ID header\n";
# 	$content .= "#ifndef CUSTOM_CAN_ID_H\n";
# 	$content .= "#define CUSTOM_CAN_ID_H\n\n";
# 	$content .= "typedef enum {\n";

# 	# Sort keys to keep the header organized
# 	for my $msg_name ( sort keys %{$can_defs} ) {
# 		my $can_id = $can_defs->{$msg_name};

# 		# Clean the name: spaces to underscores, uppercase
# 		my $enum_name = uc $msg_name;
# 		$enum_name =~ s/[^A-Z0-9]/_/g;
# 		$enum_name =~ s/_+/_/g;
# 		$enum_name =~ s/^_|_$//g;

# 		# Format the ID
# 		my $val = $can_id;

# 		# If it looks like Hex but lacks 0x, add it
# 		if ( $val =~ /^[[:xdigit:]]+$/ && $val !~ /^[[:digit:]]+$/ ) {
# 			$val = '0x' . lc $val;
# 		}

# 		# Handle '10d' style markers
# 		elsif ( $val =~ /^([[:xdigit:]]+)d$/ ) {
# 			$val = '0x' . lc $1;
# 		}

# 		$content .= "    ${enum_name}_CAN_ID = $val,\n";
# 	}

# 	$content .= "} GRCAN_CUSTOM_ID;\n\n";
# 	$content .= "#endif // CUSTOM_CAN_ID_H\n";

# 	# 3. Write out
# 	my $dir = dirname($output_path);
# 	if ( $dir && $dir ne '.' && !-d $dir ) {
# 		make_path($dir);
# 	}

# 	open my $fh, '>', $output_path;
# 	print {$fh} $content;
# 	close $fh;

# 	print "Successfully updated $output_path\n";
# 	return;
# }

# sub parse_custom_ids {
# 	my ($path) = @_;
# 	my %defs;
# 	my $in_section  = 0;
# 	my $current_msg = '';

# 	open my $fh, '<', $path;
# 	while ( my $line = <$fh> ) {
# 		chomp $line;

# 		# Detect the start of the "Custom CAN ID" section
# 		if ( $line =~ /^Custom \s CAN \s ID:/ix ) {
# 			$in_section = 1;
# 			next;
# 		}

# 		# Stop if we hit a different top-level section
# 		if ( $in_section && $line =~ /^\S/ && $line !~ /^Custom/i ) {
# 			last;
# 		}

# 		next unless $in_section;

# 		# Match Message Name (2 spaces)
# 		if ( $line =~ /^\s{2} ([^:#\s][^:]+) :/x ) {
# 			$current_msg = $1;
# 		}

# 		# Match CAN ID (4 spaces)
# 		elsif ( $current_msg && $line =~ /^\s{4} CAN \s ID: \s* (0x[[:xdigit:]]+ | [[:xdigit:]]+d? | \d+)/ix ) {
# 			$defs{$current_msg} = $1;
# 			$current_msg = '';           # Reset after finding the ID
# 		}
# 	}
# 	close $fh;
# 	return \%defs;
# }
