package CMakeList;

use IO::Handle;
use strict;

sub new
{
  my $type=shift;
  my $class = ref($type) || $type;
  
  my $self={
    
    bcomp => "clang",
    ccomp => "clang",
    cflag => "-fPIC -Wconversion -W -Wall -Wextra -Wno-comment -fexceptions -std=c++11",
    boostLib => "-L/opt/local/lib -lboost_regex",
    
    masterProg => undef,
    depLists => undef,	

    incLib => undef,           ## Array of all our includes used
    srcDir => undef,           ## Array of all our cxx directories
    incDir => undef            ## Array of all our includes used
  };
  
  bless $self,$class;
  $self->{masterProg}=[ ];
  $self->{incDir}=[ ];
  $self->{srcDir}={ };
  
  $self->{incLib}=[ ];
  $self->{depLists}={};
  return $self;
}

sub addMasterProgs
  ##
  ## Add an array of programs
  ## Names : Controlflags
  ##
{
  my $self=shift;
  my $Ar=shift;
  
  push(@{$self->{masterProg}},@{$Ar});
  return;
}

sub hasCPPFiles
  ##
    ## Simple test to find if we have C++
  ## files in the directory 
{
  
  my $Dname=shift;  ## Directory to search
  
  my $cppAll=`ls ./$Dname/*.cxx 2> /dev/null`;
  return ($cppAll) ? 1 : 0;
}

sub findSubSrcDir
 ##
 ## Adds sub-directories of libraries [include files]
 ## Assumes that all *Inc are being added   
 ##
{
  my $self=shift;
  my $tName=shift;  ## Top directory name

  my $topDirName=($tName) ? $tName : "./";
  
  my $dirAll=`ls -d ./$topDirName/*/ 2> /dev/null`;
  my @DPts=split /\s+/,$dirAll;
  my @subDir;
  foreach my $dname (@DPts)
    {
      if ($dname=~/$topDirName\/(.*)\// &&
	  $dname!~/\/Main\//)
        {
	  push(@subDir,$1) if (hasCPPFiles($dname));
	}
    }

  $self->addSrcDir($tName,\@subDir) if (@subDir);
  return;
}

sub findSubIncDir
 ##
 ## Adds sub-directories of libraries [include files]
 ## Assumes that all *Inc are being added   
 ##
{
  my $self=shift;
  my $topDirName=shift;  ## Top directory name

  my $dirAll=`ls -d ./$topDirName/*/ 2> /dev/null`;
  my @DPts=split /\s+/,$dirAll;
  my @subDir;
  my @blanklibFlags=("","");
  foreach my $dname (@DPts)
    {
      if ($dname=~/$topDirName\/(.*Inc)\//)
        {
	  push(@subDir,$1);
	}
    }

  $self->addIncDir($topDirName,\@subDir) if (@subDir);
  return;
}

sub addIncDir
  ## 
  ## Set the include directory
  ## input is Headname (Model etc) and 
  ## 
  ##
{
  my $self=shift;
  my $DHead = shift;
  my $Ar=shift;
  $DHead.="/" if (length($DHead)>1);
      
  foreach my $name (@{$Ar})
    {
      push(@{$self->{incDir}},$DHead.$name);
    }
  return;
}

sub addSrcDir
  ## 
  ## Set the include directory
  ## input is Headname (Model etc) and 
  ## 
  ##
{
  my $self=shift;
  my $DHead = shift;
  my $Ar=shift;
  $DHead.="/" if (length($DHead)>1);
      
  foreach my $name (@{$Ar})
    {
      $self->{srcDir}{$name}=$DHead.$name;
    }
  return;
}

sub addDepUnit
  ## Ugly function to set dep list.
  ## Does it actually work??
{
  my $self=shift;
  my $name = shift;   ## Project name
  my $Ar=shift;       ## List of dependency
  
  ## Get master prog:
  my $index;
  for($index=0;$index<scalar(@{$self->{masterProg}});$index++)
    {
      last if ($self->{masterProg}[$index] eq $name);
    }
  if ( $index==scalar(@{$self->{masterProg}}) )
    {
      print STDERR "Ignoring build item :: ",$name,"\n";
      return;
    }
  $self->{depLists}{$name}= [ @{$Ar} ];
  return;
}

sub writeHeader
  ##
  ## Header
  ## Names : Controlflags
  ##
{
  my $self=shift;
  my $DX=shift;   ## FILEGLOB

  print $DX "cmake_minimum_required(VERSION 3.2)\n\n";

  
  print $DX "set(CMAKE_CXX_COMPILER ",$self->{ccomp},")\n";
  print $DX "add_definitions(",$self->{cflag},")\n";

  print $DX "set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ./lib)\n";
  
  print $DX "\n";

  return;
}
 
sub writeIncludes
  # Write out the include list
{
  my $self=shift;
  my $DX=shift;

  foreach my $item (@{$self->{incDir}})
    {
      print $DX "include_directories(\"\${PROJECT_SOURCE_DIR}/";
      print $DX $item,"\")\n";
    }
  print $DX "## END INCLUDES \n\n";
  return;
}


sub writeGLOB
  # Write out the include list
{
  my $self=shift;
  my $DX=shift;

  print $DX "## GLOBS \n";
  foreach my $item (keys (%{$self->{srcDir}}))
  {
      my $val=$self->{srcDir}{$item};
      
    print $DX "file(GLOB ",$item," \"\${PROJECT_SOURCE_DIR}\/",
    $val."\/\*.cxx\")\n";
    print $DX "add_library(lib".$item." SHARED \$\{".$item."\})\n";

  }
  print $DX "## END GLOBS \n\n";

  return;
}

sub writeExcutables
 #
 # Write out the executable list
 #   
{
  my $self=shift;
  my $DX=shift;
  
  print $DX "## EXECUTABLES \n";
  foreach my $item (@{$self->{masterProg}})
    {
      print $DX "add_executable(",$item," \${PROJECT_SOURCE_DIR}/Main/",
      $item,")\n";
      foreach my $dItem (@{$self->{depLists}{$item}})
        {
	  print $DX "target_link_libraries(",$item,"  lib",$dItem,")\n";
        }
      print $DX "target_link_libraries(",$item," boost_regex)\n";
      print $DX "target_link_libraries(",$item," stdc++)\n ";
      print $DX "target_link_libraries(",$item," gsl)\n";
    }
  
  
  print $DX "## END EXECUTABLE \n\n";
  return;
}

sub writeTail
  # Write stuff at end of CMakeLists file
{
  my $self=shift;
  my $DX=shift;

  my $pdir=`pwd`;
  $pdir=$1 if ($pdir=~/.*\/(.*)/);
  
  my $tarString;
  
  print $DX "add_custom_target(tar ",
    " COMMAND tar zcvf \${PROJECT_SOURCE_DIR}/",$pdir.".tgz \n";

  foreach my $item (keys (%{$self->{srcDir}}))
    {
      my $val=$self->{srcDir}{$item};
      print $DX "     \${PROJECT_SOURCE_DIR}/",$val,"/*.cxx \n";
    }
  foreach my $item (@{$self->{incDir}})
    {
      print $DX "     \${PROJECT_SOURCE_DIR}/",$item,"/*.h \n";
    }

  print $DX "     \${PROJECT_SOURCE_DIR}/CMake.pl  \n";
  print $DX "     \${PROJECT_SOURCE_DIR}/CMakeList.pm \n";
  print $DX " )\n";
  print $DX "\n";
  ## TAGS:
  print $DX "add_custom_target(tags ",
    " COMMAND etags  \n";


  foreach my $item (keys (%{$self->{srcDir}}))
    {
      my $val=$self->{srcDir}{$item};
      print $DX "     \${PROJECT_SOURCE_DIR}/",$val,"/*.cxx \n";
    }
  foreach my $item (@{$self->{incDir}})
    {
      print $DX "     \${PROJECT_SOURCE_DIR}/",$item,"/*.h \n";
    }

  print $DX " )\n";
  print $DX "\n";
  return;
}

sub writeCMake
 ##
 ## Write everything
{
  my $self=shift;

  my $DX;
  open($DX,">","CMakeLists.txt");

  $self->writeHeader($DX);
  $self->writeIncludes($DX);
  $self->writeGLOB($DX);
  $self->writeExcutables($DX);  
  $self->writeTail($DX);
}
  
1;
 
