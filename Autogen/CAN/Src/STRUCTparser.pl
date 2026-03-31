#!/usr/bin/env perl
use strict;
use warnings;
use English qw(-no_match_vars);
use File::Basename;
use File::Path qw(make_path);

# --- 1. Configuration ---
my $yaml_path   = $ARGV[0] // 'Doc/GRCAN.CANdo';
my $output_path = $ARGV[1] // 'Inc/GRCAN_MSG_DATA.h';

if ( !-e $yaml_path ) { die "Error: $yaml_path not found.\n"; }

# --- 2. Trackers (Gatekeepers) ---
my %seen_structs;
my $content = "// Auto-generated GRCAN Message Data (Pure Perl Parser)\n";
$content .= "// Resolves #338 (Naming), #367 (Duplicates), and C Syntax Errors\n";
$content .= "#ifndef GRCAN_MSG_DATA_H\n#define GRCAN_MSG_DATA_H\n\n#include <stdint.h>\n\n";

# --- 3. Manual Parsing Logic ---
open my $fh, '<', $yaml_path or die "Could not open $yaml_path: $OS_ERROR";

my $current_msg = '';
my @fields = ();

while (my $line = <$fh>) {
    chomp $line;

    # Match Message Name (Indented by 2 spaces)
    if ($line =~ /^\s{2}([^:#\s][^:]+):/x) {
        # Process the previous message before starting the new one
        if ($current_msg && @fields) {
            $content .= generate_struct($current_msg, \@fields);
        }

        $current_msg = $1;
        $current_msg =~ s/^\s+|\s+$//g;
        @fields = ();
        next;
    }

    # Match Field Name (Indented by 4 spaces)
    if ($current_msg && $line =~ /^\s{4}([^:#\s][^:]+):/x) {
        my $field_name = $1;
        $field_name =~ s/^\s+|\s+$//g;
        push(@fields, $field_name);
    }
}

# Process the very last message in the file
if ($current_msg && @fields) {
    $content .= generate_struct($current_msg, \@fields);
}

close $fh;

$content .= "#endif // GRCAN_MSG_DATA_H\n";

# --- 4. Helper Function: Struct Generation ---
sub generate_struct {
    my ($msg_name, $fields_ref) = @_;

    # Standardize Struct Name: GRCAN_NAME_t
    my $clean_name = uc $msg_name;
    $clean_name =~ s/[^A-Z0-9]/_/g;
    $clean_name =~ s/_+/_/g;
    $clean_name =~ s/^_|_$//g;

    my $struct_name = "GRCAN_${clean_name}_t";

    # GATEKEEPER: Prevent duplicate struct definitions
    if ($seen_structs{$struct_name}) {
        warn "Skipping duplicate struct definition: $struct_name\n";
        return "";
    }
    $seen_structs{$struct_name} = 1;

    # Build the C Struct
    my $str = "typedef struct {\n";
    for my $field (@{$fields_ref}) {
        # Clean the field name
        $field = lc $field;
        $field =~ s/[^a-z0-9]/_/g;
        $field =~ s/_+/_/g;
        $field =~ s/^_|_$//g;

        # THE FIX: If field starts with a number, add leading underscore for C compatibility
        if ($field =~ /^\d/) {
            $field = "_" . $field;
        }

        $str .= "    uint8_t    $field;\n";
    }
    $str .= "} $struct_name;\n\n";

    return $str;
}

# --- 5. Write Output (Overwrites existing file) ---
my $dir = dirname($output_path);
make_path($dir) if ($dir && !-d $dir);

open my $out, '>', $output_path or die "Cannot write to $output_path: $OS_ERROR";
print {$out} $content;
close $out;

print "Successfully updated $output_path (Fixed illegal identifiers).\n";
exit 0;







# #!/usr/bin/env perl
# use strict;
# use warnings;
# use English qw(-no_match_vars);
# use autodie qw(open close);
# use File::Basename;
# use File::Path qw(make_path);
# use Readonly;

# # --- Configuration & Constants ---
# Readonly::Scalar my $BITS_PER_BYTE => 8;
# Readonly::Scalar my $CELL_COUNT    => 32;
# Readonly::Scalar my $EMPTY_STR     => q{};
# Readonly::Scalar my $SPACE_STR     => q{ };

# main();

# sub main {
#     # Default paths per Gaucho Racing repo structure
#     my $yaml_path   = $ARGV[0] // 'Doc/format.CANdo';
#     my $output_path = $ARGV[1] // 'Inc/GRCAN_MSG_DATA.h';
#     my $prefix      = 'GRCAN'; # FIXED: Resolved Issue #338 (Renamed from GR_OLD)

#     ensure_directory_exists($output_path);

#     my %desc_map = parse_descriptions($yaml_path);
#     generate_header( $yaml_path, $output_path, $prefix, \%desc_map );

#     return;
# }

# sub ensure_directory_exists {
#     my ($path) = @_;
#     my $dir = dirname $path;
#     if ( $dir && $dir ne q{.} && !-d $dir ) {
#         make_path $dir;
#     }
#     return;
# }

# sub parse_descriptions {
#     my ($path) = @_;
#     my %map;
#     my $in_msg_section = 0;

#     open my $in, '<', $path;
#     my @lines = <$in>;
#     close $in;

#     for my $i ( 0 .. $#lines ) {
#         my $line = $lines[$i];
#         chomp $line;

#         if ( $line =~ /^Message \s ID:/smx ) {
#             $in_msg_section = 1;
#             next;
#         }
#         if ( $line =~ /^\S/smx ) {
#             $in_msg_section = 0;
#         }
#         if ( !$in_msg_section ) {
#             next;
#         }

#         if ( $line =~ /^\s{2,4} ([^:#\s][^:]+) :/smx ) {
#             my $raw_name = $1;
#             my $f_name   = clean_field_name($raw_name);
#             my ( $desc, $new_i ) = extract_desc_from_array( \@lines, $i );
#             if ($desc) {
#                 $map{$f_name} = $desc;
#             }
#             $i = $new_i;
#         }
#     }
#     return %map;
# }

# sub extract_desc_from_array {
#     my ( $lines_ref, $index ) = @_;
#     my $description      = '';
#     my $i                = $index;
#     my $in_comment_block = 0;

#     while ( ++$i < scalar @{$lines_ref} ) {
#         my $sub = ${$lines_ref}[$i];

#         if ( $sub =~ /^\s+ comment: \s* (.*)/smx ) {
#             my $text = $1;
#             $in_comment_block = 1;
#             if ( $text ne '' ) {
#                 $description .= ( $description ? ' ' : '' ) . $text;
#             }
#             next;
#         }

#         if ($in_comment_block) {
#             if ( $sub =~ /^\s{4,6} \w+[\w\s]*:(\s|$)/smx ) {
#                 last;
#             }
#             if ( $sub =~ /^\s{6,} (.+)/smx ) {
#                 my $line_text = $1;
#                 $description .= ( $description ? ' ' : '' ) . $line_text;
#                 next;
#             }
#         }

#         if ( $sub =~ /^\s{0,4} \S/smx ) {
#             last;
#         }
#     }

#     $description =~ s/^\s+|\s+$//gsmx;
#     return ( $description, $i - 1 );
# }

# sub generate_header {
#     my ( $in_p, $out_p, $prefix, $d_map ) = @_;
#     my $lines_ref = slurp_file($in_p);
#     my @output;

#     push @output, "/* Auto-generated Message Data Layouts */\n";
#     push @output, "#ifndef ${prefix}_MSG_DATA_H\n";
#     push @output, "#define ${prefix}_MSG_DATA_H\n\n";
#     push @output, "#include <stdint.h>\n\n";

#     my $in_msg_section = 0;
#     my $current_msg    = $EMPTY_STR;
#     my @fields         = ();
#     my %seen_structs; # FIX: Tracker to prevent redefinition of common structs (e.g. PING)

#     for my $i ( 0 .. $#{$lines_ref} ) {
#         my $line = ${$lines_ref}[$i];
#         if ( $line =~ /^Message \s ID:/smx ) { $in_msg_section = 1; next; }
#         if ( $line =~ /^\S/smx )             { $in_msg_section = 0; }
#         if ( !$in_msg_section )              { next; }

#         if ( $line =~ /^\s{2} ([^:#\s][^:]+) :$/smx ) {
#             my $msg_name = $1;
#             if ($current_msg) {
#                 push @output, process_message( $current_msg, \@fields, $d_map, $prefix, \%seen_structs );
#             }
#             $current_msg = $msg_name;
#             @fields      = ();
#         }

#         elsif ( $line =~ /^\s{4} ([^:#\s][^:]+) :/smx ) {
#             my $field_raw = $1;
#             my ( $f_data, $new_i ) = parse_field_details( $lines_ref, $i, $field_raw );
#             if ( %{$f_data} ) {
#                 push @fields, $f_data;
#             }
#             $i = $new_i;
#         }
#     }

#     if ($current_msg) {
#         push @output, process_message( $current_msg, \@fields, $d_map, $prefix, \%seen_structs );
#     }
#     push @output, "#endif // ${prefix}_MSG_DATA_H\n";

#     write_file( $out_p, \@output );
#     return;
# }

# sub slurp_file {
#     my ($path) = @_;
#     open my $fh, '<', $path;
#     my @lines = <$fh>;
#     close $fh;
#     chomp @lines;
#     return \@lines;
# }

# sub write_file {
#     my ( $path, $content_ref ) = @_;
#     open my $fh, '>', $path;
#     for my $line ( @{$content_ref} ) {
#         print {$fh} $line or die "Could not write to $path: $OS_ERROR";
#     }
#     close $fh;
#     return;
# }

# sub parse_field_details {
#     my ( $lines_ref, $index, $name ) = @_;
#     my ( $start, $data_type ) = ( undef, 'u8' );
#     my $i = $index;

#     while ( ++$i < scalar @{$lines_ref} ) {
#         my $sub = ${$lines_ref}[$i];
#         if ( $sub =~ /bit_start: \s* (\d+)/smx ) {
#             $start = $1;
#         }
#         if ( $sub =~ /data \s type: \s* (\w+)/smx ) {
#             $data_type = $1;
#         }
#         if ( $sub =~ /^\s{4} [^#\s]/smx || $sub =~ /^\s{0,2} \S/smx ) {
#             last;
#         }
#     }
#     my %res = defined $start ? ( name => $name, start => $start, type => $data_type ) : ();
#     return ( \%res, $i - 1 );
# }

# sub clean_field_name {
#     my ($name) = @_;
#     my $clean = lc $name;
#     $clean =~ s/[^[:lower:][:digit:]]/_/gsmx;
#     $clean =~ s/_+/_/gsmx;
#     $clean =~ s/^_|_$//gsmx;
#     return $clean || 'unknown_field';
# }

# sub process_message {
#     my ( $name, $f_ref, $d_map, $prefix, $seen_ref ) = @_;
#     my @buf;

#     # Generate clean tag for the struct name
#     my $tag = uc $name;
#     $tag =~ s/[^[:alpha:][:digit:]]/_/gsmx;
#     $tag =~ s/_+/_/gsmx;
#     $tag =~ s/^_|_$//gsmx;

#     my $final_type_name = "${prefix}_${tag}_t"; # Standard: GRCAN_MSG_NAME_t

#     # Prevent re-defining the same struct if it appears multiple times (Issue #367)
#     if ( $seen_ref->{$final_type_name} ) {
#         return $EMPTY_STR;
#     }
#     $seen_ref->{$final_type_name} = 1;

#     # Special Case: Cell Data (Specific alignment for BMS)
#     if ( $name =~ /Cell \s Data/ismx ) {
#         push @buf, sprintf "/** %s */\ntypedef struct {\n    struct {\n        uint8_t voltage;\n        uint8_t temperature;\n    } cells[%d];\n} %s;\n\n", $name, $CELL_COUNT, $final_type_name;
#         return join $EMPTY_STR, @buf;
#     }

#     # Group fields by byte for the struct layout
#     my %byte_map;
#     for my $f ( @{$f_ref} ) {
#         push @{ $byte_map{ int( $f->{start} / $BITS_PER_BYTE ) } }, $f;
#     }

#     push @buf, "/** $name */\ntypedef struct {\n";
#     my @sorted = sort { $a <=> $b } keys %byte_map;

#     for my $i ( 0 .. $#sorted ) {
#         push @buf, process_byte_entry( \@sorted, \%byte_map, \$i, $d_map );
#     }

#     push @buf, "} $final_type_name;\n\n";
#     return join $EMPTY_STR, @buf;
# }

# sub process_byte_entry {
#     my ( $sorted_ref, $map_ref, $idx_ref, $d_map ) = @_;
#     my @out;
#     my $b_idx  = ${$sorted_ref}[ ${$idx_ref} ];
#     my $fields = ${$map_ref}{$b_idx};

#     # Handle multi-field bytes (Errors/Faults/Ping Blocks)
#     if ( scalar @{$fields} > 2 ) {
#         push @out, handle_multi_field_range( $sorted_ref, $map_ref, $idx_ref );
#     }
#     else {
#         my $f_var =
#           ( scalar @{$fields} == 1 )
#           ? clean_field_name( ${$fields}[0]->{name} )
#           : join q{_}, map { clean_field_name( $_->{name} ) } @{$fields};
#         if ( $f_var =~ /^[[:digit:]]/smx ) {
#             $f_var = q{_} . $f_var;
#         }

#         # Resolve bit-width types
#         my $type =
#             ( ${$fields}[0]->{type} =~ /32/smx ) ? 'uint32_t'
#           : ( ${$fields}[0]->{type} =~ /16/smx ) ? 'uint16_t'
#           :                                        'uint8_t';

#         my $desc = join $SPACE_STR, map { ${$d_map}{ clean_field_name( $_->{name} ) } // () } @{$fields};

#         push @out, sprintf "    /** %s (Byte %d) */\n    %-10s %-30s\n", ( $desc || "Byte $b_idx" ), $b_idx, $type, $f_var . q{;};
#     }
#     return join $EMPTY_STR, @out;
# }

# sub handle_multi_field_range {
#     my ( $bytes_ref, $map_ref, $idx_ref ) = @_;
#     my $start_byte = ${$bytes_ref}[ ${$idx_ref} ];
#     my $has_error  = grep { $_->{name} =~ /error|fault|violation/ismx } @{ ${$map_ref}{$start_byte} };

#     while ( ${$idx_ref} + 1 < scalar @{$bytes_ref} ) {
#         my $next_byte = ${$bytes_ref}[ ${$idx_ref} + 1 ];
#         my $next_f    = ${$map_ref}{$next_byte};
#         if ( scalar @{$next_f} <= 2 ) {
#             my $first_name = ${$next_f}[0]->{name};
#             if ( $first_name !~ /reserved/ismx ) {
#                 last;
#             }
#         }
#         ${$idx_ref}++;
#     }

#     my $len    = ( ${$bytes_ref}[ ${$idx_ref} ] - $start_byte ) + 1;
#     my $v_name = $has_error   ? 'error_fault_violation_bits' : 'ping_block';
#     my $suffix = ( $len > 1 ) ? "[$len]"                     : $EMPTY_STR;
#     return sprintf "    uint8_t    %s%s;\n", $v_name, $suffix;
# }










# # #!/usr/bin/perl
# # use strict;
# # use warnings;
# # use English qw(-no_match_vars);
# # use autodie qw(open close);
# # use File::Basename;
# # use File::Path qw(make_path);
# # use Readonly;

# # # Brutal-compliant constants
# # Readonly::Scalar my $BITS_PER_BYTE => 8;
# # Readonly::Scalar my $CELL_COUNT    => 32;
# # Readonly::Scalar my $EMPTY_STR     => q{};
# # Readonly::Scalar my $SPACE_STR     => q{ };

# # main();

# # sub main {
# # 	my $yaml_path   = $ARGV[0] // 'format.CANdo';
# # 	my $output_path = $ARGV[1] // 'CANDler.h';
# # 	my $prefix      = 'GR_OLD';

# # 	ensure_directory_exists($output_path);

# # 	my %desc_map = parse_descriptions($yaml_path);
# # 	generate_header( $yaml_path, $output_path, $prefix, \%desc_map );

# # 	return;
# # }

# # sub ensure_directory_exists {
# # 	my ($path) = @_;
# # 	my $dir = dirname $path;
# # 	if ( $dir && $dir ne q{.} && !-d $dir ) {
# # 		make_path $dir;
# # 	}
# # 	return;
# # }

# # sub parse_descriptions {
# # 	my ($path) = @_;
# # 	my %map;
# # 	my $in_msg_section = 0;

# # 	open my $in, '<', $path;
# # 	my @lines = <$in>;
# # 	close $in;

# # 	for my $i ( 0 .. $#lines ) {
# # 		my $line = $lines[$i];
# # 		chomp $line;

# # 		if ( $line =~ /^Message \s ID:/smx ) {
# # 			$in_msg_section = 1;
# # 			next;
# # 		}
# # 		if ( $line =~ /^\S/smx ) {
# # 			$in_msg_section = 0;
# # 		}
# # 		if ( !$in_msg_section ) {
# # 			next;
# # 		}

# # 		# Use explicit \s{2,4} because /x ignores literal spaces
# # 		if ( $line =~ /^\s{2,4} ([^:#\s][^:]+) :/smx ) {
# # 			my $raw_name = $1;
# # 			my $f_name   = clean_field_name($raw_name);
# # 			my ( $desc, $new_i ) = extract_desc_from_array( \@lines, $i );
# # 			if ($desc) {
# # 				$map{$f_name} = $desc;
# # 			}
# # 			$i = $new_i;
# # 		}
# # 	}
# # 	return %map;
# # }

# # sub extract_desc_from_array {
# # 	my ( $lines_ref, $index ) = @_;
# # 	my $description      = '';
# # 	my $i                = $index;
# # 	my $in_comment_block = 0;

# # 	while ( ++$i < scalar @{$lines_ref} ) {
# # 		my $sub = ${$lines_ref}[$i];

# # 		# 1. Match the start of the comment block
# # 		if ( $sub =~ /^\s+ comment: \s* (.*)/smx ) {
# # 			my $text = $1;
# # 			$in_comment_block = 1;
# # 			if ( $text ne '' ) {
# # 				$description .= ( $description ? ' ' : '' ) . $text;
# # 			}
# # 			next;
# # 		}

# # 		# 2. If we are in the block, grab lines that ARE NOT new YAML keys
# # 		if ($in_comment_block) {

# # 			# A YAML key usually looks like: "    units:" or "    data type:"
# # 			# This regex says: Stop if the line starts with 4-6 spaces,
# # 			# followed by a word, and then a colon + space/newline.
# # 			if ( $sub =~ /^\s{4,6} \w+[\w\s]*:(\s|$)/smx ) {
# # 				last;
# # 			}

# # 			# Otherwise, if it's indented text, it's part of our sentence!
# # 			if ( $sub =~ /^\s{6,} (.+)/smx ) {
# # 				my $line_text = $1;
# # 				$description .= ( $description ? ' ' : '' ) . $line_text;
# # 				next;
# # 			}
# # 		}

# # 		# 3. Global break if we hit a new message or field entirely
# # 		if ( $sub =~ /^\s{0,4} \S/smx ) {
# # 			last;
# # 		}
# # 	}

# # 	# Clean up any trailing/leading whitespace
# # 	$description =~ s/^\s+|\s+$//gsmx;
# # 	return ( $description, $i - 1 );
# # }

# # sub generate_header {
# # 	my ( $in_p, $out_p, $prefix, $d_map ) = @_;
# # 	my $lines_ref = slurp_file($in_p);
# # 	my @output;

# # 	push @output, "/* Auto-generated header file */\n";
# # 	push @output, "#ifndef ${prefix}_MESSAGES_H\n";
# # 	push @output, "#define ${prefix}_MESSAGES_H\n\n";
# # 	push @output, "#include <stdint.h>\n\n";

# # 	my $in_msg_section = 0;
# # 	my $current_msg    = $EMPTY_STR;
# # 	my @fields         = ();

# # 	for my $i ( 0 .. $#{$lines_ref} ) {
# # 		my $line = ${$lines_ref}[$i];
# # 		if ( $line =~ /^Message \s ID:/smx ) { $in_msg_section = 1; next; }
# # 		if ( $line =~ /^\S/smx )             { $in_msg_section = 0; }
# # 		if ( !$in_msg_section )              { next; }

# # 		# Explicitly match 2 spaces for Message Name
# # 		if ( $line =~ /^\s{2} ([^:#\s][^:]+) :$/smx ) {
# # 			my $msg_name = $1;
# # 			if ($current_msg) {
# # 				push @output, process_message( $current_msg, \@fields, $d_map, $prefix );
# # 			}
# # 			$current_msg = $msg_name;
# # 			@fields      = ();
# # 		}

# # 		# Explicitly match 4 spaces for Field Name
# # 		elsif ( $line =~ /^\s{4} ([^:#\s][^:]+) :/smx ) {
# # 			my $field_raw = $1;
# # 			my ( $f_data, $new_i ) = parse_field_details( $lines_ref, $i, $field_raw );
# # 			if ( %{$f_data} ) {
# # 				push @fields, $f_data;
# # 			}
# # 			$i = $new_i;
# # 		}
# # 	}

# # 	if ($current_msg) {
# # 		push @output, process_message( $current_msg, \@fields, $d_map, $prefix );
# # 	}
# # 	push @output, "#endif\n";

# # 	write_file( $out_p, \@output );
# # 	return;
# # }

# # sub slurp_file {
# # 	my ($path) = @_;
# # 	open my $fh, '<', $path;
# # 	my @lines = <$fh>;
# # 	close $fh;
# # 	chomp @lines;
# # 	return \@lines;
# # }

# # sub write_file {
# # 	my ( $path, $content_ref ) = @_;
# # 	open my $fh, '>', $path;
# # 	for my $line ( @{$content_ref} ) {
# # 		print {$fh} $line or die "Could not write to $path: $OS_ERROR";
# # 	}
# # 	close $fh;
# # 	return;
# # }

# # sub parse_field_details {
# # 	my ( $lines_ref, $index, $name ) = @_;
# # 	my ( $start, $data_type ) = ( undef, 'u8' );
# # 	my $i = $index;

# # 	while ( ++$i < scalar @{$lines_ref} ) {
# # 		my $sub = ${$lines_ref}[$i];
# # 		if ( $sub =~ /bit_start: \s* (\d+)/smx ) {
# # 			$start = $1;
# # 		}
# # 		if ( $sub =~ /data \s type: \s* (\w+)/smx ) {
# # 			$data_type = $1;
# # 		}
# # 		if ( $sub =~ /^\s{4} [^#\s]/smx || $sub =~ /^\s{0,2} \S/smx ) {
# # 			last;
# # 		}
# # 	}
# # 	my %res = defined $start ? ( name => $name, start => $start, type => $data_type ) : ();
# # 	return ( \%res, $i - 1 );
# # }

# # sub clean_field_name {
# # 	my ($name) = @_;
# # 	my $clean = lc $name;

# # 	# Correctly inverted POSIX character class
# # 	$clean =~ s/[^[:lower:][:digit:]]/_/gsmx;
# # 	$clean =~ s/_+/_/gsmx;
# # 	$clean =~ s/^_|_$//gsmx;
# # 	return $clean || 'unknown_field';
# # }

# # sub process_message {
# # 	my ( $name, $f_ref, $d_map, $prefix ) = @_;
# # 	my @buf;
# # 	my $tag = uc $name =~ s/[^[:alpha:][:digit:]]/_/grsmx =~ s/_+/_/grsmx =~ s/^_|_$//grsmx;

# # 	if ( $name =~ /Cell \s Data/ismx ) {
# # 		push @buf, sprintf "/** %s */\ntypedef struct {\n\tstruct {\n\t\tuint8_t voltage;\n\t\tuint8_t temperature;\n\t} cells[%d];\n} %s_%s_MSG;\n\n", $name, $CELL_COUNT, $prefix, $tag;
# # 		return join $EMPTY_STR, @buf;
# # 	}

# # 	my %byte_map;
# # 	for my $f ( @{$f_ref} ) {
# # 		push @{ $byte_map{ int( $f->{start} / $BITS_PER_BYTE ) } }, $f;
# # 	}

# # 	push @buf, "/** $name */\ntypedef struct {\n";
# # 	my @sorted = sort { $a <=> $b } keys %byte_map;

# # 	for my $i ( 0 .. $#sorted ) {
# # 		push @buf, process_byte_entry( \@sorted, \%byte_map, \$i, $d_map );
# # 	}
# # 	push @buf, "} ${prefix}_${tag}_MSG;\n\n";
# # 	return join $EMPTY_STR, @buf;
# # }

# # sub process_byte_entry {
# # 	my ( $sorted_ref, $map_ref, $idx_ref, $d_map ) = @_;
# # 	my @out;
# # 	my $b_idx  = ${$sorted_ref}[ ${$idx_ref} ];
# # 	my $fields = ${$map_ref}{$b_idx};

# # 	if ( scalar @{$fields} > 2 ) {
# # 		push @out, handle_multi_field_range( $sorted_ref, $map_ref, $idx_ref );
# # 	}
# # 	else {
# # 		my $f_var =
# # 		  ( scalar @{$fields} == 1 )
# # 		  ? clean_field_name( ${$fields}[0]->{name} )
# # 		  : join q{_}, map { clean_field_name( $_->{name} ) } @{$fields};
# # 		if ( $f_var =~ /^[[:digit:]]/smx ) {
# # 			$f_var = q{_} . $f_var;
# # 		}

# # 		my $type =
# # 		    ( ${$fields}[0]->{type} =~ /32/smx ) ? 'uint32_t'
# # 		  : ( ${$fields}[0]->{type} =~ /16/smx ) ? 'uint16_t'
# # 		  :                                        'uint8_t';
# # 		my $desc = join $SPACE_STR, map { ${$d_map}{ clean_field_name( $_->{name} ) } // () } @{$fields};

# # 		push @out, sprintf "\t/** %s (Byte %d) */\n\t%-10s %-30s\n", ( $desc || "Byte $b_idx" ), $b_idx, $type, $f_var . q{;};
# # 	}
# # 	return join $EMPTY_STR, @out;
# # }

# # sub handle_multi_field_range {
# # 	my ( $bytes_ref, $map_ref, $idx_ref ) = @_;
# # 	my $start_byte = ${$bytes_ref}[ ${$idx_ref} ];
# # 	my $has_error  = grep { $_->{name} =~ /error|fault|violation/ismx } @{ ${$map_ref}{$start_byte} };

# # 	while ( ${$idx_ref} + 1 < scalar @{$bytes_ref} ) {
# # 		my $next_byte = ${$bytes_ref}[ ${$idx_ref} + 1 ];
# # 		my $next_f    = ${$map_ref}{$next_byte};
# # 		if ( scalar @{$next_f} <= 2 ) {
# # 			my $first_name = ${$next_f}[0]->{name};
# # 			if ( $first_name !~ /reserved/ismx ) {
# # 				last;
# # 			}
# # 		}
# # 		${$idx_ref}++;
# # 	}

# # 	my $len    = ( ${$bytes_ref}[ ${$idx_ref} ] - $start_byte ) + 1;
# # 	my $v_name = $has_error   ? 'error_fault_violation_bits' : 'ping_block';
# # 	my $suffix = ( $len > 1 ) ? "[$len]"                     : $EMPTY_STR;
# # 	return sprintf "\tuint8_t    %s%s;\n", $v_name, $suffix;
# # }
