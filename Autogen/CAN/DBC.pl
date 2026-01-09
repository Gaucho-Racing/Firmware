#!/usr/bin/env perl
use strict;
use warnings;

# Usage: perl generate_dbc.pl format.yaml routing.yaml output.dbc

my $format_file = $ARGV[0] // 'format.yaml';
my $routing_file = $ARGV[1] // $format_file;
my $output_file = $ARGV[2] // 'output.dbc';

# Simple YAML parser (handles the specific structure we need)
sub parse_yaml {
    my $file = shift;
    open my $fh, '<', $file or die "Cannot open $file: $!";
    
    my %data;
    my @stack = (\%data);
    my @indent_stack = (-1);
    my $current_key;
    
    while (my $line = <$fh>) {
        chomp $line;
        next if $line =~ /^\s*$/;      # skip blank
        next if $line =~ /^\s*#/;       # skip comments
        
        # Get indent level
        my ($indent) = $line =~ /^(\s*)/;
        my $indent_len = length($indent);
        $line =~ s/^\s+//;
        
        # Pop stack if dedented
        while ($indent_len <= $indent_stack[-1] && @indent_stack > 1) {
            pop @stack;
            pop @indent_stack;
        }
        
        # List item
        if ($line =~ /^-\s*(.*)/) {
            my $rest = $1;
            my $parent = $stack[-1];
            
            if (ref($parent) eq 'ARRAY') {
                if ($rest =~ /^(\w+):\s*(.*)/) {
                    # - key: value (start of hash in array)
                    my $new_hash = { $1 => $2 eq '' ? {} : $2 };
                    push @$parent, $new_hash;
                    push @stack, $new_hash;
                    push @indent_stack, $indent_len;
                    $current_key = $1;
                } else {
                    push @$parent, $rest;
                }
            }
            next;
        }
        
        # Key: value
        if ($line =~ /^(.+?):\s*(.*)/) {
            my ($key, $value) = ($1, $2);
            my $parent = $stack[-1];
            
            # Check for "# bit start N" pattern in value/comment
            my $bit_start_val;
            if ($value =~ /#\s*bit start\s+(\d+)/) {
                $bit_start_val = $1;
            }
            
            if ($value eq '' || $value =~ /^#/) {
                # New section
                $parent->{$key} = {};
                if (defined $bit_start_val) {
                    $parent->{$key}{'_bit_start'} = $bit_start_val;
                }
                push @stack, $parent->{$key};
                push @indent_stack, $indent_len;
                $current_key = $key;
            } elsif ($value eq '|' || $value eq '>') {
                # Multi-line (skip for now)
                $parent->{$key} = '';
            } else {
                # Clean value
                $value =~ s/\s*#.*$//;  # remove inline comments
                $value =~ s/^["']|["']$//g;  # remove quotes
                $parent->{$key} = $value;
            }
        }
    }
    close $fh;
    return \%data;
}

# Parse routing yaml (simpler structure)
sub parse_routing_yaml {
    my $file = shift;
    open my $fh, '<', $file or die "Cannot open $file: $!";
    
    my @routing;
    my $current;
    
    while (my $line = <$fh>) {
        chomp $line;
        next if $line =~ /^\s*$/;
        next if $line =~ /^\s*#/;
        next if $line =~ /^routing:/;
        
        if ($line =~ /^\s*-\s*sender:\s*(.+)/) {
            $current = { sender => $1 };
            push @routing, $current;
        } elsif ($line =~ /^\s+msg:\s*(.+)/) {
            $current->{msg} = $1;
        } elsif ($line =~ /^\s+target:\s*(.+)/) {
            $current->{target} = $1;
        } elsif ($line =~ /^\s+bus:\s*(.+)/) {
            $current->{bus} = $1;
        } elsif ($line =~ /^\s+can_id_override:\s*(.+)/) {
            $current->{can_id_override} = $1;
        }
    }
    close $fh;
    return \@routing;
}

print "Parsing $format_file...\n";
my $format = parse_yaml($format_file);

my $routing;
if (-e $routing_file && $routing_file ne $format_file) {
    print "Parsing $routing_file...\n";
    $routing = parse_routing_yaml($routing_file);
} else {
    print "Reading routing from format file...\n";
    $routing = parse_routing_yaml($format_file);
}

my $messages = $format->{'Message ID'} // {};
my $gr_ids = $format->{'GR ID'} // {};
my $byte_order = $format->{'byte_order'} // $format->{'byte order'} // 'little_endian';

# Byte order: 1 = little endian (Intel), 0 = big endian (Motorola)
my $bo_value = ($byte_order eq 'little_endian') ? 1 : 0;

# Data type to bit length
my %type_bits = (
    'b'    => 1,
    'u4'   => 4,
    's8'   => 8,
    'u8'   => 8,
    'i8'   => 8,
    'u16'  => 16,
    'i16'  => 16,
    's16'  => 16,
    'u32'  => 32,
    'i32'  => 32,
    's32'  => 32,
    'u64'  => 64,
    'i64'  => 64,
    's'    => 64,
    'double' => 64,
);

sub is_signed {
    my $type = shift;
    return ($type =~ /^[is]/) ? '-' : '+';
}

sub parse_map_equation {
    my $eq = shift // "1x";
    $eq =~ s/^["']|["']$//g;
    
    my ($scale, $offset) = (1, 0);
    
    if ($eq =~ /^(\d*\.?\d*)x$/) {
        $scale = $1 || 1;
    } elsif ($eq =~ /^(\d*\.?\d*)x\+(\d+\.?\d*)$/) {
        $scale = $1 || 1;
        $offset = $2;
    } elsif ($eq =~ /^(\d*\.?\d*)x-(\d+\.?\d*)$/) {
        $scale = $1 || 1;
        $offset = -$2;
    } elsif ($eq =~ /^x\/(\d+\.?\d*)$/) {
        $scale = 1 / $1;
    } elsif ($eq =~ /^(\d+\.?\d*)x\/(\d+\.?\d*)$/) {
        $scale = $1 / $2;
    } elsif ($eq =~ /^abs\(x\)\/(\d+\.?\d*)$/) {
        $scale = 1 / $1;
    }
    
    return ($scale, $offset);
}

sub normalize_name {
    my $name = shift;
    $name =~ s/\s+/_/g;
    $name =~ s/-/_/g;
    $name =~ s/[^a-zA-Z0-9_]//g;
    return $name;
}

sub get_gr_id_hex {
    my $name = shift;
    my $hex_str = $gr_ids->{$name};
    return undef unless defined $hex_str;
    $hex_str =~ s/^["']|["']$//g;
    $hex_str =~ s/^0x//i;
    return hex($hex_str);
}

sub get_message_def {
    my $msg_name = shift;
    return $messages->{$msg_name};
}

sub compute_can_id {
    my ($sender_id, $msg_id, $target_id) = @_;
    return ($sender_id << 16) | ($msg_id << 8) | $target_id;
}

# Collect nodes
my %nodes;
for my $name (keys %$gr_ids) {
    $nodes{normalize_name($name)} = 1;
}

# Build DBC messages
my @dbc_messages;
my %seen_ids;

for my $route (@$routing) {
    my $sender = $route->{sender};
    my $msg_name = $route->{msg};
    my $target = $route->{target};
    my $can_id_override = $route->{can_id_override};
    
    my $msg_def = get_message_def($msg_name);
    unless ($msg_def) {
        warn "Warning: Message '$msg_name' not found, skipping\n";
        next;
    }
    
    my $can_id;
    if (defined $can_id_override) {
        $can_id_override =~ s/^["']|["']$//g;
        if ($can_id_override =~ /^0x/i) {
            $can_id = hex($can_id_override);
        } else {
            $can_id = int($can_id_override);
        }
    } else {
        my $sender_id = get_gr_id_hex($sender);
        my $target_id = get_gr_id_hex($target);
        
        my $msg_id_str = $msg_def->{'MSG ID'} // '0x000';
        $msg_id_str =~ s/^["']|["']$//g;
        $msg_id_str =~ s/^0x//i;
        my $msg_id = hex($msg_id_str);
        
        unless (defined $sender_id && defined $target_id) {
            warn "Warning: Cannot resolve IDs for $sender -> $target, skipping\n";
            next;
        }
        
        $can_id = compute_can_id($sender_id, $msg_id, $target_id);
    }
    
    # Skip duplicates
    my $id_key = sprintf("%X_%s_%s", $can_id, $sender, $target);
    next if $seen_ids{$id_key}++;
    
    my $length = $msg_def->{'MSG LENGTH'} // 8;
    my $dbc_msg_name = normalize_name($sender) . "_" . normalize_name($msg_name) . "_to_" . normalize_name($target);
    
    # Collect signals
    my @signals;
    for my $field_name (keys %$msg_def) {
        next if $field_name eq 'MSG ID';
        next if $field_name eq 'MSG LENGTH';
        next if $field_name =~ /^Reserved/i;
        
        my $field = $msg_def->{$field_name};
        next unless ref($field) eq 'HASH';
        
        # Get bit start from _bit_start field or comment key
        my $bit_start = 0;
        if (defined $field->{'_bit_start'}) {
            $bit_start = $field->{'_bit_start'};
        } else {
            for my $k (keys %$field) {
                if ($k =~ /bit start/) {
                    my $v = $field->{$k};
                    ($bit_start) = $v =~ /(\d+)/ if defined $v;
                    last;
                }
            }
        }
        
        my $data_type = $field->{'data type'} // 'u8';
        my $bit_length;
        if ($data_type =~ /^(\w+)\[(\d+)\]$/) {
            my ($base, $count) = ($1, $2);
            $bit_length = ($type_bits{$base} // 8) * $count;
        } else {
            $bit_length = $type_bits{$data_type} // 8;
        }
        
        my $signed = is_signed($data_type);
        my $map_eq = $field->{'map equation'} // "1x";
        my ($scale, $offset) = parse_map_equation($map_eq);
        
        my $min = $field->{'scaled min'} // 0;
        my $max = $field->{'scaled max'} // 0;
        $min = 0 if !defined($min) || $min eq '-' || $min eq '';
        $max = 0 if !defined($max) || $max eq '-' || $max eq '';
        
        my $unit = $field->{'units'} // '';
        $unit =~ s/^["']|["']$//g;
        
        push @signals, {
            name => normalize_name($field_name),
            start_bit => $bit_start,
            length => $bit_length,
            byte_order => $bo_value,
            signed => $signed,
            scale => $scale,
            offset => $offset,
            min => $min,
            max => $max,
            unit => $unit,
            receiver => normalize_name($target),
        };
    }
    
    @signals = sort { $a->{start_bit} <=> $b->{start_bit} } @signals;
    
    push @dbc_messages, {
        name => $dbc_msg_name,
        can_id => $can_id,
        length => $length,
        transmitter => normalize_name($sender),
        signals => \@signals,
    };
}

# Write DBC
print "Writing $output_file...\n";
open my $fh, '>', $output_file or die "Cannot open $output_file: $!";

print $fh "VERSION \"\"\n\n";
print $fh "NS_ :\n\n";
print $fh "BS_:\n\n";

print $fh "BU_:";
for my $node (sort keys %nodes) {
    print $fh " $node";
}
print $fh "\n\n";

for my $msg (sort { $a->{can_id} <=> $b->{can_id} } @dbc_messages) {
    my $can_id = $msg->{can_id};
    # Extended ID flag if > 11 bits
    if ($can_id > 0x7FF) {
        $can_id = $can_id | 0x80000000;
    }
    
    printf $fh "BO_ %u %s: %d %s\n",
        $can_id,
        $msg->{name},
        $msg->{length},
        $msg->{transmitter};
    
    for my $sig (@{$msg->{signals}}) {
        printf $fh " SG_ %s : %d|%d@%d%s (%.10g,%.10g) [%s|%s] \"%s\" %s\n",
            $sig->{name},
            $sig->{start_bit},
            $sig->{length},
            $sig->{byte_order},
            $sig->{signed},
            $sig->{scale},
            $sig->{offset},
            $sig->{min},
            $sig->{max},
            $sig->{unit},
            $sig->{receiver};
    }
    print $fh "\n";
}

close $fh;
print "Done! Generated " . scalar(@dbc_messages) . " messages.\n";