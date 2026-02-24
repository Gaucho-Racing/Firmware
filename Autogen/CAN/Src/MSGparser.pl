#!/usr/bin/perl
use strict;
use warnings;
use File::Basename;
use File::Path qw(make_path);

my $yaml_file   = $ARGV[0] // 'format.CANdo';
my $output_file = $ARGV[1] // 'can_msg_ids.h';
my $dir         = dirname($output_file);
make_path($dir) if $dir && $dir ne '.' && !-d $dir;

# Safety check: Verify the YAML source exists
if ( !-e $yaml_file ) {
	die "CANfigurator Error: Input YAML not found at '$yaml_file'.\n";
}

open my $fh, '<', $yaml_file or die "CANfigurator Error: Can't open $yaml_file: $!";

my @msg_ids;

# --- Parsing Logic ---
while ( my $line = <$fh> ) {

	# Stop parsing if we hit other sections
	last if $line =~ /^(Custom CAN ID|GR ID):/;

	# Identify a Message Name (indented by 2 spaces)
	if ( $line =~ /^\s{2}(\w[\w\s\d\.]+):\s*$/ ) {
		my $msg_name = $1;
		my $found_id = 0;

		# Inner loop: Scan this message block for its MSG ID
		while ( my $next_line = <$fh> ) {

			# Catch the MSG ID field (indented by 4 spaces)
			if ( $next_line =~ /^\s{4}MSG ID:\s*(0x[0-9A-Fa-f]+)/ ) {
				my $id        = $1;
				my $enum_name = "MSG_" . uc($msg_name);
				$enum_name =~ s/[^A-Z0-9]+/_/g;    # Clean up spaces/dots for C enum compatibility
				$enum_name =~ s/_+/_/g;
				$enum_name =~ s/^_|_$//g;

				push @msg_ids, { name => $enum_name, id => $id };
				$found_id = 1;
				last;
			}

			# Safety break: If we hit a new message block or section before finding an ID,
			# return to outer loop to process the new line.
			if ( $next_line =~ /^\s{2}\w/ || $next_line =~ /^\w/ ) {

				# Reposition the file pointer so the outer loop can read this header
				seek( $fh, -length($next_line), 1 );
				last;
			}

			# All other fields (bit_start, data type, map equation, etc.) are
			# naturally ignored as the loop continues until it hits MSG ID.
		}
	}
}
close($fh);

# --- Writing Logic ---
open my $out, '>', $output_file or die "CANfigurator Error: Can't create $output_file: $!";

print {$out} "// Auto-generated CAN Message IDs\n";
print {$out} "#ifndef CAN_MSG_IDS_H\n";
print {$out} "#define CAN_MSG_IDS_H\n\n";
print {$out} "typedef enum {\n";

foreach my $msg (@msg_ids) {
	printf $out "    %-40s = %s,\n", $msg->{name}, $msg->{id};
}

print {$out} "} can_msg_id_t;\n\n";
print {$out} "#endif // CAN_MSG_IDS_H\n";

close($out);

print "CANfigurator: Generated $output_file with " . scalar(@msg_ids) . " message IDs\n";

# #!/usr/bin/perl
# use strict;
# use warnings;
# use File::Basename;
# use File::Path qw(make_path);
# my $yaml_file   = $ARGV[0] // 'format.CANdo';
# my $output_file = $ARGV[1] // 'can_msg_ids.h';
# my $dir         = dirname($output_file);
# make_path($dir) if $dir && $dir ne '.' && !-d $dir;

# # Safety check: Verify the YAML source exists before parsing
# if ( !-e $yaml_file ) {
# 	die "CANfigurator Error: Input YAML not found at '$yaml_file'.\n" . "Check your CMake source paths or provide the file as the first argument.\n";
# }

# # Open the YAML for reading
# open( my $fh, '<', $yaml_file ) or die "CANfigurator Error: Can't open $yaml_file: $!";

# my @msg_ids;

# # --- Parsing Logic ---
# while ( my $line = <$fh> ) {

# 	# Stop parsing if we hit other sections
# 	last if $line =~ /^(Custom CAN ID|GR ID):/;

# 	if ( $line =~ /^\s{2}(\w[\w\s]+\w):\s*$/ ) {
# 		my $msg_name = $1;

# 		while ( my $next_line = <$fh> ) {
# 			last if $next_line =~ /^(Custom CAN ID|GR ID):/;

# 			if ( $next_line =~ /^\s{4}MSG ID:\s*(0x[0-9A-Fa-f]+)/ ) {
# 				my $id        = $1;
# 				my $enum_name = "MSG_" . uc($msg_name);
# 				$enum_name =~ s/\s+/_/g;

# 				push @msg_ids, { name => $enum_name, id => $id };
# 				last;
# 			}
# 			last if $next_line =~ /^\s{2}\w[\w\s]+:/;
# 		}
# 	}
# }
# close($fh);

# # --- Writing Logic ---
# # Open the header file for writing using the dynamic output path
# open( my $out, '>', $output_file ) or die "CANfigurator Error: Can't create $output_file: $!";

# print $out "// Auto-generated CAN Message IDs\n";
# print $out "#ifndef CAN_MSG_IDS_H\n";
# print $out "#define CAN_MSG_IDS_H\n\n";
# print $out "typedef enum {\n";

# foreach my $msg (@msg_ids) {
# 	print $out "    $msg->{name} = $msg->{id},\n";
# }

# print $out "} can_msg_id_t;\n\n";
# print $out "#endif // CAN_MSG_IDS_H\n";

# close($out);

# print "CANfigurator: Generated $output_file with " . scalar(@msg_ids) . " message IDs\n";
