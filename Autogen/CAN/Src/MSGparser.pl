#!/usr/bin/env perl
use strict;
use warnings;
use autodie qw(open close);
use File::Basename;
use File::Path qw(make_path);
use English    qw(-no_match_vars);

main();

sub main {
	my $yaml_file   = $ARGV[0] // 'format.CANdo';
	my $output_file = $ARGV[1] // 'can_msg_ids.h';

	my $dir = dirname($output_file);
	if ( $dir && $dir ne q{.} && !-d $dir ) {
		make_path($dir);
	}

	if ( !-e $yaml_file ) {
		die "CANfigurator Error: Input YAML not found at '$yaml_file'.\n";
	}

	my $msg_ids_ref = parse_msg_yaml($yaml_file);
	write_msg_header( $output_file, $msg_ids_ref );

	my $count       = scalar @{$msg_ids_ref};
	my $log_success = print "CANfigurator: Generated $output_file with $count message IDs\n";
	if ( !$log_success ) {
		die "Failed to write to STDOUT: $OS_ERROR";
	}
	return;
}

sub parse_msg_yaml {
	my ($path) = @_;

	open my $fh, '<', $path;
	my @lines = <$fh>;
	close $fh;

	my @msg_ids;
	my $i   = 0;
	my $max = $#lines;

	# Use 'while' instead of C-style 'for' to satisfy the linter
	# while allowing manual index manipulation.
	while ( $i <= $max ) {
		my $line = $lines[$i];

		if ( $line =~ /^ (Custom[ ]CAN[ ]ID | GR[ ]ID) : /smx ) {
			last;
		}

		if ( $line =~ /^ \s{2} ( \w [\w\s\d.]+ ) : \s* $/smx ) {
			my $name = $1;

			# Pass by reference so we can skip lines in the array
			my $entry = find_id_in_lines( \@lines, \$i, $name );
			if ($entry) {
				push @msg_ids, $entry;
			}
		}
		$i++;
	}
	return \@msg_ids;
}

sub find_id_in_lines {
	my ( $lines_ref, $idx_ref, $msg_name ) = @_;

	my $start = ${$idx_ref} + 1;
	my $end   = $#{$lines_ref};

	# Standard Perlish range loop satisfies ControlStructures::ProhibitCStyleForLoops
	for my $j ( $start .. $end ) {
		my $next_line = $lines_ref->[$j];

		if ( $next_line =~ /^ \s{4} MSG[ ]ID : \s* (0x[[:xdigit:]]+) /smx ) {

			# Update the outer index
			${$idx_ref} = $j;

			my $id        = $1;
			my $enum_name = 'MSG_' . uc $msg_name;
			$enum_name =~ s/\W+/_/gsmx;
			$enum_name =~ s/_+/_/gsmx;
			$enum_name =~ s/^_|_$//gsmx;

			return { name => $enum_name, id => $id };
		}

		# Break if we hit a new message block
		if ( $next_line =~ /^ \s{2} \w /smx || $next_line =~ /^ \w /smx ) {
			return;
		}
	}
	return;
}

sub write_msg_header {
	my ( $path, $msg_ids_ref ) = @_;

	my @header_lines;
	push @header_lines, "// Auto-generated CAN Message IDs\n";
	push @header_lines, "#ifndef CAN_MSG_IDS_H\n";
	push @header_lines, "#define CAN_MSG_IDS_H\n\n";
	push @header_lines, "typedef enum {\n";

	foreach my $msg ( @{$msg_ids_ref} ) {
		push @header_lines, sprintf "    %-40s = %s,\n", $msg->{name}, $msg->{id};
	}

	push @header_lines, "} GRCAN_MSG_ID;\n\n";
	push @header_lines, "#endif // CAN_MSG_IDS_H\n";

	open my $out, '>', $path;
	for my $line (@header_lines) {
		my $success = print {$out} $line;
		if ( !$success ) {
			die "Failed to write to $path: $OS_ERROR";
		}
	}
	close $out;

	return;
}
