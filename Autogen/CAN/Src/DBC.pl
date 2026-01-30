#!/usr/bin/env perl
#
# generate_dbc.pl - Convert YAML CAN spec to DBC format
#
# Usage: perl generate_dbc.pl format.yaml [output.dbc] [CAN1|CAN2|CAN3]
#
# No external dependencies - parses YAML manually like struct generator does.
#
use strict;
use warnings;
use File::Basename;
use File::Path qw(make_path);
# ============================================================================
# ARGUMENTS
# ============================================================================
my $yaml_path   = $ARGV[0] // 'format.CANdo';
my $output_path = $ARGV[1] // 'output.dbc';
my $bus_filter  = $ARGV[2];    # optional: CAN1, CAN2, or CAN3
my$dir = dirname($output_path);
make_path($dir) if $dir && $dir ne '.' && !-d $dir;
if ( !-e $yaml_path ) {
	die "Error: Could not find YAML file at: $yaml_path\n";
}

# If output is a directory, append default filename
if ( -d $output_path ) {
	$output_path =~ s/\/$//;    # remove trailing slash
	$output_path .= '/output.dbc';
}

# ============================================================================
# DATA TYPE MAPPINGS
# ============================================================================
my %TYPE_BITS = (
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
	's'      => 64,
	'double' => 64,
);

# ============================================================================
# HELPER FUNCTIONS
# ============================================================================

sub normalize_name {
	my ($name) = @_;
	$name =~ s/\s+/_/g;
	$name =~ s/-/_/g;
	$name =~ s/[^a-zA-Z0-9_]//g;
	return $name;
}

sub hex_to_int {
	my ($str) = @_;
	return undef unless defined $str;
	$str =~ s/^\s+|\s+$//g;
	$str =~ s/^["']|["']$//g;
	if ( $str =~ /^0x/i ) {
		return hex($str);
	}
	return int($str);
}

sub get_bit_length {
	my ($dtype) = @_;
	return 8 unless defined $dtype;

	# Handle array types like b[24]
	if ( $dtype =~ /^(\w+)\[(\d+)\]$/ ) {
		my ( $base, $count ) = ( $1, $2 );
		my $base_bits = $TYPE_BITS{$base} // 8;
		return $base_bits * $count;
	}

	return $TYPE_BITS{$dtype} // 8;
}

sub get_sign_char {
	my ($dtype) = @_;
	return '+' unless defined $dtype;
	return ( $dtype =~ /^[is]/ ) ? '-' : '+';
}

sub parse_map_equation {
	my ($eq) = @_;
	return ( 1, 0 ) unless defined $eq && $eq ne '-';

	$eq =~ s/^["']|["']$//g;

	return ( 1, 0 ) if $eq eq '1x' || $eq eq 'x';

	if ( $eq =~ /^([\d.]+)x$/ )           { return ( $1 + 0,  0 ); }
	if ( $eq =~ /^([\d.]+)x\+([\d.]+)$/ ) { return ( $1 + 0,  $2 + 0 ); }
	if ( $eq =~ /^([\d.]+)x-([\d.]+)$/ )  { return ( $1 + 0,  -( $2 + 0 ) ); }
	if ( $eq =~ /^x-([\d.]+)$/ )          { return ( 1,       -( $1 + 0 ) ); }
	if ( $eq =~ /^x\/([\d.]+)$/ )         { return ( 1 / $1,  0 ); }
	if ( $eq =~ /^([\d.]+)x\/([\d.]+)$/ ) { return ( $1 / $2, 0 ); }
	if ( $eq =~ /^abs\(x\)\/([\d.]+)$/ )  { return ( 1 / $1,  0 ); }
	if ( $eq =~ /^([\d.]+)x--([\d.]+)$/ ) { return ( $1 + 0,  $2 + 0 ); }

	return ( 1, 0 );
}

# ============================================================================
# PARSE YAML FILE
# ============================================================================
# Single-pass parser tracking:
#   - Current section (routing, Message ID, GR ID)
#   - Current message name (when in Message ID section)
#   - Current signal name (when inside a message)
#   - Current routing entry (when in routing section)

sub parse_yaml {
	my ($filename) = @_;

	my %data = (
		'routing'       => [],
		'Message ID'    => {},
		'Custom CAN ID' => {},
		'GR ID'         => {},
		'byte order'    => 'little_endian',
	);

	open my $fh, '<', $filename or die "Cannot open $filename: $!";

	my $current_section = '';
	my $current_msg     = '';
	my $current_signal  = '';
	my $current_route   = undef;

	while ( my $line = <$fh> ) {
		chomp $line;

		# Skip blank lines and pure comment lines
		next if $line =~ /^\s*$/;
		next if $line =~ /^\s*#/;

		# Count leading spaces
		my ($indent)   = $line =~ /^(\s*)/;
		my $indent_len = length( $indent // '' );
		my $content    = $line;
		$content =~ s/^\s+//;

		# ---- TOP LEVEL (indent 0) ----
		if ( $indent_len == 0 && $content =~ /^([^:]+):/ ) {
			my $key = $1;
			$current_section = $key;
			$current_msg     = '';
			$current_signal  = '';
			$current_route   = undef;

			# Handle "byte order: little_endian" on same line
			if ( $content =~ /^byte[_ ]order:\s*(\S+)/ ) {
				$data{'byte order'} = $1;
			}
			next;
		}

		# ---- ROUTING SECTION ----
		if ( $current_section eq 'routing' ) {

			# New route entry: "- sender: ACU"
			if ( $content =~ /^-\s*sender:\s*(.+)/ ) {
				$current_route = { sender => $1 };
				push @{ $data{'routing'} }, $current_route;
				next;
			}

			# Route properties
			if ( defined $current_route ) {
				if ( $content =~ /^msg:\s*(.+)/ )             { $current_route->{msg}             = $1; }
				if ( $content =~ /^target:\s*(.+)/ )          { $current_route->{target}          = $1; }
				if ( $content =~ /^bus:\s*(.+)/ )             { $current_route->{bus}             = $1; }
				if ( $content =~ /^can_id_override:\s*(.+)/ ) { $current_route->{can_id_override} = $1; }
			}
			next;
		}

		# ---- GR ID SECTION ----
		if ( $current_section eq 'GR ID' ) {

			# "  NodeName: 0x03"
			if ( $indent_len == 2 && $content =~ /^([^:]+):\s*(.+)/ ) {
				my ( $name, $val ) = ( $1, $2 );
				$name =~ s/^\s+|\s+$//g;
				$val  =~ s/^\s+|\s+$//g;
				$val  =~ s/^["']|["']$//g;
				$data{'GR ID'}{$name} = $val;
			}
			next;
		}

		# ---- CUSTOM CAN ID SECTION ----
		if ( $current_section eq 'Custom CAN ID' ) {

			# Message name at indent 2: "  DTI Control 1:"
			if ( $indent_len == 2 && $content =~ /^["']?([^"':]+)["']?:/ ) {
				$current_msg = $1;
				$current_msg =~ s/^\s+|\s+$//g;
				$data{'Custom CAN ID'}{$current_msg} = { signals => [] };
				next;
			}

			# Message properties at indent 4
			if ( $indent_len == 4 && $current_msg ) {
				if ( $content =~ /^['"]?CAN ID['"]?:\s*(.+)/ ) {
					$data{'Custom CAN ID'}{$current_msg}{'CAN ID'} = $1;
				}
				if ( $content =~ /^['"]?Length['"]?:\s*(\d+)/ ) {
					$data{'Custom CAN ID'}{$current_msg}{'Length'} = int($1);
				}
				next;
			}

			# Signal list item at indent 6: "- name: ..."
			if ( $indent_len == 6 && $current_msg && $content =~ /^-\s*name:\s*["']?([^"']+)["']?/ ) {
				my $sig_name = $1;
				$sig_name =~ s/^\s+|\s+$//g;
				push @{ $data{'Custom CAN ID'}{$current_msg}{signals} }, { name => $sig_name };
				next;
			}

			# Signal bit_start at indent 8
			if ( $indent_len == 8 && $current_msg && $content =~ /^bit_start:\s*(\d+)/ ) {
				my $sigs = $data{'Custom CAN ID'}{$current_msg}{signals};
				if (@$sigs) {
					$sigs->[-1]{bit_start} = int($1);
				}
				next;
			}

			next;
		}

		# ---- MESSAGE ID SECTION ----
		if ( $current_section eq 'Message ID' ) {

			# Message name at indent 2: "  ECU Status 1:"
			if ( $indent_len == 2 && $content =~ /^([^:]+):/ ) {
				$current_msg = $1;
				$current_msg =~ s/^\s+|\s+$//g;
				$data{'Message ID'}{$current_msg} = {};
				$current_signal = '';
				next;
			}

			# Message properties or signal at indent 4
			if ( $indent_len == 4 && $current_msg ) {

				# MSG ID: 0x003
				if ( $content =~ /^MSG ID:\s*(.+)/ ) {
					$data{'Message ID'}{$current_msg}{'MSG ID'} = $1;
					next;
				}

				# MSG LENGTH: 8
				if ( $content =~ /^MSG LENGTH:\s*(\d+)/ ) {
					$data{'Message ID'}{$current_msg}{'MSG LENGTH'} = int($1);
					next;
				}

				# Signal name with bit start: "    ECU State: # bit start 0"
				if ( $content =~ /^([^:]+):\s*#\s*bit start\s+(\d+)/ ) {
					my ( $sig_name, $bit_start ) = ( $1, $2 );
					$sig_name =~ s/^\s+|\s+$//g;
					$current_signal = $sig_name;
					$data{'Message ID'}{$current_msg}{$current_signal} = { '_bit_start' => int($bit_start), };
					next;
				}

				# Signal name without visible bit start (shouldn't happen but handle it)
				if ( $content =~ /^([^:]+):/ ) {
					my $sig_name = $1;
					$sig_name =~ s/^\s+|\s+$//g;
					next if $sig_name =~ /^Reserved/i;
					$current_signal = $sig_name;
					$data{'Message ID'}{$current_msg}{$current_signal} //= {};
					next;
				}
			}

			# Signal properties at indent 6
			if ( $indent_len == 6 && $current_msg && $current_signal ) {

				# Remove inline comments for property parsing
				my $prop_line = $content;
				$prop_line =~ s/\s*#.*$// unless $prop_line =~ /^#/;

				if ( $prop_line =~ /^([^:]+):\s*(.*)/ ) {
					my ( $key, $val ) = ( $1, $2 );
					$key =~ s/^\s+|\s+$//g;
					$val =~ s/^\s+|\s+$//g;
					$val =~ s/^["']|["']$//g;
					$data{'Message ID'}{$current_msg}{$current_signal}{$key} = $val;
				}
				next;
			}
		}
	}

	close $fh;
	return \%data;
}

# ============================================================================
# MAIN
# ============================================================================

print "Parsing $yaml_path...\n";

my $yaml = parse_yaml($yaml_path);

my $messages        = $yaml->{'Message ID'}    // {};
my $custom_messages = $yaml->{'Custom CAN ID'} // {};
my $gr_ids          = $yaml->{'GR ID'}         // {};
my $routing         = $yaml->{'routing'}       // [];
my $byte_order      = $yaml->{'byte order'}    // 'little_endian';

# Byte order: 1 = little endian, 0 = big endian
my $bo_value = ( $byte_order eq 'little_endian' ) ? 1 : 0;

print "Found " . scalar( keys %$messages ) . " message definitions\n";
print "Found " . scalar( keys %$custom_messages ) . " custom CAN ID messages\n";
print "Found " . scalar(@$routing) . " routing entries\n";
print "Found " . scalar( keys %$gr_ids ) . " node IDs\n";

# Collect all node names
my %nodes;
for my $name ( keys %$gr_ids ) {
	$nodes{ normalize_name($name) } = 1;
}

# Build DBC messages from routing
my @dbc_messages;
my %seen_ids;

for my $route (@$routing) {

	# Filter by bus if specified
	if ( defined $bus_filter && ( $route->{bus} // '' ) ne $bus_filter ) {
		next;
	}

	my $sender          = $route->{sender};
	my $msg_name        = $route->{msg};
	my $target          = $route->{target};
	my $can_id_override = $route->{can_id_override};

	next unless defined $sender && defined $msg_name && defined $target;

	# Get message definition (check both sections)
	my $msg_def   = $messages->{$msg_name};
	my $is_custom = 0;

	if ( !defined $msg_def && defined $custom_messages->{$msg_name} ) {
		$msg_def   = $custom_messages->{$msg_name};
		$is_custom = 1;
	}

	unless ( defined $msg_def ) {
		warn "Warning: Message '$msg_name' not found, skipping\n";
		next;
	}

	# Calculate CAN ID
	my $can_id;
	if ( defined $can_id_override ) {
		$can_id = hex_to_int($can_id_override);
	}
	elsif ( $is_custom && defined $msg_def->{'CAN ID'} ) {

		# Custom messages have their own CAN ID
		$can_id = hex_to_int( $msg_def->{'CAN ID'} );
	}
	else {
		my $sender_id = hex_to_int( $gr_ids->{$sender} );
		my $target_id = hex_to_int( $gr_ids->{$target} );
		my $msg_id    = hex_to_int( $msg_def->{'MSG ID'} // '0x000' );

		unless ( defined $sender_id && defined $target_id && defined $msg_id ) {
			warn "Warning: Cannot resolve IDs for $sender -> $target ($msg_name), skipping\n";
			next;
		}

		# Composite ID: sender << 16 | msg << 8 | target
		$can_id = ( $sender_id << 16 ) | ( $msg_id << 8 ) | $target_id;
	}

	# Skip duplicates
	my $id_key = sprintf( "%X_%s_%s", $can_id, $sender, $target );
	next if $seen_ids{$id_key}++;

	# Message length
	my $length = $msg_def->{'MSG LENGTH'} // $msg_def->{'Length'} // 8;

	# Build DBC message name
	my $dbc_msg_name = normalize_name($sender) . "_" . normalize_name($msg_name) . "_to_" . normalize_name($target);

	# Collect signals
	my @signals;

	if ($is_custom) {

		# Custom CAN ID format: signals is an array
		my $sig_list = $msg_def->{signals} // [];
		for my $sig (@$sig_list) {
			my $sig_name = $sig->{name};
			next unless defined $sig_name;

			my $bit_start = $sig->{bit_start};
			unless ( defined $bit_start ) {
				warn "Warning: No bit start for '$sig_name' in '$msg_name', skipping signal\n";
				next;
			}

			# Custom messages don't have type info, default to u16
			push @signals, {
				name       => normalize_name($sig_name),
				start_bit  => $bit_start,
				length     => 16,                          # default
				byte_order => $bo_value,
				sign       => '+',
				scale      => 1,
				offset     => 0,
				min        => 0,
				max        => 0,
				unit       => '',
				receiver   => normalize_name($target),
			};
		}
	}
	else {
		# Standard Message ID format
		for my $sig_name ( keys %$msg_def ) {
			next if $sig_name eq 'MSG ID';
			next if $sig_name eq 'MSG LENGTH';
			next if $sig_name =~ /^Reserved/i;

			my $sig_def = $msg_def->{$sig_name};
			next unless ref($sig_def) eq 'HASH';

			# Get bit start
			my $bit_start = $sig_def->{'_bit_start'};
			unless ( defined $bit_start ) {
				warn "Warning: No bit start for '$sig_name' in '$msg_name', skipping signal\n";
				next;
			}

			my $dtype      = $sig_def->{'data type'} // 'u8';
			my $bit_length = get_bit_length($dtype);
			my $sign_char  = get_sign_char($dtype);

			my ( $scale, $offset ) = parse_map_equation( $sig_def->{'map equation'} );

			my $min_val = $sig_def->{'scaled min'} // 0;
			my $max_val = $sig_def->{'scaled max'} // 0;
			$min_val = 0 if !defined($min_val) || $min_val eq '-' || $min_val eq '';
			$max_val = 0 if !defined($max_val) || $max_val eq '-' || $max_val eq '';

			my $unit = $sig_def->{'units'} // '';
			$unit = '' if $unit eq '-';

			push @signals,
			  {
				name       => normalize_name($sig_name),
				start_bit  => $bit_start,
				length     => $bit_length,
				byte_order => $bo_value,
				sign       => $sign_char,
				scale      => $scale,
				offset     => $offset,
				min        => $min_val,
				max        => $max_val,
				unit       => $unit,
				receiver   => normalize_name($target),
			  };
		}
	}

	# Sort signals by bit position, then by name for determinism
	@signals = sort { $a->{start_bit} <=> $b->{start_bit} || $a->{name} cmp $b->{name} } @signals;

	push @dbc_messages,
	  {
		name        => $dbc_msg_name,
		can_id      => $can_id,
		length      => $length,
		transmitter => normalize_name($sender),
		signals     => \@signals,
	  };
}

# Sort messages by CAN ID
@dbc_messages = sort { $a->{can_id} <=> $b->{can_id} } @dbc_messages;

# ============================================================================
# WRITE DBC FILE
# ============================================================================

open my $out, '>', $output_path or die "Cannot open $output_path: $!";

print $out "VERSION \"\"\n\n";
print $out "NS_ :\n\n";
print $out "BS_:\n\n";

# Nodes
print $out "BU_:";
for my $node ( sort keys %nodes ) {
	print $out " $node";
}
print $out "\n\n";

# Messages
for my $msg (@dbc_messages) {
	my $can_id = $msg->{can_id};

	# Extended ID flag if > 11 bits (0x7FF)
	if ( $can_id > 0x7FF ) {
		$can_id = $can_id | 0x80000000;
	}

	printf $out "BO_ %u %s: %d %s\n", $can_id, $msg->{name}, $msg->{length}, $msg->{transmitter};

	for my $sig ( @{ $msg->{signals} } ) {
		printf $out " SG_ %s : %d|%d@%d%s (%s,%s) [%s|%s] \"%s\" %s\n",
		  $sig->{name},
		  $sig->{start_bit},
		  $sig->{length},
		  $sig->{byte_order},
		  $sig->{sign},
		  $sig->{scale},
		  $sig->{offset},
		  $sig->{min},
		  $sig->{max},
		  $sig->{unit},
		  $sig->{receiver};
	}

	print $out "\n";
}

close $out;

my $msg_count = scalar @dbc_messages;
print "Successfully generated $output_path ($msg_count messages)\n";
