#!/usr/bin/env perl
## no critic (InputOutput::RequireBriefOpen, InputOutput::RequireCheckedSyscalls, CodeLayout::RequireTidyCode)
use strict;
use warnings;
use File::Basename;
use File::Path qw(make_path);
use English    qw(-no_match_vars);

my $yaml_file   = $ARGV[0] // 'format.CANdo';
my $output_file = $ARGV[1] // 'can_msg_ids.h';
my $dir         = dirname($output_file);

if ( $dir && $dir ne q{.} && !-d $dir ) {
	make_path($dir);
}

if ( !-e $yaml_file ) {
	die "CANfigurator Error: Input YAML not found at '$yaml_file'.\n";
}

open my $fh, '<', $yaml_file or die "CANfigurator Error: Can't open $yaml_file: $OS_ERROR\n";

my @msg_ids;

# --- Parsing Logic ---
while ( my $line = <$fh> ) {

	if ( $line =~ /^ (Custom[ ]CAN[ ]ID | GR[ ]ID) : /smx ) {
		last;
	}

	if ( $line =~ /^ \s{2} ( \w [\w\s\d.]+ ) : \s* $/smx ) {
		my $msg_name = $1;

		while ( my $next_line = <$fh> ) {

			# Use POSIX xdigit class instead of [0-9A-Fa-f]
			if ( $next_line =~ /^ \s{4} MSG[ ]ID : \s* (0x[[:xdigit:]]+) /smx ) {
				my $id        = $1;
				my $enum_name = 'MSG_' . uc $msg_name;

				# Use \W (non-word character) instead of [^A-Z0-9]
				$enum_name =~ s/\W+/_/gsmx;
				$enum_name =~ s/_+/_/gsmx;
				$enum_name =~ s/^_|_$//gsmx;

				push @msg_ids, { name => $enum_name, id => $id };
				last;
			}

			if ( $next_line =~ /^ \s{2} \w /smx || $next_line =~ /^ \w /smx ) {
				seek $fh, -length $next_line, 1;
				last;
			}
		}
	}
}

close $fh or die "CANfigurator Error: Can't close $yaml_file: $OS_ERROR\n";

# --- Writing Logic ---
open my $out, '>', $output_file or die "CANfigurator Error: Can't create $output_file: $OS_ERROR\n";

print {$out} "// Auto-generated CAN Message IDs\n";
print {$out} "#ifndef CAN_MSG_IDS_H\n";
print {$out} "#define CAN_MSG_IDS_H\n\n";
print {$out} "typedef enum {\n";

foreach my $msg (@msg_ids) {
	printf {$out} "    %-40s = %s,\n", $msg->{name}, $msg->{id};
}

print {$out} "} can_msg_id_t;\n\n";
print {$out} "#endif // CAN_MSG_IDS_H\n";

close $out or die "CANfigurator Error: Can't close $output_file: $OS_ERROR\n";

print 'CANfigurator: Generated ' . $output_file . ' with ' . scalar @msg_ids . " message IDs\n";
