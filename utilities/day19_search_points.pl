#!/usr/bin/perl
use strict;
use warnings;
use Data::Dumper;

sub read_points{
    my ($fname) = @_;
    open my $fh, '<', $fname or die "Could not open $fname: $!";
    my @points;
    while(<$fh>){
	chomp;
	push @points, $_;
    }
    return \@points;
}

sub read_id_points{
    my ($fname) = @_;
    open my $fh, '<', $fname or die "Could not open $fname: $!";
    my %point_id;
    while(<$fh>){
      chomp;
      my ($id, $point_str) = split / /, $_;
      $point_id{$point_str} = $id;
    }
    return \%point_id;
}
# local $, = "\n"; print @ARGV, "\n";
# print Dumper(read_points($ARGV[0]));
# print Dumper(read_id_points($ARGV[1]));

my $ep = read_points($ARGV[0]);
my %expected_points;
for(@$ep){$expected_points{$_} = 1;}
my $current_points = read_id_points($ARGV[1]);
for(@$ep){
  unless(defined $current_points->{$_}){
    print "Expected but not found $_\n";
  }
}

for(keys %$current_points){
  unless(defined $expected_points{$_}){
    print "Found but not expecting: $_ in ", $current_points->{$_}, "\n"
  }
}
