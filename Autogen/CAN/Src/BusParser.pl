#!/usr/bin/env perl
use strict;
use warnings;
use File::Basename;
use File::Path qw(make_path);
use English    qw(-no_match_vars);
use autodie    qw(open close);

main();

sub main {
	my $yaml_path   = $ARGV[0] // q{format.CANdo};
	my $output_path = $ARGV[1] // q{GRCAN_BUS_ID.h};
	my $dir         = dirname($output_path);

	if ( $dir && $dir ne q{.} && !-d $dir ) {
		make_path($dir);
	}

	if ( !-e $yaml_path ) {
		die "CANfigurator Error: Could not find YAML file at: $yaml_path\n";
	}

	my $bus_ids_ref  = parse_bus_ids($yaml_path);
	my @header_lines = generate_bus_header_content($bus_ids_ref);
	write_output( $output_path, \@header_lines );

	my $log_success = print "CANfigurator: Successfully generated $output_path\n";
	if ( !$log_success ) {
		die "Failed to write to STDOUT: $OS_ERROR";
	}
	return;
}

sub parse_bus_ids {
	my ($path) = @_;

	open my $fh, '<', $path;
	my @lines = <$fh>;
	close $fh;

	my @found_ids;
	my $in_section = 0;

	for my $i ( 0 .. $#lines ) {
		my $line = $lines[$i];
		chomp $line;

		# 1. Detect the "Bus ID:" section header (flexible spacing/case)
		if ( $line =~ /^Bus\s*ID\s*:/ix ) {
			$in_section = 1;
			next;
		}

		# 2. Stop if we hit a new top-level section (0 indentation)
		last if $in_section && $line =~ /^\S/ && $line !~ /^Bus/i;
		next if !$in_section;

		# 3. Match a bus name at indent 2 with no inline value: "  Name:"
		if ( $line =~ /^\s{2} ([^:\#\s][^:]*) : \s* $/x ) {
			my $name = $1;
			$name =~ s/^\s+|\s+$//g;

			my $entry = { name => $name, id => q{}, comment => q{} };
			_populate_bus_children( \@lines, $i, $entry );

			push @found_ids, $entry;
		}
	}
	return \@found_ids;
}

sub _populate_bus_children {
	my ( $lines_ref, $index, $entry ) = @_;
	my $j = $index + 1;

	while ( $j <= $#{$lines_ref} && ${$lines_ref}[$j] =~ /^\s{4,}/ ) {
		my $line = ${$lines_ref}[$j];
		chomp $line;

		if ( $line =~ /^\s{4} id: \s* (\d+) /x ) {
			$entry->{id} = $1;
		}
		elsif ( $line =~ /^\s{4} max_dlc: \s* (\d+) /x ) {
			$entry->{max_dlc} = $1;
		}
		elsif ( $line =~ /^\s{4} comment: \s* (.*) /ix ) {
			my $text = $1;
			$text =~ s/^\s+|\s+$//g;
			if ( $text ne q{} ) {
				$entry->{comment} = $text;
			}
			else {
				my $k = $j + 1;
				while ( $k <= $#{$lines_ref} && ${$lines_ref}[$k] =~ /^\s{6,} (.+) /x ) {
					my $sub_text = $1;
					$sub_text =~ s/^\s+|\s+$//g;
					$entry->{comment} .= ( $entry->{comment} ? q{ } : q{} ) . $sub_text;
					$k++;
				}
				$j = $k;
				next;
			}
		}
		$j++;
	}
	return;
}

sub generate_bus_header_content {
	my ($ids_ref) = @_;
	my @header_lines;

	push @header_lines, "// Auto-generated Bus ID enum header\n";
	push @header_lines, "#ifndef GRCAN_BUS_ID_H\n";
	push @header_lines, "#define GRCAN_BUS_ID_H\n\n";
	push @header_lines, "/** GR CAN Bus IDs */\n";
	push @header_lines, "typedef enum {\n";

	# Sort by ID (numeric)
	my @sorted = sort { ( $a->{id} // 0 ) <=> ( $b->{id} // 0 ) } @{$ids_ref};

	for my $item (@sorted) {
		next if $item->{id} eq q{};

		my $const_name = $item->{name};
		$const_name =~ s/[[:^alnum:]]/_/g;    # Clean name for C constant

		if ( $item->{comment} ne q{} ) {
			push @header_lines, sprintf "\t/** %s */\n", $item->{comment};
		}
		push @header_lines, sprintf "\tGRCAN_BUS_%s = %s,\n", uc $const_name, $item->{id};
	}

	push @header_lines, "} GRCAN_BUS_ID;\n\n";
	push @header_lines, "#endif // GRCAN_BUS_ID_H\n";

	return @header_lines;
}

sub write_output {
	my ( $path, $lines_ref ) = @_;
	my $content = join q{}, @{$lines_ref};

	open my $out, '>', $path;
	print {$out} $content
	  or die "Failed to write to $path: $OS_ERROR";
	close $out;

	return;
}
