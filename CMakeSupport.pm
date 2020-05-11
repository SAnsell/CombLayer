## Simple stuff for CMake


sub getAllMain()
 ##
 ## Find the main files
 ##
{
  my $self=shift;

  my $cppMain=`ls ./Main/*.cxx 2> /dev/null`;
  my @files=split /\s+/,$cppMain;
  my @Out;
  foreach my $item (@files)
  {
      if ($item=~/Main\/(\S+)\.cxx/)
      {
	  push(@Out,$1);
      }
  }
  return @Out;
}

  
 
1;
