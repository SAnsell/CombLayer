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
    boostLib => "-L/opt/local/lib ",
    
    masterProg => undef,
    definitions => undef,
    depLists => undef,
    optimise => "",
    gtk => 0,
    debug => "",
    noregex => 0,
    
    incLib => undef,           ## Array of all our includes used
    srcDir => undef,           ## Array of all our cxx directories
    incDir => undef            ## Array of all our includes used
  };
  
  bless $self,$class;
  $self->{masterProg}=[ ];
  $self->{definitions}=[ ];
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

sub setParameters
  ##
  ## Given a set of flags
  ## Determine which parameters we should set
  ##  
{
  my $self=shift;
  my $Ar=shift;

  my $nogsl=1;
  foreach my $FlagArray (@{$Ar})
    {
      my @Flags =split " ",$FlagArray;
      foreach my $Ostr (@Flags)
        {
	  $self->{optimise}.=" -O2 " if ($Ostr eq "-O");
	  push(@{$self->{definitions}},"NO_REGEX") if ($Ostr eq "-NR");
	  $self->{noregex}=1 if ($Ostr eq "-NR");
	  $self->{optimise}.=" -pg " if ($Ostr eq "-p"); ## Gprof
	  $self->{gcov}=1 if ($Ostr eq "-C");
	  $self->{gtk}=1 if ($Ostr eq "-gtk");
	  $self->{debug}="" if ($Ostr eq "-g");
	  $self->{bcomp}=$1 if ($Ostr=~/-gcc=(.*)/);
	  $self->{ccomp}=$1 if ($Ostr=~/-g\+\+=(.*)/);
	  $self->{cxx11}="" if ($Ostr=~/-std/);
	  
	}
    }
#  $self->{gsl}*=$nogsl;

  print STDERR "INIT Opt=",$self->{optimise},"\n";
  print STDERR "INIT Deb=",$self->{debug},"\n";
#  print STDERR "GSL == ",$self->{gsl},"\n";
#  print STDERR "ROOT == ",$self->{root},"\n";
#  print STDERR "REGEX == ",$self->{regex},"\n";
#  print STDERR "G++ == ",$self->{ccomp},$self->{cxx11},"\n";
#  print STDERR "GCC == ",$self->{bcomp},"\n";
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

  print $DX "cmake_minimum_required(VERSION 2.8)\n\n";

  
  print $DX "set(CMAKE_CXX_COMPILER ",$self->{ccomp},")\n";
  print $DX "set(CMAKE_CXX_FLAGS \"",$self->{cflag}.$self->{optimise}.$self->{debug},"\")\n";
  print $DX "set(CMAKE_CXX_RELEASE_FLAGS \"",$self->{cflag}." -O2 ".$self->{debug},"\")\n";
  
  print $DX "set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ./lib)\n";

  if ($self->{gtk})
  {
      print $DX "find_package(PkgConfig REQUIRED)\n";
      print $DX "pkg_check_modules(GTK3 REQUIRED gtk+-3.0)\n";
  }
  
  foreach my $item (@{$self->{definitions}})
  {
    print $DX "add_definitions(-D",$item,")\n";
  }
  print $DX "\n";
  print $DX "if(\"\${CMAKE_CXX_COMPILER_ID}\" STREQUAL \"Clang\")\n";
  print $DX "set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS \"";
  print $DX "\${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} ";
  print $DX "-undefined dynamic_lookup\")\n";
  print $DX "endif()\n";

  print $DX "if(\"\${CMAKE_CXX_COMPILER_ID}\" STREQUAL \"GNU\")\n";
  print $DX "set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS \"";
  print $DX "\${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} ";
  print $DX "-Wl,--start-group\")\n";
  print $DX "endif()\n";
  
  return;
}
 
sub writeIncludes
  # Write out the include list
{
  my $self=shift;
  my $DX=shift;

  print $DX "include_directories(\${GTK3_INCLUDE_DIRS})\n" if ($self->{gtk});
      
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
      if (!$self->{noregex})
        {
#          print $DX "target_link_libraries(",$item," boost_regex)\n";
          print $DX "target_link_libraries(",$item," boost_filesystem)\n";
	}
      print $DX "target_link_libraries(",$item," stdc++)\n";
      print $DX "target_link_libraries(",$item," gsl)\n";
      print $DX "target_link_libraries(",$item," gslcblas)\n";
      print $DX "target_link_libraries(",$item," m)\n";
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

  print $DX "set(ALLCXX \n";
  foreach my $item (keys (%{$self->{srcDir}}))
    {
      my $val=$self->{srcDir}{$item};
      print $DX "     \./",$val,"/*.cxx \n";
    }
  print $DX "     \./Main/*.cxx )\n";

  print $DX "set(ALLHXX \n";
  foreach my $item (@{$self->{incDir}})
    {
      print $DX "     \./",$item,"/*.h \n";
    }
  print $DX "      )\n";

  print $DX "set(ASRC \${ALLHXX} \${ALLCXX} )\n";
  
## DOXYGEN
  
  print $DX "add_custom_target(doxygen ".
      " COMMAND ".
      " { cat Doxyfile \\; echo \"INPUT=\" \${ASRC} \"\" \\;} | doxygen - )\n";

## SLOC
  
  print $DX "add_custom_target(sloc ".
      " COMMAND sloccount \${ASRC} )\n";

## WORDS

  my $wordString;  
  print $DX "add_custom_target(words ",
    " COMMAND grep -v -e \'^[[:space:][:cntrl:]]*\$\$\' \n";
  foreach my $item (keys (%{$self->{srcDir}}))
    {
      ## Care here because we want local tar file
      my $val=$self->{srcDir}{$item};
      print $DX "     \./",$val,"/*.cxx \n";
    }
  foreach my $item (@{$self->{incDir}})
    {
      print $DX "     \./",$item,"/*.h \n";
    }
  print $DX "     \./Main/*.cxx \n";
  print $DX "     \./CMake.pl  \n";
  print $DX "     .//CMakeList.pm \n";
  print $DX " | wc )\n";
  print $DX "\n";

    
  my $tarString;  
  print $DX "add_custom_target(tar ",
    " COMMAND tar zcvf \${PROJECT_SOURCE_DIR}/",$pdir.".tgz \n";

  foreach my $item (keys (%{$self->{srcDir}}))
    {
      ## Care here because we want local tar file
      my $val=$self->{srcDir}{$item};
      print $DX "     \./",$val,"/*.cxx \n";
    }
  foreach my $item (@{$self->{incDir}})
    {
      print $DX "     \./",$item,"/*.h \n";
    }

  print $DX "     \./Main/*.cxx \n";
  print $DX "     \./CMake.pl  \n";
  print $DX "     .//CMakeList.pm \n";
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
 
