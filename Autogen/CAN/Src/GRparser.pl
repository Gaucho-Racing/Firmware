#!/usr/bin/env perl
use strict;
use warnings;
use File::Basename;
use File::Path qw(make_path);
use English    qw(-no_match_vars);
use autodie    qw(open close);

main();

sub main {
    my $yaml_path   = $ARGV[0] // 'Doc/format.CANdo';
    my $output_path = $ARGV[1] // 'Inc/GR_IDS.h';
    my $dir         = dirname($output_path);

    if ( $dir && $dir ne q{.} && !-d $dir ) {
        make_path($dir);
    }

    if ( !-e $yaml_path ) {
        die "CANfigurator Error: Could not find YAML file at: $yaml_path\n";
    }

    my $msg_ids_ref  = parse_yaml_briefly($yaml_path);
    my @header_lines = generate_gr_header_content($msg_ids_ref);
    write_output_briefly( $output_path, \@header_lines );

    print "CANfigurator: Successfully generated $output_path\n";
    return;
}

sub parse_yaml_briefly {
    my ($path) = @_;

    open my $fh, '<', $path;
    my @lines = <$fh>;
    close $fh;

    my @found_ids;
    my %seen_ids;   # Tracker for ID values (0x2A)
    my %seen_names; # Tracker for Node names (CCU)
    my $in_section = 0;

    for my $line (@lines) {
        if ( $line =~ /^GR[ ]ID:/smx ) {
            $in_section = 1;
            next;
        }
        # Exit section if we hit a new top-level key
        last if $in_section && $line =~ /^\w/smx;
        next if !$in_section;

        if ( $line =~ /^ \s+ ([^:]+) : \s* ["']? ( [^"'\s#]+ ) ["']? /smx ) {
            my $name = $1;
            my $val  = $2;

            $name =~ s/^\s+|\s+$//gsmx;
            $val  =~ s/^\s+|\s+$//gsmx;

            # --- THE FIX FOR ISSUE #373 ---
            if ( $seen_ids{$val} ) {
                warn "GRparser: Skipping duplicate ID $val ($name)\n";
                next;
            }
            if ( $seen_names{$name} ) {
                warn "GRparser: Skipping duplicate Name $name\n";
                next;
            }

            push @found_ids, { name => $name, id => $val };
            $seen_ids{$val}    = 1;
            $seen_names{$name} = 1;
        }
    }
    return \@found_ids;
}

sub generate_gr_header_content {
    my ($ids_ref) = @_;
    my @header_lines;

    push @header_lines, "// Auto-generated GR ID enum header\n";
    push @header_lines, "#ifndef GR_IDS_H\n";
    push @header_lines, "#define GR_IDS_H\n\n";
    push @header_lines, "typedef enum {\n";

    my @sorted = sort { $a->{name} cmp $b->{name} } @{$ids_ref};

    for my $item (@sorted) {
        my $const_name = uc $item->{name};
        $const_name =~ s/[[:^alnum:]]/_/gsmx;
        $const_name =~ s/_+/_/g;

        if ( defined $item->{id} && $item->{id} ne q{} ) {
            push @header_lines, sprintf "    %s_NODE_ID = %s,\n", $const_name, $item->{id};
        }
    }

    push @header_lines, "} GRCAN_NODE_ID_t;\n\n";
    push @header_lines, "#endif // GR_IDS_H\n";

    return @header_lines;
}

sub write_output_briefly {
    my ( $path, $lines_ref ) = @_;
    my $content = join q{}, @{$lines_ref};

    open my $out, '>', $path;
    print {$out} $content;
    close $out;

    return;
}





# #!/usr/bin/env perl
# use strict;
# use warnings;
# use File::Basename;
# use File::Path qw(make_path);
# use English    qw(-no_match_vars);
# use autodie    qw(open close);

# main();

# sub main {
# 	my $yaml_path   = $ARGV[0] // 'format.CANdo';
# 	my $output_path = $ARGV[1] // 'GR_IDS.h';
# 	my $dir         = dirname($output_path);

# 	if ( $dir && $dir ne q{.} && !-d $dir ) {
# 		make_path($dir);
# 	}

# 	if ( !-e $yaml_path ) {
# 		die "CANfigurator Error: Could not find YAML file at: $yaml_path\n";
# 	}

# 	my $msg_ids_ref  = parse_yaml_briefly($yaml_path);
# 	my @header_lines = generate_gr_header_content($msg_ids_ref);
# 	write_output_briefly( $output_path, \@header_lines );

# 	my $log_success = print "CANfigurator: Successfully generated $output_path\n";
# 	if ( !$log_success ) {
# 		die "Failed to write to STDOUT: $OS_ERROR";
# 	}
# 	return;
# }

# sub parse_yaml_briefly {
# 	my ($path) = @_;

# 	open my $fh, '<', $path;
# 	my @lines = <$fh>;
# 	close $fh;

# 	my @found_ids;
# 	my $in_section = 0;

# 	for my $line (@lines) {
# 		if ( $line =~ /^GR[ ]ID:/smx ) {
# 			$in_section = 1;
# 			next;
# 		}
# 		last if $in_section && $line =~ /^\w/smx;
# 		next if !$in_section;

# 		# Refined regex: match key, colon, then capture value (stripping optional quotes)
# 		if ( $line =~ /^ \s+ ([^:]+) : \s* ["']? ( [^"'\s#]+ ) ["']? /smx ) {
# 			my $name = $1;
# 			my $val  = $2;

# 			$name =~ s/^\s+|\s+$//gsmx;
# 			$val  =~ s/^\s+|\s+$//gsmx;

# 			push @found_ids, { name => $name, id => $val };
# 		}
# 	}
# 	return \@found_ids;
# }

# sub generate_gr_header_content {
# 	my ($ids_ref) = @_;
# 	my @header_lines;

# 	push @header_lines, "// Auto-generated GR ID enum header\n";
# 	push @header_lines, "#ifndef GR_IDS_H\n";
# 	push @header_lines, "#define GR_IDS_H\n\n";
# 	push @header_lines, "typedef enum {\n";

# 	my @sorted = sort { $a->{name} cmp $b->{name} } @{$ids_ref};

# 	for my $item (@sorted) {
# 		my $const_name = $item->{name};
# 		$const_name =~ s/[[:^alnum:]]/_/gsmx;

# 		if ( defined $item->{id} && $item->{id} ne q{} ) {
# 			push @header_lines, sprintf "    %s = %s,\n", $const_name, $item->{id};
# 		}
# 	}

# 	push @header_lines, "} GRCAN_NODE_ID;\n\n";
# 	push @header_lines, "#endif // GR_IDS_H\n";

# 	return @header_lines;
# }

# sub write_output_briefly {
# 	my ( $path, $lines_ref ) = @_;
# 	my $content = join q{}, @{$lines_ref};

# 	open my $out, '>', $path;
# 	my $success = print {$out} $content;
# 	if ( !$success ) {
# 		die "Failed to write to $path: $OS_ERROR";
# 	}
# 	close $out;

# 	return;
# }
