#!/usr/bin/env perl
use strict;
use warnings;
use File::Basename;
use File::Path qw(make_path);
use English    qw(-no_match_vars);
use autodie    qw(open close);

main();

sub main {
	my $yaml_path   = $ARGV[0] // 'format.CANdo';
	my $output_path = $ARGV[1] // 'GRCAN_BUS_ID.h';
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

	for my $line (@lines) {
		if ( $line =~ /^Bus[ ]ID:/smx ) {
			$in_section = 1;
			next;
		}
		last if $in_section && $line =~ /^\w/smx;
		next if !$in_section;

		if ( $line =~ /^ \s+ ([^:]+) : \s* ["']? ( [^"'\s#]+ ) ["']? \s* (?: [#] \s* (.*?) )? \s* $/smx ) {
			my $name    = $1;
			my $val     = $2;
			my $comment = $3 // q{};

			$name    =~ s/^\s+|\s+$//gsmx;
			$val     =~ s/^\s+|\s+$//gsmx;
			$comment =~ s/^\s+|\s+$//gsmx;

			push @found_ids, { name => $name, id => $val, comment => $comment };
		}
	}
	return \@found_ids;
}

sub generate_bus_header_content {
	my ($ids_ref) = @_;
	my @header_lines;

	push @header_lines, "// Auto-generated Bus ID enum header\n";
	push @header_lines, "#ifndef GRCAN_BUS_ID_H\n";
	push @header_lines, "#define GRCAN_BUS_ID_H\n\n";
	push @header_lines, "/** GR CAN Bus IDs */\n";
	push @header_lines, "typedef enum {\n";

	my @sorted = sort { $a->{id} <=> $b->{id} || $a->{name} cmp $b->{name} } @{$ids_ref};

	for my $item (@sorted) {
		my $const_name = $item->{name};
		$const_name =~ s/[[:^alnum:]]/_/gsmx;

		if ( defined $item->{id} && $item->{id} ne q{} ) {
			if ( defined $item->{comment} && $item->{comment} ne q{} ) {
				push @header_lines, sprintf "    /** %s */\n", $item->{comment};
			}
			push @header_lines, sprintf "    GRCAN_BUS_%s = %s,\n", uc $const_name, $item->{id};
		}
	}

	push @header_lines, "} GRCAN_BUS_ID;\n\n";
	push @header_lines, "#endif // GRCAN_BUS_ID_H\n";

	return @header_lines;
}

sub write_output {
	my ( $path, $lines_ref ) = @_;
	my $content = join q{}, @{$lines_ref};

	open my $out, '>', $path;
	my $success = print {$out} $content;
	if ( !$success ) {
		die "Failed to write to $path: $OS_ERROR";
	}
	close $out;

	return;
}
