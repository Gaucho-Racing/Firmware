#!/usr/bin/perl
use strict;
use warnings;
use autodie qw(open close);
use File::Basename;
use File::Path qw(make_path);

main();

sub main {
    my $yaml_path   = $ARGV[0] // 'format.CANdo';
    my $output_path = $ARGV[1] // 'CANDler.h';
    my $prefix      = "GR_OLD";

    ensure_directory_exists($output_path);

    my %desc_map = parse_descriptions($yaml_path);
    generate_header($yaml_path, $output_path, $prefix, \%desc_map);

    return;
}

sub ensure_directory_exists {
    my ($path) = @_;
    my $dir = dirname($path);
    if ($dir && $dir ne '.' && !-d $dir) {
        make_path($dir);
    }
    return;
}

sub parse_descriptions {
    my ($path) = @_;
    my %map;
    my $in_msg_section = 0;

    # Slurp and close immediately to satisfy RequireBriefOpen
    open( my $in, '<', $path );
    my @lines = <$in>;
    close $in;

    for ( my $i = 0 ; $i < scalar @lines ; $i++ ) {
        my $line = $lines[$i];
        chomp $line;

        if ( $line =~ /^Message ID:/ ) {
            $in_msg_section = 1;
            next;
        }
        if ( $line =~ /^\S/ ) {
            $in_msg_section = 0;
        }
        next unless $in_msg_section;

        if ( $line =~ /^\s{2,4}([^:#\s][^:]+):/ ) {
            my $f_name = clean_field_name($1);
            # Move index forward and extract description from the local array
            my ( $desc, $new_i ) = extract_desc_from_array( \@lines, $i );
            $map{$f_name} = $desc if $desc;
            $i = $new_i;
        }
    }
    return %map;
}

sub extract_desc_from_array {
    my ( $lines_ref, $index ) = @_;
    my $description = "";
    my $i = $index;

    while ( ++$i < scalar @{$lines_ref} ) {
        my $sub = $lines_ref->[$i];
        if ( $sub =~ /^\s+#\s*(.*)/ ) {
            $description .= " " . $1;
        }
        # Break if we hit a new field or message
        last if $sub =~ /^\s{2,4}[^#\s]/ || $sub =~ /^\S/;
    }

    $description =~ s/^\s+//;
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
    my $current_msg    = "";
    my @fields         = ();

    for ( my $i = 0 ; $i < scalar @{$lines_ref} ; $i++ ) {
        my $line = $lines_ref->[$i];
        if ( $line =~ /^Message ID:/ ) { $in_msg_section = 1; next; }
        if ( $line =~ /^\S/ ) { $in_msg_section = 0; }
        next unless $in_msg_section;

        if ( $line =~ /^  ([^:#\s][^:]+):$/ ) {
            if ($current_msg) {
                push @output, process_message( $current_msg, \@fields, $d_map, $prefix );
            }
            $current_msg = $1;
            @fields      = ();
        }
        elsif ( $line =~ /^\s{4}([^:#\s][^:]+):/ ) {
            my ( $f_data, $new_i ) = parse_field_details( $lines_ref, $i, $1 );
            push @fields, $f_data if %{$f_data};
            $i = $new_i;
        }
    }

    push @output, process_message( $current_msg, \@fields, $d_map, $prefix ) if $current_msg;
    push @output, "#endif\n";

    write_file( $out_p, \@output );
    return;
}

sub slurp_file {
    my ($path) = @_;
    open( my $fh, '<', $path );
    my @lines = <$fh>;
    close $fh;
    chomp @lines;
    return \@lines;
}

sub write_file {
    my ( $path, $content_ref ) = @_;
    open( my $fh, '>', $path );
    for my $line (@$content_ref) {
        print {$fh} $line or die "Could not write to $path: $!";
    }
    close $fh;
    return;
}

sub parse_field_details {
    my ( $lines_ref, $index, $name ) = @_;
    my ( $start, $data_type ) = ( undef, "u8" );
    my $i = $index;

    while ( ++$i < scalar @{$lines_ref} ) {
        my $sub = $lines_ref->[$i];
        if ( $sub =~ /bit_start:\s*(\d+)/ ) {
            $start = $1;
        }
        if ( $sub =~ /data type:\s*(\w+)/ ) {
            $data_type = $1;
        }
        last if $sub =~ /^\s{4}[^#\s]/ || $sub =~ /^\s{0,2}\S/;
    }
    my %res = defined $start ? ( name => $name, start => $start, type => $data_type ) : ();
    return ( \%res, $i - 1 );
}

sub clean_field_name {
    my ($name) = @_;
    my $clean = lc $name;
    $clean =~ s/[^a-z0-9]/_/g;
    $clean =~ s/_+/_/g;
    $clean =~ s/^_|_$//g;
    return $clean || "unknown_field";
}

sub process_message {
    my ( $name, $f_ref, $d_map, $prefix ) = @_;
    my @buf;
    my $tag = uc( $name =~ s/[^A-Za-z0-9]/_/gr =~ s/_+/_/gr =~ s/^_|_$//gr );

    if ( $name =~ /Cell Data/i ) {
        push @buf, sprintf "/** %s */\ntypedef struct {\n\tstruct {\n\t\tuint8_t voltage;\n\t\tuint8_t temperature;\n\t} cells[32];\n} %s_%s_MSG;\n\n", $name, $prefix, $tag;
        return join '', @buf;
    }

    my %byte_map;
    for my $f (@$f_ref) {
        push @{ $byte_map{ int( $f->{start} / 8 ) } }, $f;
    }

    push @buf, "/** $name */\ntypedef struct {\n";
    my @sorted = sort { $a <=> $b } keys %byte_map;

    for ( my $i = 0 ; $i < scalar @sorted ; $i++ ) {
        push @buf, process_byte_entry( \@sorted, \%byte_map, \$i, $d_map );
    }
    push @buf, "} ${prefix}_${tag}_MSG;\n\n";
    return join '', @buf;
}

sub process_byte_entry {
    my ( $sorted_ref, $map_ref, $idx_ref, $d_map ) = @_;
    my @out;
    my $b_idx  = $sorted_ref->[$$idx_ref];
    my $fields = $map_ref->{$b_idx};

    if ( scalar @{$fields} > 2 ) {
        push @out, handle_multi_field_range( $sorted_ref, $map_ref, $idx_ref );
    }
    else {
        my $f_var = ( scalar @{$fields} == 1 )
          ? clean_field_name( $fields->[0]->{name} )
          : join( '_', map { clean_field_name( $_->{name} ) } @{$fields} );
        $f_var = "_" . $f_var if $f_var =~ /^\d/;

        my $type = ( $fields->[0]->{type} =~ /32/ ) ? "uint32_t"
          : ( $fields->[0]->{type} =~ /16/ )         ? "uint16_t"
          :                                           "uint8_t";
        my $desc = join ' ', map { $d_map->{ clean_field_name( $_->{name} ) } // () } @{$fields};

        push @out, sprintf "\t/** %s (Byte %d) */\n\t%-10s %-30s\n", ( $desc || "Byte $b_idx" ), $b_idx, $type, $f_var . ";";
    }
    return join '', @out;
}

sub handle_multi_field_range {
    my ( $bytes_ref, $map_ref, $idx_ref ) = @_;
    my $start_byte = $bytes_ref->[$$idx_ref];
    my $has_error  = grep { $_->{name} =~ /error|fault|violation/i } @{ $map_ref->{$start_byte} };

    while ( $$idx_ref + 1 < scalar @{$bytes_ref} ) {
        my $next_byte = $bytes_ref->[ $$idx_ref + 1 ];
        my $next_f    = $map_ref->{$next_byte};
        if ( scalar @{$next_f} <= 2 ) {
            last if $next_f->[0]->{name} !~ /reserved/i;
        }
        $$idx_ref++;
    }

    my $len    = ( $bytes_ref->[$$idx_ref] - $start_byte ) + 1;
    my $v_name = $has_error ? "error_fault_violation_bits" : "ping_block";
    return sprintf "\tuint8_t    %s%s;\n", $v_name, ( $len > 1 ? "[$len]" : "" );
}
