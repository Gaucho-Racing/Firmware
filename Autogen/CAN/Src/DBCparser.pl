#!/usr/bin/env perl
use strict;
use warnings;
use autodie qw(open close);
use English qw(-no_match_vars);
use Readonly;

# --- Constants ---
Readonly::Scalar my $BITS_PER_BYTE         => 8;
Readonly::Scalar my $SHIFT_MSG             => 8;
Readonly::Scalar my $SHIFT_SENDER          => 16;
Readonly::Scalar my $EXTENDED_ID_THRESHOLD => 0x7FF;
Readonly::Scalar my $EXTENDED_ID_MASK      => 2_147_483_648;
Readonly::Scalar my $EMPTY_STR             => q{};
Readonly::Scalar my $SPACE_STR             => q{ };
Readonly::Scalar my $UNDERSCORE            => q{_};
Readonly::Scalar my $HYPHEN                => q{-};

Readonly::Hash my %TYPE_BITS => (
	'b'      => 1,
	'u4'     => 4,
	's4'     => 4,
	'u8'     => 8,
	's8'     => 8,
	'i8'     => 8,
	'u16'    => 16,
	's16'    => 16,
	'i16'    => 16,
	'u32'    => 32,
	's32'    => 32,
	'i32'    => 32,
	'u64'    => 64,
	's64'    => 64,
	'i64'    => 64,
	'double' => 64,
	's'      => 64,
);

main();

# --- Main Execution ---
sub main {
	my $input_file = $ARGV[0];

	if ( !$input_file ) {
		die "Usage: perl script.pl <input.yaml> [output.dbc]\n";
	}

	my $output_file = $ARGV[1] // 'output.dbc';

	my $lines_ref = slurp_file($input_file);
	my $data_ref  = parse_input($lines_ref);

	my @output_lines;
	push @output_lines, 'VERSION ""' . "\n\n" . 'NS_ :' . "\n\n" . 'BS_:' . "\n\n" . 'BU_: ';

	my @normalized_nodes = sort grep { $_ ne 'ALL' } map { normalize($_) } keys %{ $data_ref->{grid} };
	my $nodes = join $SPACE_STR, @normalized_nodes;

	push @output_lines, $nodes . " ALL\n\n";

	for my $route ( @{ $data_ref->{routing} } ) {
		my $msg_out = get_dbc_message( $route, $data_ref );
		if ( $msg_out ne $EMPTY_STR ) {
			push @output_lines, $msg_out;
		}
	}

	write_file( $output_file, \@output_lines );

	my $log_success = print "Successfully generated $output_file\n";
	if ( !$log_success ) {
		die "Failed to write to stdout: $OS_ERROR\n";
	}

	return;
}

# --- File IO Subroutines ---
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
		my $print_success = print {$fh} $line;
		if ( !$print_success ) {
			die "Could not write to $path: $OS_ERROR";
		}
	}
	close $fh;
	return;
}

# --- DBC Generation Subroutines ---
sub get_dbc_message {
	my ( $r_ref, $d_ref ) = @_;
	my $m_name = $r_ref->{msg};

	my $is_custom = exists $d_ref->{custom}{$m_name};
	my $m_def     = $is_custom ? $d_ref->{custom}{$m_name} : $d_ref->{messages}{$m_name};

	if ( !$m_def ) {
		return $EMPTY_STR;
	}

	my $can_id = calculate_can_id( $r_ref, $d_ref, $m_def, $is_custom );

	my $s_norm = normalize( $r_ref->{sender} );
	my $m_norm = normalize($m_name);
	my $t_norm = normalize( $r_ref->{target} );

	my $dbc_msg_name = $s_norm . $UNDERSCORE . $m_norm . '_to_' . $t_norm;
	my $msg_len      = $m_def->{len} // $BITS_PER_BYTE;

	my $output = sprintf "BO_ %u %s: %d %s\n", $can_id, $dbc_msg_name, $msg_len, $s_norm;

	my @sigs;
	if ($is_custom) {
		@sigs = @{ $m_def->{sigs} };
	}
	else {
		@sigs = map { { name => $_, %{ $m_def->{sigs}{$_} } } } keys %{ $m_def->{sigs} };
	}

	my @sorted_sigs = sort { ( $a->{start} // 0 ) <=> ( $b->{start} // 0 ) || normalize( $a->{name} ) cmp normalize( $b->{name} ) } @sigs;

	for my $s_ref (@sorted_sigs) {
		$output .= format_signal( $s_ref, $t_norm );
	}

	$output .= "\n";
	return $output;
}

sub calculate_can_id {
	my ( $r_ref, $d_ref, $m_def, $is_custom ) = @_;
	my $can_id;

	if ( $r_ref->{override} ) {
		$can_id = ( $r_ref->{override} =~ /0x/ismx ) ? hex $r_ref->{override} : int $r_ref->{override};
	}
	elsif ($is_custom) {
		$can_id = ( $m_def->{id} =~ /0x/ismx ) ? hex $m_def->{id} : int $m_def->{id};
	}
	else {
		my $s_val = $d_ref->{grid}{ $r_ref->{sender} } // '0x00';
		my $t_val = $d_ref->{grid}{ $r_ref->{target} } // '0x00';
		my $m_val = $m_def->{id}                       // '0x00';

		my $s_hex = hex $s_val;
		my $m_hex = hex $m_val;
		my $t_hex = hex $t_val;

		$can_id = ( $s_hex << $SHIFT_SENDER ) | ( $m_hex << $SHIFT_MSG ) | $t_hex;
	}

	if ( $can_id > $EXTENDED_ID_THRESHOLD ) {
		$can_id |= $EXTENDED_ID_MASK;
	}

	return $can_id;
}

sub format_signal {
	my ( $s_ref, $t_norm ) = @_;

	if ( $s_ref->{name} =~ /Reserved/ismx ) {
		return $EMPTY_STR;
	}

	my ( $factor, $offset ) = parse_eq( $s_ref->{'map equation'} );
	my $raw_type = $s_ref->{'data type'} // 'u8';
	my $bits;

	if ( $raw_type =~ /^ (\w+) \[ (\d+) \] $/smx ) {
		$bits = ( $TYPE_BITS{$1} // $BITS_PER_BYTE ) * $2;
	}
	else {
		$bits = $TYPE_BITS{$raw_type} // $BITS_PER_BYTE;
	}

	my $unit = ( $s_ref->{units} && $s_ref->{units} ne $HYPHEN && $s_ref->{units} ne 'Enum' ) ? $s_ref->{units} : $EMPTY_STR;
	$unit =~ s/'//gsmx;

	my $sign      = ( $raw_type =~ /^[is]/smx ) ? $HYPHEN : q{+};
	my $s_min     = $s_ref->{'scaled min'} // 0;
	my $s_max     = $s_ref->{'scaled max'} // 0;
	my $start_bit = $s_ref->{start}        // 0;

	return sprintf " SG_ %s : %d|%d\@1%s (%g,%g) [%s|%s] \"%s\" %s\n", normalize( $s_ref->{name} ), $start_bit, $bits, $sign, $factor, $offset, $s_min, $s_max, $unit, $t_norm;
}

# --- Helper Subroutines ---
sub normalize {
	my ($val) = @_;

	if ( !defined $val || $val eq $EMPTY_STR ) {
		return 'Unknown';
	}

	$val =~ s/^\s+|\s+$//gsmx;
	$val =~ s/[\s.-]+/_/gsmx;
	$val =~ s/\W//gsmx;
	return $val;
}

sub parse_eq {
	my ($eq) = @_;
	if ( !defined $eq || $eq eq $HYPHEN ) {
		return ( 1, 0 );
	}

	$eq =~ s/["']//gsmx;
	$eq =~ s/\s+//gsmx;    # Strip spaces to guarantee math regexes trigger perfectly

	if ( $eq =~ /^ (\d+) x \/ (\d+) $/smx )        { return ( $1 / $2, 0 ); }
	if ( $eq =~ /^ x \/ ([\d.]+) $/smx )           { return ( 1 / $1,  0 ); }
	if ( $eq =~ /^ abs[(]x[)] \/ ([\d.]+) $/smx )  { return ( 1 / $1,  0 ); }
	if ( $eq =~ /^ ([\d.]+) x $/smx )              { return ( $1 + 0,  0 ); }
	if ( $eq =~ /^ ([\d.]+) x [+] ([\d.]+) $/smx ) { return ( $1 + 0,  $2 + 0 ); }
	if ( $eq =~ /^ ([\d.]+) x - ([\d.]+) $/smx )   { return ( $1 + 0,  -( $2 + 0 ) ); }
	if ( $eq =~ /^ x - ([\d.]+) $/smx )            { return ( 1,       -( $1 + 0 ) ); }

	return ( 1, 0 );
}

# --- Parsing Engine ---
sub parse_input {
	my ($lines_ref) = @_;
	my %data = (
		routing  => [],
		messages => {},
		custom   => {},
		grid     => {},
	);

	my %state = (
		sec     => $EMPTY_STR,
		cur_msg => $EMPTY_STR,
		cur_sig => $EMPTY_STR,
		sender  => $EMPTY_STR,
		bus     => $EMPTY_STR,
		target  => $EMPTY_STR,
	);

	for my $i ( 0 .. $#{$lines_ref} ) {
		my $line = ${$lines_ref}[$i];

		if ( $line =~ /^\s*$/smx || $line =~ /^\s*[#]/smx ) {
			next;
		}

		my $indent = $EMPTY_STR;
		if ( $line =~ /^ (\s*) /smx ) {
			$indent = $1;
		}

		my $ind = length $indent;
		$line =~ s/^\s+//smx;

		if ( $ind == 0 && $line =~ /^ ([^:]+) : /smx ) {
			my $section = $1;
			$section =~ s/\s+$//smx;
			$state{sec} = $section;
			next;
		}

		_dispatch_line_parser( \%data, \%state, $ind, $line );
	}

	return \%data;
}

sub _dispatch_line_parser {
	my ( $data_ref, $state_ref, $ind, $line ) = @_;

	if ( $state_ref->{sec} eq 'routing' ) {
		parse_routing( $data_ref, $state_ref, $ind, $line );
		return;
	}
	if ( $state_ref->{sec} eq 'Message ID' ) {
		parse_message_id( $data_ref, $state_ref, $ind, $line );
		return;
	}
	if ( $state_ref->{sec} eq 'Custom CAN ID' ) {
		parse_custom_id( $data_ref, $state_ref, $ind, $line );
		return;
	}
	if ( $state_ref->{sec} eq 'GR ID' ) {
		parse_grid_id( $data_ref, $line );
		return;
	}
	return;
}

sub parse_routing {
	my ( $data_ref, $state_ref, $ind, $line ) = @_;

	if ( $line =~ /^ messages: /smx ) {
		return;
	}
	if ( $ind == 4 && $line =~ /^ ([^:]+) : /smx ) {
		my $sender = $1;
		$sender =~ s/\s+$//smx;
		$state_ref->{sender} = $sender;
		return;
	}
	if ( $ind == 6 && $line =~ /^ ([^:]+) : /smx ) {
		my $bus = $1;
		$bus =~ s/\s+$//smx;
		$state_ref->{bus} = $bus;
		return;
	}
	if ( $ind == 8 && $line =~ /^ ([^:]+) : /smx ) {
		my $target = $1;
		$target =~ s/\s+$//smx;
		$state_ref->{target} = $target;

		if ( $line =~ / \[ \s* [{] \s* msg: \s* ([^}]+?) \s* [}] \s* \] /smx ) {
			my $msg = $1;
			$msg =~ s/\s+$//smx;
			push @{ $data_ref->{routing} }, { sender => $state_ref->{sender}, bus => $state_ref->{bus}, target => $state_ref->{target}, msg => $msg };
		}
		return;
	}
	if ( $line =~ /^ [-] \s+ msg: \s* (.+) /smx ) {
		my $msg = $1;
		$msg =~ s/\s+$//smx;
		push @{ $data_ref->{routing} }, { sender => $state_ref->{sender}, bus => $state_ref->{bus}, target => $state_ref->{target}, msg => $msg };
		return;
	}
	if ( $line =~ /^ can_id_override: \s* (.+) /smx ) {
		my $override = $1;
		$override =~ s/\s+$//smx;
		if ( @{ $data_ref->{routing} } ) {
			$data_ref->{routing}->[-1]->{override} = $override;
		}
		return;
	}
	return;
}

sub parse_message_id {
	my ( $data_ref, $state_ref, $ind, $line ) = @_;

	if ( $ind == 2 && $line =~ /^ ([^:]+) : /smx ) {
		my $msg = $1;
		$msg =~ s/\s+$//smx;
		$state_ref->{cur_msg} = $msg;
		$data_ref->{messages}{ $state_ref->{cur_msg} } = { sigs => {} };
		return;
	}
	if ( $ind == 4 && $line =~ /^ MSG \s+ ID \s* : \s* (.+) /ixsm ) {
		my $id = $1;
		$id =~ s/\s+$//smx;
		$data_ref->{messages}{ $state_ref->{cur_msg} }{id} = $id;
		return;
	}
	if ( $ind == 4 && $line =~ /^ MSG \s+ LENGTH \s* : \s* (.+) /ixsm ) {
		my $len = $1;
		$len =~ s/\s+$//smx;
		$data_ref->{messages}{ $state_ref->{cur_msg} }{len} = $len;
		return;
	}
	if ( $ind == 4 && $line =~ /^ ([^:]+) : /smx ) {
		my $sig = $1;
		$sig =~ s/\s+$//smx;
		$state_ref->{cur_sig} = $sig;
		$data_ref->{messages}{ $state_ref->{cur_msg} }{sigs}{ $state_ref->{cur_sig} } = {};
		return;
	}
	if ( $ind == 6 && $state_ref->{cur_sig} ne $EMPTY_STR && $line =~ /^ ([^:]+) \s* : \s* (.+) /smx ) {
		my $k = $1;
		my $v = $2;
		$k =~ s/\s+$//smx;
		$v =~ s/\s+$//smx;

		if ( $k eq 'bit_start' || $k eq 'bit start' ) {
			$v =~ s/-.*//smx;
			$data_ref->{messages}{ $state_ref->{cur_msg} }{sigs}{ $state_ref->{cur_sig} }{start} = $v;
		}
		else {
			$data_ref->{messages}{ $state_ref->{cur_msg} }{sigs}{ $state_ref->{cur_sig} }{$k} = $v;
		}
		return;
	}
	return;
}

sub parse_custom_id {
	my ( $data_ref, $state_ref, $ind, $line ) = @_;

	if ( $ind == 2 && $line =~ /^ ["']? ([^"':]+) ["']? : /smx ) {
		my $msg = $1;
		$msg =~ s/\s+$//smx;
		$state_ref->{cur_msg} = $msg;
		$data_ref->{custom}{ $state_ref->{cur_msg} } = { sigs => [] };
		return;
	}
	if ( $line =~ /^ CAN \s+ ID \s* : \s* (.+) /ixsm ) {
		my $id = $1;
		$id =~ s/\s+$//smx;
		$data_ref->{custom}{ $state_ref->{cur_msg} }{id} = $id;
		return;
	}
	if ( $line =~ /^ Length \s* : \s* (.+) /ixsm ) {
		my $len = $1;
		$len =~ s/\s+$//smx;
		$data_ref->{custom}{ $state_ref->{cur_msg} }{len} = $len;
		return;
	}
	if ( $line =~ /^ [-] \s+ name \s* : \s* ["']? ([^"']+) ["']? /smx ) {
		my $name = $1;
		$name =~ s/\s+$//smx;
		push @{ $data_ref->{custom}{ $state_ref->{cur_msg} }{sigs} }, { name => $name };
		return;
	}
	if ( $line =~ /^ bit_start \s* : \s* ([\d\-]+) /smx ) {
		my $bs = $1;
		$bs =~ s/-.*//smx;
		if ( @{ $data_ref->{custom}{ $state_ref->{cur_msg} }{sigs} } ) {
			$data_ref->{custom}{ $state_ref->{cur_msg} }{sigs}->[-1]->{start} = $bs;
		}
		return;
	}
	return;
}

sub parse_grid_id {
	my ( $data_ref, $line ) = @_;

	if ( $line =~ /^ ([^:]+) \s* : \s* ["']? ([^"']+) ["']? /smx ) {
		my $k = $1;
		my $v = $2;
		$k =~ s/\s+$//smx;
		$v =~ s/\s+$//smx;
		$data_ref->{grid}{$k} = $v;
	}
	return;
}
