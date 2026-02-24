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

# --- Step 1: Pre-parse for descriptions ---
while ( my $line = <$in> ) {
    chomp($line);
    if    ( $line =~ /^Message ID:/ )                   { $in_msg_section = 1; next; }
    elsif ( $line =~ /^\S/ && $line !~ /^Message ID:/ ) { $in_msg_section = 0; }
    next unless $in_msg_section;

    # Match "    Field Name:"
    if ( $line =~ /^\s{2,4}([^:#\s][^:]+):/ ) {
        my $raw_name = $1;
        my $f_name = lc($raw_name);
        $f_name =~ s/[^a-z0-9]/_/g;
        $f_name =~ s/_+/_/g;
        $f_name =~ s/^_|_$//g;

        my $description = "";
        my $pos = tell($in);
        while ( my $sub = <$in> ) {
            if ( $sub =~ /^\s+#\s*(.*)/ ) {
                $description .= " " . $1;
            }
            # Stop if we hit a new field or a new message
            last if $sub =~ /^\s{2,4}[^#\s]/ || $sub =~ /^\S/;
        }
        seek( $in, $pos, 0 );
        $description =~ s/^\s+//;
        $desc_map{$f_name} = $description if $description;
    }
}

# --- Step 2: Generate the Header ---
seek( $in, 0, 0 );
$in_msg_section = 0;
my $current_msg = "";
my @fields      = ();

print $out "/* Auto-generated header file */\n";
print $out "#ifndef ${prefix}_MESSAGES_H\n";
print $out "#define ${prefix}_MESSAGES_H\n\n";
print $out "#include <stdint.h>\n\n";

while ( my $line = <$in> ) {
    chomp($line);
    if    ( $line =~ /^Message ID:/ )                   { $in_msg_section = 1; next; }
    elsif ( $line =~ /^\S/ && $line !~ /^Message ID:/ ) { $in_msg_section = 0; }
    next unless $in_msg_section;

    # Match "  Message Name:" (2 spaces)
    if ( $line =~ /^  ([^:#\s][^:]+):$/ ) {
        process_bytes_exact( $out, $current_msg, \@fields, \%desc_map ) if $current_msg;
        $current_msg = $1;
        @fields      = ();
    }
    # Match "    Field Name:" (4 spaces)
    elsif ( $line =~ /^\s{4}([^:#\s][^:]+):/ ) {
        my $f_name = $1;
        my $start = undef;
        my $data_type = "u8";

        my $pos = tell($in);
        while ( my $sub = <$in> ) {
            if ($sub =~ /bit_start:\s*(\d+)/) {
                $start = $1;
            }
            if ($sub =~ /data type:\s*(\w+)/) {
                $data_type = $1;
            }
            # Break if we hit a new field or message
            last if $sub =~ /^\s{4}[^#\s]/ || $sub =~ /^\s{0,2}\S/;
        }
        seek( $in, $pos, 0 );

        if (defined $start) {
            push @fields, { name => $f_name, start => $start, type => $data_type };
        }
    }
}

# Final call for the last message
process_bytes_exact( $out, $current_msg, \@fields, \%desc_map ) if $current_msg;
print $out "#endif\n";

# --- EXACT logic provided in your original script ---
sub process_bytes_exact {
    my ( $fh, $name, $f_ref, $d_map ) = @_;
    return if !$name || $name =~ /Message ID/;
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
            my $start_byte          = $b_idx;
            my $next_real_data_byte = $b_idx + 1;
            my $has_error           = grep { $_->{name} =~ /error|fault|violation/i } @fields;
            for ( my $j = $i + 1 ; $j < @sorted_bytes ; $j++ ) {
                my $look_idx = $sorted_bytes[$j];
                my @look_f   = @{ $byte_map{$look_idx} };
                if ( scalar @look_f > 2 || ( scalar @look_f == 1 && $look_f[0]->{name} =~ /reserved/i ) ) {
                    $next_real_data_byte = $look_idx + 1;
                    $i++;
                }
                else { $next_real_data_byte = $look_idx; last; }
            }
            my $len    = $next_real_data_byte - $start_byte;
            my $v_name = $has_error ? "error_fault_violation_bits" : "ping_block";
            printf( $fh "\tuint8_t    %s%s;\n", $v_name, ( $len > 1 ? "[$len]" : "" ) );
            next;
        }

        my $f_var = ( scalar @fields == 1 ) ? lc( $fields[0]->{name} ) : join( '_', map { lc( $_->{name} ) } @fields );
        $f_var =~ s/[^a-z0-9]/_/g; $f_var =~ s/_+/_/g; $f_var =~ s/^_|_$//g;
        $f_var = "_" . $f_var if $f_var =~ /^\d/;

        my $type       = "uint8_t";
        my $final_desc = "";
        foreach my $f (@fields) {
            $type = "uint16_t" if $f->{type} =~ /16/;
            $type = "uint32_t" if $f->{type} =~ /32/;
            my $clean_name = lc( $f->{name} ) =~ s/[^a-z0-9]/_/gr =~ s/_+/_/gr =~ s/^_|_$//gr;
            $final_desc .= " " . $d_map->{$clean_name} if $d_map->{$clean_name};
        }
        $final_desc =~ s/^\s+//;

        if   ($final_desc) { print $fh "\t/** $final_desc (Byte $b_idx) */\n"; }
        else               { print $fh "\t/** Byte $b_idx */\n"; }
        printf( $fh "\t%-10s %-30s\n", $type, $f_var . ";" );
    }
    print $fh "} ${prefix}_${struct_tag}_MSG;\n\n";
}
