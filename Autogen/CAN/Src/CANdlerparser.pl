#!/usr/bin/perl
use strict;
use warnings;
use File::Basename;
use File::Path qw(make_path);

# --- Configuration ---
my $yaml_path   = $ARGV[0] // 'format.CANdo';
my $output_path = $ARGV[1] // 'CANDler.h';
my $prefix      = "GR_OLD";

my $dir = dirname($output_path);
make_path($dir) if $dir && $dir ne '.' && !-d $dir;

open( my $in,  '<', $yaml_path )   or die "Can't open YAML: $!";
open( my $out, '>', $output_path ) or die "Can't open Output: $!";

my $in_msg_section = 0;
my %desc_map;

# --- Step 1: Pre-parse format.CANdo to map descriptions ---
while ( my $line = <$in> ) {
    chomp($line);
    if ( $line =~ /^Message ID:/ ) { $in_msg_section = 1; next; }
    elsif ( $line =~ /^\w/ && $line !~ /^Message ID:/ ) { $in_msg_section = 0; }
    next unless $in_msg_section;

    if ( $line =~ /^\s{4}([^:]+): # bit start ([\d-]+)/ ) {
        my $f_name = lc($1);
        $f_name =~ s/[^a-z0-9]/_/g;
        $f_name =~ s/_+/_/g;
        $f_name =~ s/^_|_$//g;

        my $description = "";
        my $pos = tell($in);
        while ( my $sub = <$in> ) {
            if ( $sub =~ /^\s+#\s*(.*)/ ) {
                $description .= " " . $1;
            }
            last if $sub =~ /# bit start/ || $sub =~ /^  \w/ || $sub !~ /^\s+/;
        }
        seek( $in, $pos, 0 );
        $description =~ s/^\s+//;
        $desc_map{$f_name} = $description if $description;
    }
}

# --- Step 2: Generate the Header based on your EXACT template ---
seek($in, 0, 0); # Reset to start of YAML to parse message structure
$in_msg_section = 0;
my $current_msg = "";
my @fields = ();

print $out "/* Auto-generated header file */\n";
print $out "#ifndef ${prefix}_MESSAGES_H\n";
print $out "#define ${prefix}_MESSAGES_H\n\n";
print $out "#include <stdint.h>\n\n";

while ( my $line = <$in> ) {
    chomp($line);
    if ( $line =~ /^Message ID:/ ) { $in_msg_section = 1; next; }
    elsif ( $line =~ /^\w/ && $line !~ /^Message ID:/ ) { $in_msg_section = 0; }
    next unless $in_msg_section;

    if ( $line =~ /^  ([^:]+):$/ ) {
        process_bytes_exact( $out, $current_msg, \@fields, \%desc_map ) if $current_msg;
        $current_msg = $1;
        @fields = ();
    }
    elsif ( $line =~ /^\s{4}([^:]+): # bit start (\d+)/ ) {
        my ($f_name, $start) = ($1, $2);
        my $data_type = "u8";
        my $pos = tell($in);
        while ( my $sub = <$in> ) {
            if ( $sub =~ /data type: (\w+)/ ) { $data_type = $1; last; }
            last if $sub =~ /# bit start/ || $sub =~ /^  \w/;
        }
        seek($in, $pos, 0);
        push @fields, { name => $f_name, start => $start, type => $data_type };
    }
}
process_bytes_exact( $out, $current_msg, \@fields, \%desc_map ) if $current_msg;
print $out "#endif\n";

sub process_bytes_exact {
    my ( $fh, $name, $f_ref, $d_map ) = @_;
    return if $name =~ /Message ID/;
    my $struct_tag = uc( $name =~ s/[^A-Z0-9]/_/gr =~ s/_+/_/gr =~ s/^_|_$//gr );

    if ( $name =~ /Cell Data/i ) {
        print $fh "/** $name */\ntypedef struct {\n";
        print $fh "\tstruct {\n\t\tuint8_t voltage;\n\t\tuint8_t temperature;\n\t} cells[32];\n";
        print $fh "} ${prefix}_${struct_tag}_MSG;\n\n";
        return;
    }

    my %byte_map;
    foreach my $f (@$f_ref) {
        my $byte_num = int( $f->{start} / 8 );
        push @{ $byte_map{$byte_num} }, $f;
    }

    print $fh "/** $name */\ntypedef struct {\n";
    my @sorted_bytes = sort { $a <=> $b } keys %byte_map;

    for ( my $i = 0 ; $i < @sorted_bytes ; $i++ ) {
        my $b_idx  = $sorted_bytes[$i];
        my @fields = @{ $byte_map{$b_idx} };

        if ( scalar @fields > 2 ) {
            my $start_byte = $b_idx;
            my $next_real_data_byte = $b_idx + 1;
            my $has_error = grep { $_->{name} =~ /error|fault|violation/i } @fields;
            for ( my $j = $i + 1 ; $j < @sorted_bytes ; $j++ ) {
                my $look_idx = $sorted_bytes[$j];
                my @look_f = @{ $byte_map{$look_idx} };
                if ( scalar @look_f > 2 || (scalar @look_f == 1 && $look_f[0]->{name} =~ /reserved/i) ) {
                    $next_real_data_byte = $look_idx + 1; $i++;
                } else { $next_real_data_byte = $look_idx; last; }
            }
            my $len = $next_real_data_byte - $start_byte;
            my $v_name = $has_error ? "error_fault_violation_bits" : "ping_block";
            printf( $fh "\tuint8_t    %s%s;\n", $v_name, ($len > 1 ? "[$len]" : "") );
            next;
        }

        my $f_var = (scalar @fields == 1) ? lc($fields[0]->{name}) : join('_', map { lc($_->{name}) } @fields);
        $f_var =~ s/[^a-z0-9]/_/g; $f_var =~ s/_+/_/g; $f_var =~ s/^_|_$//g;
        $f_var = "_" . $f_var if $f_var =~ /^\d/;

        my $type = "uint8_t";
        my $final_desc = "";
        foreach my $f (@fields) {
            $type = "uint16_t" if $f->{type} =~ /16/;
            $type = "uint32_t" if $f->{type} =~ /32/;
            my $clean_name = lc($f->{name}) =~ s/[^a-z0-9]/_/gr =~ s/_+/_/gr =~ s/^_|_$//gr;
            $final_desc .= " " . $d_map->{$clean_name} if $d_map->{$clean_name};
        }
        $final_desc =~ s/^\s+//;

        if ($final_desc) { print $fh "\t/** $final_desc (Byte $b_idx) */\n"; }
        else { print $fh "\t/** Byte $b_idx */\n"; }
        printf( $fh "\t%-10s %-30s\n", $type, $f_var . ";" );
    }
    print $fh "} ${prefix}_${struct_tag}_MSG;\n\n";
}



 #!/usr/bin/perl
# use strict;
# use warnings;
# use File::Basename;
# use File::Path qw(make_path);

# # --- Configuration ---
# my $yaml_path   = $ARGV[0] // 'format.CANdo';    # First argument: Input
# my $output_path = $ARGV[1] // 'CANDler.h';       # Second argument: Output
# my $prefix      = "GR_OLD";

# my $dir = dirname($output_path);
# make_path($dir) if $dir && $dir ne '.' && !-d $dir;

# # Now use these variables in your open calls:
# open( my $in,  '<', $yaml_path )   or die "Can't open YAML: $!";
# open( my $out, '>', $output_path ) or die "Can't open Output: $!";

# my $in_msg_section = 0;
# my $current_msg    = "";
# my @fields         = ();

# print $out "/* Auto-generated header file */\n";
# print $out "#ifndef ${prefix}_MESSAGES_H\n";
# print $out "#define ${prefix}_MESSAGES_H\n\n";
# print $out "#include <stdint.h>\n\n";

# while ( my $line = <$in> ) {
# 	chomp($line);
# 	if    ( $line =~ /^Message ID:/ )                   { $in_msg_section = 1; next; }
# 	elsif ( $line =~ /^\w/ && $line !~ /^Message ID:/ ) { $in_msg_section = 0; }
# 	next unless $in_msg_section;

# 	if ( $line =~ /^  ([^:]+):$/ ) {
# 		process_bytes( $out, $current_msg, \@fields ) if $current_msg;
# 		$current_msg = $1;
# 		@fields      = ();
# 	}
# 	elsif ( $line =~ /^\s{4}([^:]+): # bit start (\d+)/ ) {
# 		my ( $f_name, $start ) = ( $1, $2 );
# 		my $data_type = "u8";
# 		my $pos       = tell($in);
# 		while ( my $sub = <$in> ) {
# 			if ( $sub =~ /data type: (\w+)/ ) { $data_type = $1; last; }
# 			last if $sub =~ /# bit start/ || $sub =~ /^  \w/;
# 		}
# 		seek( $in, $pos, 0 );
# 		push @fields, { name => $f_name, start => $start, type => $data_type };
# 	}
# }
# process_bytes( $out, $current_msg, \@fields ) if $current_msg;
# print $out "#endif\n";

# sub process_bytes {
# 	my ( $fh, $name, $f_ref ) = @_;
# 	return if $name =~ /Message ID/;

# 	my $struct_tag = uc( $name =~ s/[^A-Z0-9]/_/gr =~ s/_+/_/gr =~ s/^_|_$//gr );

# 	# --- SPECIAL CASE: ACU Cell Data Messages ---
# 	if ( $name =~ /Cell Data/i ) {
# 		print $fh "/** $name */\ntypedef struct {\n";
# 		print $fh "\tstruct {\n\t\tuint8_t voltage;\n\t\tuint8_t temperature;\n\t} cells[32];\n";
# 		print $fh "} ${prefix}_${struct_tag}_MSG;\n\n";
# 		return;
# 	}

# 	my %byte_map;
# 	foreach my $f (@$f_ref) {
# 		my $byte_num = int( $f->{start} / 8 );
# 		push @{ $byte_map{$byte_num} }, $f;
# 	}

# 	print $fh "/** $name */\ntypedef struct {\n";
# 	my @sorted_bytes = sort { $a <=> $b } keys %byte_map;

# 	for ( my $i = 0 ; $i < @sorted_bytes ; $i++ ) {
# 		my $b_idx  = $sorted_bytes[$i];
# 		my @fields = @{ $byte_map{$b_idx} };

# 		# 1. BLOCK LOGIC (Triggered for 3+ fields in a byte)
# 		if ( scalar @fields > 2 ) {
# 			my $start_byte          = $b_idx;
# 			my $next_real_data_byte = $b_idx + 1;    # Default to just this byte

# 			# Check for specific keywords
# 			my $has_error_keywords = grep { $_->{name} =~ /error|fault|violation/i } @fields;

# 			# Look ahead to see if we should swallow consecutive bytes
# 			for ( my $j = $i + 1 ; $j < @sorted_bytes ; $j++ ) {
# 				my $look_idx    = $sorted_bytes[$j];
# 				my @look_fields = @{ $byte_map{$look_idx} };

# 				if ( grep { $_->{name} =~ /error|fault|violation/i } @look_fields ) {
# 					$has_error_keywords = 1;
# 				}

# 				my $is_reserved = ( scalar @look_fields == 1 && $look_fields[0]->{name} =~ /reserved/i );
# 				my $is_crowded  = ( scalar @look_fields > 2 );

# 				if ( $is_reserved || $is_crowded ) {
# 					$next_real_data_byte = $look_idx + 1;
# 					$i++;    # Advance the outer loop
# 				}
# 				else {
# 					$next_real_data_byte = $look_idx;
# 					last;
# 				}
# 			}

# 			my $len      = $next_real_data_byte - $start_byte;
# 			my $var_name = $has_error_keywords ? "error_fault_violation_bits" : "ping_block";

# 			# If it's only 1 byte, we don't necessarily need the [1] array,
# 			# but usually, it's safer for consistency.
# 			if ( $len > 1 ) {
# 				print $fh "\tuint8_t    $var_name\[$len];\n";
# 			}
# 			else {
# 				print $fh "\tuint8_t    $var_name;\n";
# 			}
# 			next;
# 		}

# 		# 2. COMBINE LOGIC (For 1 or 2 fields)
# 		my $final_var_name;
# 		if ( scalar @fields == 1 ) {
# 			$final_var_name = lc( $fields[0]->{name} );
# 		}
# 		else {
# 			$final_var_name = join( '_', map { lc( $_->{name} ) } @fields );
# 		}

# 		# Cleanup and Protection
# 		$final_var_name =~ s/[^a-z0-9]/_/g;
# 		$final_var_name =~ s/_+/_/g;
# 		$final_var_name =~ s/^_|_$//g;
# 		if ( $final_var_name =~ /^\d/ ) { $final_var_name = "_" . $final_var_name; }

# 		my $type = "uint8_t";
# 		foreach (@fields) {
# 			$type = "uint16_t" if $_->{type} =~ /16/;
# 			$type = "uint32_t" if $_->{type} =~ /32/;
# 		}

# 		printf( $fh "\t%-10s %-30s /** Byte $b_idx */\n", $type, $final_var_name . ";" );
# 	}

# 	print $fh "} ${prefix}_${struct_tag}_MSG;\n\n";
# }
