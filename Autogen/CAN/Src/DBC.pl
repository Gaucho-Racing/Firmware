#!/usr/bin/env perl
use strict;
use warnings;

# Bit size mapping
my %TYPE_BITS = (
    'b'=>1, 'u4'=>4, 's4'=>4, 'u8'=>8, 's8'=>8, 'i8'=>8,
    'u16'=>16, 's16'=>16, 'i16'=>16, 'u32'=>32, 's32'=>32, 'i32'=>32,
    'u64'=>64, 's64'=>64, 'i64'=>64, 'double'=>64, 's'=>64
);

sub normalize {
    my ($val) = @_;
    return "Unknown" unless defined $val && $val ne '';
    $val =~ s/^\s+|\s+$//g;
    $val =~ s/[\s.\-]+/_/g;
    $val =~ s/[^a-zA-Z0-9_]//g;
    return $val;
}

sub parse_eq {
    my ($eq) = @_;
    return (1, 0) unless defined $eq && $eq ne '-';
    $eq =~ s/["']//g;
    # Matches 20x/51
    if ($eq =~ /^(\d+)x\/(\d+)$/) { return ($1/$2, 0); }
    # Matches x/655.35 or x/256
    if ($eq =~ /^x\/([\d.]+)$/)     { return (1/$1, 0); }
    # Matches abs(x)/10
    if ($eq =~ /^abs\(x\)\/([\d.]+)$/) { return (1/$1, 0); }
    # Matches 0.25x
    if ($eq =~ /^([\d.]+)x$/)       { return ($1 + 0, 0); }
    # Matches 0.01x+2 or 0.1x-3276.8
    if ($eq =~ /^([\d.]+)x\+([\d.]+)$/) { return ($1 + 0, $2 + 0); }
    if ($eq =~ /^([\d.]+)x-([\d.]+)$/)  { return ($1 + 0, -($2 + 0)); }
    if ($eq =~ /^x-([\d.]+)$/)      { return (1, -($1 + 0)); }
    return (1, 0);
}

sub parse_input {
    my ($file) = @_;
    my %data = ( routing => [], messages => {}, custom => {}, grid => {} );
    open my $fh, '<', $file or die "Cannot open $file: $!";

    my ($sec, $cur_msg, $cur_sig, $sender, $bus, $target) = ('','','','','','');

    while (<$fh>) {
        chomp;
        next if /^\s*$/ || /^\s*#/;
        my ($indent) = /^(\s*)/;
        my $ind = length($indent);
        my $line = $_; $line =~ s/^\s+//;

        if ($ind == 0 && $line =~ /^([^:]+):/) {
            $sec = $1;
            next;
        }

        if ($sec eq 'routing') {
            next if $line =~ /^messages:/;
            if ($ind == 4 && $line =~ /^([^:]+):/) { $sender = $1; }
            elsif ($ind == 6 && $line =~ /^([^:]+):/) { $bus = $1; }
            elsif ($ind == 8 && $line =~ /^([^:]+):/) {
                $target = $1;
                # Handle inline lists like SAM1: [ { msg: Debug FD }, ... ]
                if ($line =~ /\[\s*\{\s*msg:\s*([^}]+)\s*\}\s*\]/) {
                    push @{$data{routing}}, { sender => $sender, bus => $bus, target => $target, msg => $1 };
                }
            }
            elsif ($line =~ /^- msg:\s*(.+)/) {
                push @{$data{routing}}, { sender => $sender, bus => $bus, target => $target, msg => $1 };
            } elsif ($line =~ /^can_id_override:\s*(.+)/) {
                $data{routing}->[-1]->{override} = $1 if @{$data{routing}};
            }
        } elsif ($sec eq 'Message ID') {
            if ($ind == 2 && $line =~ /^([^:]+):/) {
                $cur_msg = $1;
                $data{messages}{$cur_msg} = {sigs=>{}};
            }
            elsif ($line =~ /^MSG ID:\s*(.+)/) { $data{messages}{$cur_msg}{id} = $1; }
            elsif ($line =~ /^MSG LENGTH:\s*(.+)/) { $data{messages}{$cur_msg}{len} = $1; }
            elsif ($line =~ /^([^:]+):\s*#\s*bit start\s*([\d\-]+)/) {
                $cur_sig = $1; my $bs = $2; $bs =~ s/-.*//;
                $data{messages}{$cur_msg}{sigs}{$cur_sig} = {start => $bs};
            } elsif ($ind == 6 && $line =~ /^([^:]+):\s*(.+)/) {
                $data{messages}{$cur_msg}{sigs}{$cur_sig}{$1} = $2;
            }
        } elsif ($sec eq 'Custom CAN ID') {
            if ($ind == 2 && $line =~ /^["']?([^"':]+)["']?:/) { $cur_msg = $1; $data{custom}{$cur_msg} = {sigs=>[]}; }
            elsif ($line =~ /^CAN ID:\s*(.+)/) { $data{custom}{$cur_msg}{id} = $1; }
            elsif ($line =~ /^Length:\s*(.+)/) { $data{custom}{$cur_msg}{len} = $1; }
            elsif ($line =~ /^name:\s*["']?([^"']+)["']?/) { push @{$data{custom}{$cur_msg}{sigs}}, {name=>$1}; }
            elsif ($line =~ /^bit_start:\s*([\d\-]+)/) {
                my $bs = $1; $bs =~ s/-.*//;
                $data{custom}{$cur_msg}{sigs}->[-1]->{start} = $bs if @{$data{custom}{$cur_msg}{sigs}};
            }
        } elsif ($sec eq 'GR ID') {
            if ($line =~ /^([^:]+):\s*["']?([^"']+)["']?/) {
                my ($k, $v) = ($1, $2); $k =~ s/\s+$//;
                $data{grid}{$k} = $v;
            }
        }
    }
    return \%data;
}

# --- Main execution ---
my $input_file = $ARGV[0] or die "Usage: perl script.pl <input.yaml> [output.dbc]\n";
my $output_file = $ARGV[1] // "output.dbc";
my $d = parse_input($input_file);

open(my $out, '>', $output_file) or die $!;
print $out "VERSION \"\"\n\nNS_ :\n\nBS_:\n\nBU_: " . join(' ', sort grep { $_ ne 'ALL' } map { normalize($_) } keys %{$d->{grid}}) . " ALL\n\n";

foreach my $r (@{$d->{routing}}) {
    my $m_name = $r->{msg};
    # Prioritize Custom ID definitions if they exist for specific logic
    my $is_custom = exists $d->{custom}{$m_name};
    my $m_def = $is_custom ? $d->{custom}{$m_name} : $d->{messages}{$m_name};
    next unless $m_def;

    my $can_id;
    if ($r->{override}) {
        $can_id = ($r->{override} =~ /0x/i) ? hex($r->{override}) : int($r->{override});
    } elsif ($is_custom) {
        $can_id = ($m_def->{id} =~ /0x/i) ? hex($m_def->{id}) : int($m_def->{id});
    } else {
        my $s_val = $d->{grid}{$r->{sender}} // "0x00";
        my $t_val = $d->{grid}{$r->{target}} // "0x00";
        my $m_val = $m_def->{id} // "0x00";
        # Standard ID assembly logic: [Sender(8bit)][Msg(8bit)][Target(8bit)]
        $can_id = (hex($s_val) << 16) | (hex($m_val) << 8) | hex($t_val);
    }

    # Set Extended Bit if ID is > 11-bit range
    $can_id |= 0x80000000 if $can_id > 0x7FF;

    my $dbc_msg_name = normalize($r->{sender}) . "_" . normalize($m_name) . "_to_" . normalize($r->{target});
    printf $out "BO_ %u %s: %d %s\n", $can_id, $dbc_msg_name, ($m_def->{len}//8), normalize($r->{sender});

    my @sigs = $is_custom ? @{$m_def->{sigs}} : map { {name=>$_, %{$m_def->{sigs}{$_}}} } keys %{$m_def->{sigs}};
    foreach my $s (sort { ($a->{start}//0) <=> ($b->{start}//0) } @sigs) {
        next if $s->{name} =~ /Reserved/i;

        my ($f, $o) = parse_eq($s->{'map equation'});
        my $raw_type = $s->{'data type'} // 'u8';
        my $bits = 8;
        if ($raw_type =~ /^(\w+)\[(\d+)\]$/) { $bits = ($TYPE_BITS{$1}//8) * $2; }
        else { $bits = $TYPE_BITS{$raw_type} // 8; }

        my $unit = ($s->{units} && $s->{units} ne '-' && $s->{units} ne 'Enum') ? $s->{units} : "";
        $unit =~ s/'//g;

        printf $out " SG_ %s : %d|%d\@1%s (%g,%g) [%s|%s] \"%s\" %s\n",
            normalize($s->{name}),
            $s->{start},
            $bits,
            ($raw_type =~ /^[is]/ ? '-' : '+'),
            $f, $o,
            ($s->{'scaled min'}//0),
            ($s->{'scaled max'}//0),
            $unit,
            normalize($r->{target});
    }
    print $out "\n";
}
close($out);
