#!/usr/bin/perl
use strict;
use warnings;
use English qw(-no_match_vars);
use autodie qw(open close);
use File::Basename;
use File::Path qw(make_path);
use Readonly;

# Brutal-compliant constants
Readonly::Scalar my $BITS_PER_BYTE => 8;
Readonly::Scalar my $CELL_COUNT    => 32;
Readonly::Scalar my $EMPTY_STR     => q{};
Readonly::Scalar my $SPACE_STR     => q{ };

main();

sub main {
	my $yaml_path   = $ARGV[0] // 'format.CANdo';
	my $output_path = $ARGV[1] // 'CANDler.h';
	my $prefix      = 'GRCAN';

	ensure_directory_exists($output_path);

	my %desc_map = parse_descriptions($yaml_path);
	generate_header( $yaml_path, $output_path, $prefix, \%desc_map );

	return;
}

sub ensure_directory_exists {
	my ($path) = @_;
	my $dir = dirname $path;
	if ( $dir && $dir ne q{.} && !-d $dir ) {
		make_path $dir;
	}
	return;
}

sub parse_descriptions {
	my ($path) = @_;
	my %map;
	my $in_msg_section = 0;
	my $current_msg    = $EMPTY_STR;

	open my $in, '<', $path;
	my @lines = <$in>;
	close $in;

	for my $i ( 0 .. $#lines ) {
		my $line = $lines[$i];
		chomp $line;

		if ( $line =~ /^Message \s ID:/smx ) {
			$in_msg_section = 1;
			next;
		}
		if ( $line =~ /^\S/smx ) {
			$in_msg_section = 0;
		}
		if ( !$in_msg_section ) {
			next;
		}

		# Track current message name (matches "  Msg Name:")
		if ( $line =~ /^\s{2} ([^:#\s][^:]+) :$/smx ) {
			$current_msg = $1;
		}

		# Use explicit \s{2,4} because /x ignores literal spaces
		if ( $line =~ /^\s{2,4} ([^:#\s][^:]+) :/smx ) {
			my $raw_name = $1;
			my $f_name   = clean_field_name($raw_name);
			my ( $desc, $new_i ) = extract_desc_from_array( \@lines, $i );
			if ($desc) {
				my $key = $current_msg . q{::} . $f_name;
				$map{$key} = $desc;
			}
			$i = $new_i;
		}
	}
	return %map;
}

sub extract_desc_from_array {
	my ( $lines_ref, $index ) = @_;
	my $description      = q{};
	my $i                = $index;
	my $in_comment_block = 0;

	while ( ++$i < scalar @{$lines_ref} ) {
		my $sub = ${$lines_ref}[$i];

		# 1. Match the start of the comment block
		if ( $sub =~ /^\s+ comment: \s* (.*)/smx ) {
			my $text = $1;
			$in_comment_block = 1;
			if ( $text ne q{} ) {
				$description .= ( $description ? q{ } : q{} ) . $text;
			}
			next;
		}

		# 2. If we are in the block, grab lines that ARE NOT new YAML keys
		if ($in_comment_block) {

			# A YAML key usually looks like: "    units:" or "    data type:"
			# This regex says: Stop if the line starts with 4-6 spaces,
			# followed by a word, and then a colon + space/newline.
			if ( $sub =~ /^\s{4,6} \w+[\w\s]*:(\s|$)/smx ) {
				last;
			}

			# Otherwise, if it's indented text, it's part of our sentence!
			if ( $sub =~ /^\s{6,} (.+)/smx ) {
				my $line_text = $1;
				$description .= ( $description ? q{ } : q{} ) . $line_text;
				next;
			}
		}

		# 3. Global break if we hit a new message or field entirely
		if ( $sub =~ /^\s{0,4} \S/smx ) {
			last;
		}
	}

	# Clean up any trailing/leading whitespace
	$description =~ s/^\s+|\s+$//gsmx;
	return ( $description, $i - 1 );
}

sub generate_header {
	my ( $in_p, $out_p, $prefix, $d_map ) = @_;
	my $lines_ref = slurp_file($in_p);
	my @output;

	push @output, "/* Auto-generated header file */\n";
	push @output, "#ifndef ${prefix}_MESSAGES_H\n";
	push @output, "#define ${prefix}_MESSAGES_H\n\n";
	push @output, "#include <stdint.h>\n\n";

	my $in_msg_section = 0;
	my $current_msg    = $EMPTY_STR;
	my @fields         = ();

	for my $i ( 0 .. $#{$lines_ref} ) {
		my $line = ${$lines_ref}[$i];
		if ( $line =~ /^Message \s ID:/smx ) { $in_msg_section = 1; next; }
		if ( $line =~ /^\S/smx )             { $in_msg_section = 0; }
		if ( !$in_msg_section )              { next; }

		# Explicitly match 2 spaces for Message Name
		if ( $line =~ /^\s{2} ([^:#\s][^:]+) :$/smx ) {
			my $msg_name = $1;
			if ($current_msg) {
				push @output, process_message( $current_msg, \@fields, $d_map, $prefix );
			}

			# Debug payload structs are intentionally excluded.
			$current_msg = ( $msg_name =~ /^Debug(?:\s+(?:2\.0|FD))?$/ismx ) ? $EMPTY_STR : $msg_name;
			@fields      = ();
		}

		# Explicitly match 4 spaces for Field Name
		elsif ( $line =~ /^\s{4} ([^:#\s][^:]+) :/smx ) {
			my $field_raw = $1;
			my ( $f_data, $new_i ) = parse_field_details( $lines_ref, $i, $field_raw );
			if ( %{$f_data} ) {
				push @fields, $f_data;
			}
			$i = $new_i;
		}
	}

	if ($current_msg) {
		push @output, process_message( $current_msg, \@fields, $d_map, $prefix );
	}
	push @output, "#endif\n";

	write_file( $out_p, \@output );
	return;
}

sub slurp_file {
	my ($path) = @_;
	open my $fh, '<', $path;
	my @lines = <$fh>;
	close $fh;
	chomp @lines;
	return \@lines;
}

sub write_file {
	my ( $path, $content_ref ) = @_;
	open my $fh, '>', $path;
	for my $line ( @{$content_ref} ) {
		print {$fh} $line or die "Could not write to $path: $OS_ERROR";
	}
	close $fh;
	return;
}

sub parse_field_details {
	my ( $lines_ref, $index, $name ) = @_;
	my ( $start, $data_type ) = ( undef, 'u8' );
	my $i = $index;

	while ( ++$i < scalar @{$lines_ref} ) {
		my $sub = ${$lines_ref}[$i];
		if ( $sub =~ /bit[\s_]start: \s* (\d+)/smx ) {
			$start = $1;
		}
		if ( $sub =~ /data \s type: \s* (\w+)/smx ) {
			$data_type = $1;
		}
		if ( $sub =~ /^\s{4} [^#\s]/smx || $sub =~ /^\s{0,2} \S/smx ) {
			last;
		}
	}
	my %res = defined $start ? ( name => $name, start => $start, type => $data_type ) : ();
	return ( \%res, $i - 1 );
}

sub clean_field_name {
	my ($name) = @_;
	my $clean = lc $name;

	# Correctly inverted POSIX character class
	$clean =~ s/[^[:lower:][:digit:]]/_/gsmx;
	$clean =~ s/_+/_/gsmx;
	$clean =~ s/^_|_$//gsmx;
	return $clean || 'unknown_field';
}

sub process_message {
	my ( $name, $f_ref, $d_map, $prefix ) = @_;

	my @buf;
	my $tag = uc $name =~ s/[^[:alpha:][:digit:]]/_/grsmx =~ s/_+/_/grsmx =~ s/^_|_$//grsmx;

	if ( $name =~ /Cell \s Data/ismx ) {
		push @buf, sprintf "/** %s */\ntypedef struct {\n\tstruct {\n\t\tuint8_t voltage;\n\t\tuint8_t temperature;\n\t} cells[%d];\n} %s_%s_MSG;\n\n", $name, $CELL_COUNT, $prefix, $tag;
		return join $EMPTY_STR, @buf;
	}

	my %byte_map;
	for my $f ( @{$f_ref} ) {
		push @{ $byte_map{ int( $f->{start} / $BITS_PER_BYTE ) } }, $f;
	}

	push @buf, "/** $name */\ntypedef struct {\n";
	my @sorted = sort { $a <=> $b } keys %byte_map;
	my %used_field_names;

	for my $i ( 0 .. $#sorted ) {
		push @buf, process_byte_entry( $name, \@sorted, \%byte_map, \$i, $d_map, \%used_field_names );
	}
	push @buf, "} ${prefix}_${tag}_MSG;\n\n";
	return join $EMPTY_STR, @buf;
}

sub process_byte_entry {
	my ( $msg_name, $sorted_ref, $map_ref, $idx_ref, $d_map, $seen_ref ) = @_;
	my @out;
	my $b_idx  = ${$sorted_ref}[ ${$idx_ref} ];
	my $fields = ${$map_ref}{$b_idx};

	if ( scalar @{$fields} > 2 && ${$fields}[0]->{name} =~ /reserved|ping|byte/ismx ) {
		push @out, handle_multi_field_range( $sorted_ref, $map_ref, $idx_ref, $seen_ref );
	}
	else {
		# For dense semantic bytes, emit each explicit CANdo field separately.
		if ( scalar @{$fields} > 2 ) {
			for my $f ( @{$fields} ) {
				my $f_var = assign_unique_field_name( clean_field_name( $f->{name} ), $seen_ref );
				my $type =
				    ( $f->{type} =~ /32/smx ) ? 'uint32_t'
				  : ( $f->{type} =~ /16/smx ) ? 'uint16_t'
				  :                             'uint8_t';
				my $desc = ${$d_map}{ $msg_name . q{::} . clean_field_name( $f->{name} ) } // "Byte $b_idx";
				push @out, sprintf "\t/** %s (Byte %d) */\n\t%-10s %-30s\n", $desc, $b_idx, $type, $f_var . q{;};
			}
		}
		else {
			my $f_var =
			  ( scalar @{$fields} == 1 )
			  ? clean_field_name( ${$fields}[0]->{name} )
			  : join q{_}, map { clean_field_name( $_->{name} ) } @{$fields};
			$f_var = assign_unique_field_name( $f_var, $seen_ref );

			my $type =
			    ( ${$fields}[0]->{type} =~ /32/smx ) ? 'uint32_t'
			  : ( ${$fields}[0]->{type} =~ /16/smx ) ? 'uint16_t'
			  :                                        'uint8_t';
			my $desc = join $SPACE_STR, map { ${$d_map}{ $msg_name . q{::} . clean_field_name( $_->{name} ) } // () } @{$fields};

			push @out, sprintf "\t/** %s (Byte %d) */\n\t%-10s %-30s\n", ( $desc || "Byte $b_idx" ), $b_idx, $type, $f_var . q{;};
		}
	}
	return join $EMPTY_STR, @out;
}

sub handle_multi_field_range {
	my ( $bytes_ref, $map_ref, $idx_ref, $seen_ref ) = @_;
	my $start_byte = ${$bytes_ref}[ ${$idx_ref} ];
	my $has_error  = grep { $_->{name} =~ /error|fault|violation/ismx } @{ ${$map_ref}{$start_byte} };

	while ( ${$idx_ref} + 1 < scalar @{$bytes_ref} ) {
		my $next_byte = ${$bytes_ref}[ ${$idx_ref} + 1 ];
		my $next_f    = ${$map_ref}{$next_byte};
		if ( scalar @{$next_f} > 2 || ${$next_f}[0]->{name} =~ /reserved/ismx ) {
			${$idx_ref}++;
		}
		else {
			last;
		}
	}

	my $len    = ( ${$bytes_ref}[ ${$idx_ref} ] - $start_byte ) + 1;
	my $v_name = $has_error ? 'error_fault_violation_bits' : 'ping_block';
	$v_name = assign_unique_field_name( $v_name, $seen_ref );
	my $suffix = ( $len > 1 ) ? "[$len]" : $EMPTY_STR;
	return sprintf "\tuint8_t    %s%s;\n", $v_name, $suffix;
}

sub assign_unique_field_name {
	my ( $base_name, $seen_ref ) = @_;
	my $name = $base_name // 'unknown_field';
	if ( $name =~ /^[[:digit:]]/smx ) {
		$name = q{_} . $name;
	}
	if ( !$seen_ref->{$name} ) {
		$seen_ref->{$name} = 1;
		return $name;
	}
	my $idx = 1;
	while ( $seen_ref->{ $name . q{_} . $idx } ) {
		$idx++;
	}
	my $unique = $name . q{_} . $idx;
	$seen_ref->{$unique} = 1;
	return $unique;
}
