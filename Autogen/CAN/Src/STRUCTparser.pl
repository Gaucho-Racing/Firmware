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
	my $prefix      = 'GR_OLD';

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

		# Use explicit \s{2,4} because /x ignores literal spaces
		if ( $line =~ /^\s{2,4} ([^:#\s][^:]+) :/smx ) {
			my $raw_name = $1;
			my $f_name   = clean_field_name($raw_name);
			my ( $desc, $new_i ) = extract_desc_from_array( \@lines, $i );
			if ($desc) {
				$map{$f_name} = $desc;
			}
			$i = $new_i;
		}
	}
	return %map;
}

sub extract_desc_from_array {
	my ( $lines_ref, $index ) = @_;
	my $description = $EMPTY_STR;
	my $i           = $index;

	while ( ++$i < scalar @{$lines_ref} ) {
		my $sub = ${$lines_ref}[$i];
		if ( $sub =~ /^\s+ \# \s* (.*)/smx ) {
			my $comment_text = $1;
			$description .= $SPACE_STR . $comment_text;
		}

		# Check for new field or message start
		if ( $sub =~ /^\s{2,4} [^#\s]/smx || $sub =~ /^\S/smx ) {
			last;
		}
	}

	$description =~ s/^\s+//smx;
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

			my $skip_msg = 0;
            my $j = $i + 1;
            while ( $j < scalar @{$lines_ref} ) {
                my $next_l = ${$lines_ref}[$j];
                # Stop if we hit a new message or start of a new section
                last if $next_l =~ /^\s{2} [^#\s]/smx || $next_l =~ /^\S/smx;
                if ( $next_l =~ /^\s{4} generate: \s* false/smx ) {
                    $skip_msg = 1;
                    last;
                }
                $j++;
            }

			if ($current_msg) {
				push @output, process_message( $current_msg, \@fields, $d_map, $prefix );
			}
			$current_msg = $skip_msg ? $EMPTY_STR : $msg_name;
			@fields      = ();
		}

		# Explicitly match 4 spaces for Field Name
		elsif ( $line =~ /^\s{4} ([^:#\s][^:]+) :/smx ) {
			next if $current_msg eq $EMPTY_STR;

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
	my ( $start, $data_type) = ( undef, 'u8');
	my $i = $index;

	while ( ++$i < scalar @{$lines_ref} ) {
		my $sub = ${$lines_ref}[$i];
		if ( $sub =~ /bit_start: \s* (\d+)/smx ) {
			$start = $1;
		}
		if ( $sub =~ /data \s type: \s* (\w+)/smx ) {
			$data_type = $1;
		}
		if ( $sub =~ /^\s{4} [^#\s]/smx || $sub =~ /^\s{0,2} \S/smx ) {
			last;
		}
	}
	my %res = defined $start ? ( name => $name, start => $start, type => $data_type) : ();
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

	for my $i ( 0 .. $#sorted ) {
		push @buf, process_byte_entry( \@sorted, \%byte_map, \$i, $d_map );
	}
	push @buf, "} ${prefix}_${tag}_MSG;\n\n";
	return join $EMPTY_STR, @buf;
}

sub process_byte_entry {
	my ( $sorted_ref, $map_ref, $idx_ref, $d_map ) = @_;
	my @out;
	my $b_idx  = ${$sorted_ref}[ ${$idx_ref} ];
	my $fields = ${$map_ref}{$b_idx};

	if ( scalar @{$fields} > 2 ) {
		push @out, handle_multi_field_range( $sorted_ref, $map_ref, $idx_ref );
	}
	else {
		my $f_var =
		  ( scalar @{$fields} == 1 )
		  ? clean_field_name( ${$fields}[0]->{name} )
		  : join q{_}, map { clean_field_name( $_->{name} ) } @{$fields};
		if ( $f_var =~ /^[[:digit:]]/smx ) {
			$f_var = q{_} . $f_var;
		}

		my $type =
		    ( ${$fields}[0]->{type} =~ /32/smx ) ? 'uint32_t'
		  : ( ${$fields}[0]->{type} =~ /16/smx ) ? 'uint16_t'
		  :                                        'uint8_t';
		my $desc = join $SPACE_STR, map { ${$d_map}{ clean_field_name( $_->{name} ) } // () } @{$fields};

		push @out, sprintf "\t/** %s (Byte %d) */\n\t%-10s %-30s\n", ( $desc || "Byte $b_idx" ), $b_idx, $type, $f_var . q{;};
	}
	return join $EMPTY_STR, @out;
}

sub handle_multi_field_range {
	my ( $bytes_ref, $map_ref, $idx_ref ) = @_;
	my $start_byte = ${$bytes_ref}[ ${$idx_ref} ];
	my $has_error  = grep { $_->{name} =~ /error|fault|violation/ismx } @{ ${$map_ref}{$start_byte} };

	while ( ${$idx_ref} + 1 < scalar @{$bytes_ref} ) {
		my $next_byte = ${$bytes_ref}[ ${$idx_ref} + 1 ];
		my $next_f    = ${$map_ref}{$next_byte};
		if ( scalar @{$next_f} <= 2 ) {
			my $first_name = ${$next_f}[0]->{name};
			if ( $first_name !~ /reserved/ismx ) {
				last;
			}
		}
		${$idx_ref}++;
	}

	my $len    = ( ${$bytes_ref}[ ${$idx_ref} ] - $start_byte ) + 1;
	my $v_name = $has_error   ? 'error_fault_violation_bits' : 'ping_block';
	my $suffix = ( $len > 1 ) ? "[$len]"                     : $EMPTY_STR;
	return sprintf "\tuint8_t    %s%s;\n", $v_name, $suffix;
}
