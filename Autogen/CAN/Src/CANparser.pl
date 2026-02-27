#!/usr/bin/env perl
use strict;
use warnings;
use YAML::XS qw(LoadFile);
use File::Basename;

# --- Configuration ---
my $yaml_path   = $ARGV[0] // 'format.CANdo';
my $output_path = $ARGV[1] // 'Custom_CAN_ID.h';

# 1. Load the data
if ( !-e $yaml_path ) {
	die "Error: $yaml_path not found.\n";
}

my $yaml     = LoadFile($yaml_path);
my $can_defs = $yaml->{'Custom CAN ID'};

# 2. Open the file in WRITE mode
# Fixed: "InputOutput::RequireBriefOpen" - Keeping open close to prints
# Fixed: "Variables::ProhibitPunctuationVars" - Using concatenation for $!
open my $fh, '>', $output_path or die 'Error: Cannot open ' . $output_path . ' for writing: ' . $!;

# Fixed: "InputOutput::RequireCheckedSyscalls" and "Variables::ProhibitPunctuationVars"
print {$fh} "// Auto-generated Custom CAN ID header\n" or die 'Print failed: ' . $!;
print {$fh} "#ifndef CUSTOM_CAN_ID_H\n"                or die 'Print failed: ' . $!;
print {$fh} "#define CUSTOM_CAN_ID_H\n\n"              or die 'Print failed: ' . $!;
print {$fh} "typedef enum {\n"                         or die 'Print failed: ' . $!;

for my $msg_name ( sort keys %{ $can_defs } ) {
    my $entry = $can_defs->{$msg_name};

	if ( ref $entry ne 'HASH' ) {
		next;
	}

	my $can_id = $entry->{'CAN ID'};
	if ( !defined $can_id ) {
		next;
	}

    my $enum_name = uc $msg_name;
    $enum_name =~ s/[[:^upper:][:digit:]]/_/g;
    $enum_name =~ s/_+/_/g;
    $enum_name =~ s/^_|_$//g;

    my $val = $can_id;
    if ( $val =~ /^[[:xdigit:]]+$/ && $val !~ /^[[:digit:]]+$/ ) {
        $val = '0x' . lc $val;
    }
    elsif ( $val =~ /^([[:xdigit:]]+)d$/ ) {
        $val = '0x' . lc $1;
    }

    print {$fh} "    ${enum_name}_CAN_ID = $val,\n" or die 'Print failed: ' . $!;
}

print {$fh} "} Custom_CAN_ID_t;\n\n"      or die 'Print failed: ' . $!;
print {$fh} "#endif // CUSTOM_CAN_ID_H\n" or die 'Print failed: ' . $!;

# Fixed: "InputOutput::RequireCheckedClose"
close $fh or die 'Error: Failed to close ' . $output_path . ': ' . $!;

print "Successfully updated $output_path\n" or die 'Print failed: ' . $!;

exit 0;

# #!/usr/bin/env perl
# use strict;
# use warnings;
# use fatal qw(open close); # Addresses Linter Error #1 & #5 (Unchecked returns)
# use autodie qw(open close print);
# use YAML::XS 'LoadFile';
# use File::Basename;

# # --- Configuration ---
# my $yaml_path   = $ARGV[0] // 'format.CANdo';
# my $output_path = $ARGV[1] // 'Custom_CAN_ID.h';

# # 1. Load the data
# if ( !-e $yaml_path ) {
#     die "Error: $yaml_path not found.\n";
# }

# my $yaml     = LoadFile($yaml_path);
# my $can_defs = $yaml->{'Custom CAN ID'};

# # 2. Open the file in WRITE mode
# # autodie handles the "or die" automatically here
# open my $fh, '>', $output_path;

# print $fh "// Auto-generated Custom CAN ID header\n";
# print $fh "#ifndef CUSTOM_CAN_ID_H\n";
# print $fh "#define CUSTOM_CAN_ID_H\n\n";
# print $fh "typedef enum {\n";

# # Sort to keep the header organized
# for my $msg_name ( sort keys %$can_defs ) {
#     my $entry = $can_defs->{$msg_name};
#     next unless ref($entry) eq 'HASH';

#     my $can_id = $entry->{'CAN ID'};
#     next unless defined $can_id;

#     # Clean the name: spaces to underscores, uppercase
#     my $enum_name = uc($msg_name);
#     $enum_name =~ s/[^A-Z0-9]/_/g;
#     $enum_name =~ s/_+/_/g;
#     $enum_name =~ s/^_|_$//g;

#     # Format the ID
#     my $val = $can_id;
#     if ( $val =~ /^[0-9A-Fa-f]+$/ && $val !~ /^\d+$/ ) {
#         $val = "0x" . lc($val);
#     }
#     elsif ( $val =~ /^([0-9A-Fa-f]+)d$/ ) {
#         $val = "0x" . lc($1);
#     }

#     print $fh "    ${enum_name}_CAN_ID = $val,\n";
# }

# print $fh "} Custom_CAN_ID_t;\n\n";
# print $fh "#endif // CUSTOM_CAN_ID_H\n";

# close $fh;

# print "Successfully updated $output_path\n";

# # Addresses Linter Error #3 (Explicit return for main script flow)
# exit 0;

# #!/usr/bin/env perl
# use strict;
# use warnings;
# use YAML::XS 'LoadFile';
# use File::Basename;

# # --- Configuration ---
# my $yaml_path   = $ARGV[0] // 'format.CANdo';
# my $output_path = $ARGV[1] // 'Custom_CAN_ID.h';

# # 1. Load the data
# if ( !-e $yaml_path ) { die "Error: $yaml_path not found.\n"; }
# my $yaml     = LoadFile($yaml_path);
# my $can_defs = $yaml->{'Custom CAN ID'};

# # 2. Open the file in WRITE mode ('>')
# # This updates the existing file by overwriting it with fresh data.
# open my $fh, '>', $output_path or die "Error: Cannot open $output_path: $!";

# print $fh "// Auto-generated Custom CAN ID header\n";
# print $fh "#ifndef CUSTOM_CAN_ID_H\n";
# print $fh "#define CUSTOM_CAN_ID_H\n\n";

# print $fh "typedef enum {\n";

# # Sort to keep the header organized
# for my $msg_name ( sort keys %$can_defs ) {
# 	my $entry = $can_defs->{$msg_name};
# 	next unless ref($entry) eq 'HASH';

# 	my $can_id = $entry->{'CAN ID'};
# 	next unless defined $can_id;

# 	# Clean the name: spaces to underscores, uppercase
# 	my $enum_name = uc($msg_name);
# 	$enum_name =~ s/[^A-Z0-9]/_/g;
# 	$enum_name =~ s/_+/_/g;
# 	$enum_name =~ s/^_|_$//g;

# 	# Format the ID (Decimal stays decimal, Hex gets 0x)
# 	my $val = $can_id;
# 	if ( $val =~ /^[0-9A-Fa-f]+$/ && $val !~ /^\d+$/ ) {
# 		$val = "0x" . lc($val);    # Handles 'A16'
# 	}
# 	elsif ( $val =~ /^([0-9A-Fa-f]+)d$/ ) {
# 		$val = "0x" . lc($1);      # Handles '10d'
# 	}

# 	print $fh "    ${enum_name}_CAN_ID = $val,\n";
# }

# print $fh "} Custom_CAN_ID_t;\n\n";
# print $fh "#endif // CUSTOM_CAN_ID_H\n";
# close $fh;

# print "Successfully updated $output_path\n";
