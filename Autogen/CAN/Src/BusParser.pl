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
	my $output_path = $ARGV[1] // q{GR_CAN_BUS_ID.h};
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

		# 3. Match the Bus Entry: "  Name: ID"
		# Matches: 2 spaces, captures Name (everything before colon), captures ID (the number)
		if ( $line =~ /^\s{2} ([^:#\s][^:]+) : \s* (\d+) /x ) {
			my $name = $1;
			my $val  = $2;
			$name =~ s/^\s+|\s+$//g;

			my $entry = { name => $name, id => $val, comment => q{} };
			$entry->{comment} = _extract_comment_block( \@lines, $i, $entry->{comment} );

			push @found_ids, $entry;
		}
	}
	return \@found_ids;
}

sub _extract_comment_block {
	my ( $lines_ref, $index, $existing ) = @_;
	my $comment = $existing // q{};
	my $j       = $index + 1;

	while ( $j <= $#{$lines_ref} && ${$lines_ref}[$j] =~ /^\s{4,}/ ) {
		my $line = ${$lines_ref}[$j];
		if ( $line =~ /^\s{4} comment: \s* (.*) /ix ) {
			my $text = $1;
			$text =~ s/^\s+|\s+$//g;
			if ( $text ne q{} ) {
				$comment = $text;
			}
			else {
				my $k = $j + 1;
				while ( $k <= $#{$lines_ref} && ${$lines_ref}[$k] =~ /^\s{6,} (.+) /x ) {
					my $sub_text = $1;
					$sub_text =~ s/^\s+|\s+$//g;
					$comment .= ( $comment ? q{ } : q{} ) . $sub_text;
					$k++;
				}
			}
		}
		$j++;
	}
	return $comment;
}

sub generate_bus_header_content {
	my ($ids_ref) = @_;
	my @header_lines;

	push @header_lines, "// Auto-generated Bus ID enum header\n";
	push @header_lines, "#ifndef GR_CAN_BUS_ID_H\n";
	push @header_lines, "#define GR_CAN_BUS_ID_H\n\n";
	push @header_lines, "/** GR CAN Bus IDs */\n";
	push @header_lines, "typedef enum {\n";

	# Sort by ID (numeric)
	my @sorted = sort { ( $a->{id} // 0 ) <=> ( $b->{id} // 0 ) } @{$ids_ref};

	for my $item (@sorted) {
		next if $item->{id} eq q{};

		my $const_name = $item->{name};
		$const_name =~ s/[[:^alnum:]]/_/g;    # Clean name for C constant

		if ( $item->{comment} ne q{} ) {
			push @header_lines, sprintf "    /** %s */\n", $item->{comment};
		}
		push @header_lines, sprintf "    GR_CAN_BUS_%s = %s,\n", uc $const_name, $item->{id};
	}

	push @header_lines, "} GR_CAN_BUS_ID;\n\n";
	push @header_lines, "#endif // GR_CAN_BUS_ID_H\n";

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
