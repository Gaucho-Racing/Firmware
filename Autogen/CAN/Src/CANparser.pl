#!/usr/bin/env perl
use strict;
use warnings;
use English qw(-no_match_vars);
use autodie qw(open close);
use File::Basename;
use File::Path qw(make_path);

main();

sub main {
	my $yaml_path   = $ARGV[0] // q{format.CANdo};
	my $output_path = $ARGV[1] // q{Custom_CAN_ID.h};

	if ( !-e $yaml_path ) {
		die "Error: $yaml_path not found.\n";
	}

	# 1. Parse the data manually (No YAML::XS dependency needed)
	my $can_defs = parse_custom_ids($yaml_path);

	# 2. Generate the C Header
	my $content = "// Auto-generated Custom CAN ID header\n";
	$content .= "#ifndef CUSTOM_CAN_ID_H\n";
	$content .= "#define CUSTOM_CAN_ID_H\n\n";
	$content .= "typedef enum {\n";

	# --- TRACKER FOR ISSUE #369 ---
	my %seen_enum_names;

	# Sort keys to keep the header organized
	for my $msg_name ( sort keys %{$can_defs} ) {
		my $can_id = $can_defs->{$msg_name};

		# Clean the name: spaces to underscores, uppercase
		my $enum_name = uc $msg_name;
		$enum_name =~ s/[[:^alnum:]]/_/g;
		$enum_name =~ s/_+/_/g;
		$enum_name =~ s/^_|_$//g;

		# Format the final C identifier
		my $full_enum_identifier = "${enum_name}_CAN_ID";

		# --- FIX FOR #369: Skip if this identifier already exists in the enum ---
		if ( $seen_enum_names{$full_enum_identifier} ) {
			warn "Issue #369: Skipping redefinition of $full_enum_identifier\n";
			next;
		}
		$seen_enum_names{$full_enum_identifier} = 1;

		# Format the ID
		my $val = $can_id;

		# If it looks like Hex but lacks 0x, add it
		if ( $val =~ /^[[:xdigit:]]+$/ && $val !~ /^[[:digit:]]+$/ ) {
			$val = '0x' . lc $val;
		}

		# Handle '10d' style markers
		elsif ( $val =~ /^([[:xdigit:]]+)d$/ ) {
			$val = '0x' . lc $1;
		}

		$content .= "    $full_enum_identifier = $val,\n";
	}

	$content .= "} GRCAN_CUSTOM_ID;\n\n";
	$content .= "#endif // CUSTOM_CAN_ID_H\n";

	# 3. Write out
	my $dir = dirname($output_path);
	if ( $dir && $dir ne q{.} && !-d $dir ) {
		make_path($dir);
	}

	open my $fh, '>', $output_path;
	print {$fh} $content
	  or die "Failed to write to $output_path: $OS_ERROR";
	close $fh;

	my $log_success = print "Successfully updated $output_path\n";
	if ( !$log_success ) {
		die "Failed to write to STDOUT: $OS_ERROR";
	}
	return;
}

sub parse_custom_ids {
	my ($path) = @_;
	my %defs;
	my $in_section  = 0;
	my $current_msg = q{};

	open my $fh, '<', $path;
	_process_custom_id_lines( \%defs, $fh, \$in_section, \$current_msg );
	close $fh;
	return \%defs;
}

sub _process_custom_id_lines {
	my ( $defs_ref, $fh, $in_section_ref, $current_msg_ref ) = @_;

	while ( my $line = <$fh> ) {
		chomp $line;

		# Detect the start of the "Custom CAN ID" section
		if ( $line =~ /^Custom \s CAN \s ID:/ix ) {
			${$in_section_ref} = 1;
			next;
		}

		# Stop if we hit a different top-level section
		if ( ${$in_section_ref} && $line =~ /^\S/ && $line !~ /^Custom/i ) {
			last;
		}

		if ( !${$in_section_ref} ) {
			next;
		}

		# Match Message Name (2 spaces)
		if ( $line =~ /^\s{2} ([^:#\s][^:]+) :/x ) {
			${$current_msg_ref} = $1;
		}

		# Match CAN ID (4 spaces)
		elsif ( ${$current_msg_ref} && $line =~ /^\s{4} CAN \s ID: \s* (0x[[:xdigit:]]+ | [[:xdigit:]]+d? | \d+)/ix ) {
			$defs_ref->{ ${$current_msg_ref} } = $1;
			${$current_msg_ref} = q{};    # Reset after finding the ID
		}
	}
	return;
}

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
