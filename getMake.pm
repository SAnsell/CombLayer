package getMake;

use IO::Handle;
use strict;

sub new
{
  my $type=shift;
  my $class = ref($type) || $type;
  
  my $self={
    
    bcomp => "gcc",
    ccomp => "g++",
    fcomp => "gfortran",
    cflag => "-fPIC -Wconversion -W -Wall -Wextra ".
	"-Wno-comment -fexceptions",

    boostInc => "-I/opt/local/include",
    boostLib => "-L/opt/local/lib -lboost_regex",
    clangLib => "-lstdc++",

    masterProg => [ ],        ## Executables
    nocompile => [ ],         ## Not to be compiled
    libnames => [ ],          ## Lib names 
    sublibdir => [ ],         ## SubDirectories (.cxx / .f)
    coreDir => undef,         ## .cxx sub direct [core]
    libflags => [ ],          ## Libray flags
    incdir => undef,          ## Include Directories
    controlflags => [ ],      ## Executable control flags
    depList => [ ],           ## Executable dependence lists

    incCppFiles => undef,     ## Include files in each C++
    incForFiles => undef,     ## Include files in each for
    
    luaShared=> "Script",     ## Script directory
    swigShared=> "Swig",      ## Swig directory
    swigIndex=> undef,

    optimise=>"",
    debug=>"-g",
    verbose=>"",
    output=>"lib",
    shared=>0,
    fortranlib=>0,
    boost=>0,
    gtk=>0,
    gcov=>0,          ## Gcov
    gsl=>0,          ## Gnu scietific lib
    lua=>0,          ## lua
    xlib=>0,         ## X11 libs
    pglib=>0,        ## PGPlot 
    glut=>0,
    swig=>0,


    OLFE => undef,
    StdLib => undef,          ## Array of all the standard librarys includes used
    IncLib => undef           ## Array of all our includes used
  };
  
  bless $self,$class;
  $self->{incdir}=[ ];
  $self->{coreDir}={ };
  $self->{incCppFiles}={ };
  $self->{incForFiles}={ };
  $self->{StdLib}=[ ];
  $self->{IncLib}=[ ];
  $self->{swigIndex}={ };
  return $self;
}

## STATIC FUNCTIONS:

sub spcLen
  ##
  ## Create a length of space
  ## 
{
  my $index=shift;
  my $out;
  for(my $i=0;$i<$index;$i++)
    {
      $out.=' ';
    }
  return $out;
}

sub getFileExt
{
  my $base=shift;
  my $AllType=`ls $base\.* 2> /dev/null`;

  return "for" if ($AllType=~ /${base}\.for/ || $AllType=~ /${base}\.f/);
  return "cxx" if ($AllType=~ /${base}\.cxx/ || $AllType=~ /${base}\.cpp/);
  return "c" if ($AllType=~ /${base}\.c/);
  print STDERR "All == ",$AllType,"\n";
  return "";
}

sub printString
  ##
  ## Prints out the string with breaks on the spaces at 72 char
  ## or less.
  ##
{
  my $line=shift;
  my $maxLen=(@_) ? shift : 72;
  my $spcHead=(@_) ? shift : 12;
  chomp $line;
  $line=~s/\n//g;

  my $subline=substr($line,0,$maxLen);
  while(length($line)>$maxLen)
    {
      if ($subline=~/^(.*)\s+(.*)$/)
	{
	  print $1," \\\n";
	  $line=spcLen($spcHead).$2.substr($line,$maxLen);
	}
      $subline=substr($line,0,$maxLen);
    }
  print $line,"\n";
  return;
}

sub getCppFiles
  ## 
  ## Gets a list of .cxx files from a directory
  ## 
{
  my $Dname=shift;
  my $cppAll=`ls ./$Dname/*.cxx 2> /dev/null`;
  my @CppFiles =split " ",$cppAll;
  my @CppOut;
  foreach my $fn (@CppFiles)
    {
      $fn=~s/.*\///;
      push(@CppOut,$fn);
    }
  return @CppOut;
}

sub getMainCppFiles
  ## 
  ## Gets a list of .cxx files from a directory
  ## that have a main function
  ## 
{
  my $Dname=shift;
  my $cppAll=`ls ./$Dname/*.cxx 2> /dev/null`;
  my @CppFiles =split " ",$cppAll;
  my @CppOut;

  my $line;
  foreach my $fn (@CppFiles)
    {
      open(my $DTX,'<',$fn) or next;
      my $hasNoMain=1;
      while(defined($line=<$DTX>) && $hasNoMain)
        {
	  $hasNoMain=0 if ($line=~/^\s*main\s*\(/);
	}
      close($DTX);
      if (!($hasNoMain))
        {
	  $fn=~s/.*\///;
	  push(@CppOut,$fn);
	}
    }
  return @CppOut;
}


sub getCFiles
  ## 
  ## Gets a list of .c files from a directory
  ## 
{
  my $Dname=shift;
  my $cAll=`ls ./$Dname/*.c 2> /dev/null`;
  my @CFiles =split " ",$cAll;
  my @COut;
  foreach my $fn (@CFiles)
    {
      $fn=~s/.*\///;
      push(@COut,$fn);
    }
  return @COut;

}

sub getForFiles
  ## 
  ## Gets a list of .f and .for files from a directory
  ## 
{
  my $Dname=shift;
  my $forAll=`ls ./$Dname/*.f ./$Dname/*.for 2> /dev/null`;  
  my @FFiles =split " ",$forAll;
  my @FOut;
  foreach my $fn (@FFiles)
    {
      $fn=~s/.*\///;
      push(@FOut,$fn);
    }
  return @FOut;
}

sub getSwigFiles
  ## 
  ## Gets a list of .i files from a directory
  ## 
{
  my $Dname=shift;
  my $iAll=`ls ./$Dname/*.i  2> /dev/null`;  
  my @IFiles =split " ",$iAll;
  return @IFiles;
}
 
sub getHFiles
  ## 
  ## Gets a list of .h files from a directory
  ## 
{
  my $Dname=shift;
  
  my @HFiles =split " ",`ls ./$Dname/*.h`;
  return @HFiles;
}

sub incDirName
  ##
  ## Get the name 
  ## INCDIR[A-Z]
  ## 
{
  my $incType=shift;        ## Number
  return "NULL" if ($incType<0);
  return "SRCDIR" if ($incType==0);
  my $outString="INCDIR";
  if ($incType<27) 
    {
      $outString.= chr(ord('A')+$incType-1);
    }
  else
    {
      $outString.= chr(ord('a')+$incType-27);
    }
  
  return $outString;
}

sub checkExists
  ## Determine if files exist and return type 
  ## string or null 
{
  my $str=shift;
  return length(`ls $str 2> /dev/null`) ? $str : "";
}

sub checkCoreExists
  ## Determine if files exist and return type 
  ## string or null 
{
  my $self=shift;
  my $dname=shift;
  my $str=shift;
  $dname=~s/.*\///;

  my $full=$dname.'/'.$str;
  if (exists($self->{coreDir}{$dname}) && 
      length(`ls $full 2> /dev/null`))
    {
      return $full;
    }
  return "";
}

sub buildMaster
  ##
  ## Given an array , find in the item is on the 
  ## array and insert and the obvious place
  ##
{
  my $MArray=shift;
  my $NewArray=shift;
 
  my $index=0;
  my $lastFound=scalar(@{$MArray});
  foreach my $file (@{$NewArray})
    {
      my $index=0;
      my $N=$#$MArray+1;
      for($index=0;$index<$N;$index++)
	{
	  last if ($MArray->[$index] eq $file);
	}
      ## File not found
      if ($index==$N)
	{
	  splice(@{$MArray},$#$MArray+1,0,$file);
	  $lastFound++;
 	}
    }
  return;
}

sub addFlags
  ## 
  ## Process flag information
  ## 
{
  my $flag=shift;
  my $debug=(@_) ? shift : 0;
  my $retString="";
  my @items = split '\s+',$flag;
  
  foreach my $ix (@items)
    {
      print STDERR "item == ",$ix,"\n" if ($debug);
      $retString.="\$(BOOSTLIBS) " if ($ix eq "-B");
      $retString.="\$(CLANGLIBS) " if ($ix eq "-c");
      $retString.="\$(GCOVLIBS) " if ($ix eq "-C");
      $retString.="\$(XLIBS) " if ($ix eq "-X");
      $retString.="\$(PGLIBS) " if ($ix eq "-P");
      $retString.="\$(GSLLIBS) " if ($ix eq "-S");
      $retString.="\$(GTKLIBS) " if ($ix eq "-M");
      $retString.="\$(GTKLIBS) " if ($ix eq "-K");
      $retString.="\$(OPENLIBS) " if ($ix eq "-G");
      $retString.="\$(FORTLIBS) " if ($ix eq "-f");
      $retString.="\$(LUALIBS) " if ($ix eq "-u");
      $retString.="\$(LUALIBS) " if ($ix eq "-w");
    }
  return $retString;
}

## NON STATIC FUNCTIONS

sub macModification
  # Modify everything to run on a MAC
{
}

sub setCompiler
{
  my $self=shift;
  my $addstring=shift;
  
  $self->{bcomp}="gcc-".$addstring;
  $self->{ccomp}="g++-".$addstring;
  $self->{fcomp}="gfortran-".$addstring;
  return;
}

sub createMainDep
  ## Write the MainDEP line
{
  my $self=shift;
  my $Ostr="MAINDEP=";
  for my $fn (@{$self->{masterProg}})
    {
      $Ostr.=$fn.".o ";
    }
  printString($Ostr,66,12);
  print "\n";
  return;
}
sub selectMaster
  ##
  ## Given a filename determine if it is on 
  ## the mainprog list
  ##
{
  my $self=shift;
  my $Ar=shift;
  
  
  my %MP;
  for my $item (@{$self->{masterProg}})
    {
      $MP{$item}=1;
    }

  my @Out;
  for my $fn (@{$Ar})
    {
      my $part=$fn;
      $part=$1 if ($part=~/.*\/(.*)/);
      $part=$1 if ($part=~/(.*)\./);
      push(@Out,$fn) if (exists($MP{$part}));
    }
  
  return @Out;
}

sub fullPathHFile
  # Determine the path 
{
  my $self=shift;
  my $libN=shift;  ## Libary name 
  my $FName=shift; ## File naem 

  return 1 if (-e $libN."/".$FName);
  my $cnt=1;
  foreach my $incD ( @{$self->{incdir}} )
    {
      $cnt++;
      return $cnt if -e ($incD."/".$FName);
    }

  die("No file ".$FName);
}
  

sub openHFile
  ##
  ## Determines were an .h/.inc file lives
  ##
{
  my $self=shift;
  my $libN=shift;
  my $FH = shift;  ## FileHANDLE to open
  my $CName = shift;
  return 1 if (open($$FH,'<',$libN."/".$CName));
  my $cnt=1;
  
  foreach my $incD ( @{$self->{incdir}} )
    {
      $cnt++;
      return $cnt if (open($$FH,'<',$incD."/".$CName));
    }
  die("No file ".$CName." in inc or ".$libN);
}

sub openCxxFile
  ##
  ## Determines were an .cxx file lives
  ## Opens the file and returns the full file name
  ##
{
  my $self=shift;
  my $libN=shift;
  my $FH = shift;  ## File to open
  my $CName = shift;
  return 1 if (open($$FH,'<',$libN."/".$CName));

  my $cnt=1;
  foreach my $incD ( @{$self->{sublibdir}} )
    {
      $cnt++;
      return $cnt if (open($$FH,'<',$incD."/".$CName));
    }
  
  die("No file ".$CName);
}

sub writeSwigInsertion
 #
 # Writes out all the includes that 
 # are going to be needed by Project.cpp file
 # Returns the reduced filename
{
  my $self=shift;
  my $fname=shift;  ## File name of 

  $fname = $1 if ($fname=~/.*\/(.*)/);
  $fname = $1 if ($fname=~/(.*)\..*/);

  my $fullName=$self->{swigShared}."\/".$fname."Inc.hxx";
  open(my $DX,'>',$fullName) or die("Unable to open ".$fullName.":".$fname);
  for my $item (@{$self->{StdLib}})
    {
      print $DX "\#include \<".$item."\>\n";
    }
  print $DX "\n";
  for my $item (@{$self->{IncLib}})
    {
      print $DX "\#include \"".$item."\"\n" if ($item!~/Swig/);
    }
  print $DX "\n";
  close($DX);
  return $fname;
}

sub printTail
{ 
  my $name=shift;
  my $tarname= ($name=~/.*\/(.*)/) ? $1 : $name;

  print "words: \n";
  print "\tgrep -v -e \'\^[[:space:][:cntrl:]]\*\$\$\' \$(ASRC) \| wc \n";
  print "\n";
  print "loc: \n";
  print "\tsloccount \$(ASRC) \n";
  print "\n";
  print "cccc: \n";
  print "\tcccc \$(ASRC) \n"; 
  print "\n";
  print "clean:\n";
  print "\trm \$(ODEP)\n";
  print "\n";
  print "tar:\n";
  print "\ttar -zcvf ".$tarname.".tgz \$(ASRC) Doxyfile Makefile ";
  print "getMk.pl getMake.pm";
  print "tar:\n";
  print "\ttar -zcvf ".$tarname."_core.tgz \$(CORESRC) ";
  print "getMk.pl getMake.pm";
  print "\n";
  print "cost:\n";
  print "\tsloccount \$(ASRC) Doxyfile Makefile ";
  print "getMk.pl getMake.pm";
  print "\n";
  print "doxygen:\n";
  print "\t(cat Doxyfile; echo \"INPUT= \$(ASRC)\" ) | doxygen -\n";
  print "\n";
  return;
}

sub printIncType
  ##
  ## Print Include type
  ##
{
  my $self=shift;
  my $libN=shift;
  my $incType=shift;  ## Number
  
  if ($incType==0)
    {
      print "\$(SRCDIR)\/";
    }
  elsif ($incType>=1)
    {
      print "\$(".incDirName($incType).")\/";
    }
  return;
}

sub IncFile
  ##
  ## Test if a include file exists
  ##
{
  my $self=shift;
  my $libName=shift;    ## Library Name (current directory) [Has priority over include list]
  my $CName = shift;    ## File to find

  return 1 if (-e $libName."/".$CName);
  my $cnt=1;
  foreach my $incD ( @{$self->{incdir}} )
    {
      $cnt++;
      return $cnt if (-e $incD."/".$CName);
    }
  print STDERR "ZERO on: ",$CName,"\n";
  return 0;
}


sub clearInclude
  ## Clears the total include files
{
  my $self=shift;
  $self->{incCppFiles} = { };
  $self->{incForFiles} = { };
  return;
}


sub addMasterProgs
  ##
  ## Add an array of programs
  ## Names : Controlflags
  ##
{
  my $self=shift;
  my $Ar=shift;
  my $Cr=shift;
  
  push(@{$self->{masterProg}},@{$Ar});
  push(@{$self->{controlflags}},@{$Cr});
  return;
}

sub addLibs
  ##
  ## Add an array of libraries
  ## and their subdirectories
  ##
{
  my $self=shift;
  my $Lr=shift;  ## Libraries
  my $Dr=shift;  ## Directories
  my $Fr=shift;  ## Libflags
  
  $self->{LibName}={};
  for(my $i=0;$i< scalar(@{$Lr});$i++)
    {
      $self->{LibName}{$Lr->[$i]}=$i;
    }

  push(@{$self->{libnames}},@{$Lr});
  push(@{$self->{sublibdir}},@{$Dr});
  push(@{$self->{libflags}},@{$Fr});
  return;
}

sub addIncDir
  ## 
  ## Set the include directory 
  ##
{
  my $self=shift;
  my $Ar=shift;

  foreach my $name (@{$Ar})
   {
     push(@{$self->{incdir}},$name);
   }
  return;
}

sub addCoreItems
  ## 
  ## Set the core directory 
  ##
{
  my $self=shift;
  my $Ar=shift;
  my $Br=shift;

  foreach my $name (@{$Ar},@{$Br})
   {
     $self->{coreDir}{$name}=1;
   }
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

  $self->{optimise}="";
  $self->{debug}="-g";  
  $self->{verbose}="";  
  my $nogsl=1;
  foreach my $FlagArray (@{$Ar},@{$self->{controlflags}},@{$self->{libflags}})
    {
      my @Flags =split " ",$FlagArray;
      foreach my $Ostr (@Flags)
        {
	  $self->{verbose}.=" -v " if ($Ostr eq "-v");
	  $self->{optimise}.=" -O2 " if ($Ostr eq "-O");
	  $self->{optimise}.=" -pg " if ($Ostr eq "-p"); ## Gprof
	  $self->{gcov}=1 if ($Ostr eq "-C");
	  $self->{debug}="" if ($Ostr eq "-g");
	  $self->{glut}=1 if ($Ostr eq "-G");    
	  $self->{boost}=1 if ($Ostr eq "-B");
	  $self->{gtk}=1 if ($Ostr eq "-K");
	  $self->{gtk}=2 if ($Ostr eq "-M");
	  $self->{pglib}=1 if ($Ostr eq "-P");
	  $self->{gsl}=1 if ($Ostr eq "-S");
	  $nogsl=-1 if ($Ostr eq "-NS");
	  $self->{xlib}=1 if ($Ostr eq "-X");
	  $self->{shared}=1 if ($Ostr eq "-s");
	  $self->{fortranlib}=1 if ($Ostr eq "-F");
	  $self->{lua}=1 if ($Ostr eq "-u");
	  $self->{swig}=1 if ($Ostr eq "-w");
	  $self->{bcomp}=$1 if ($Ostr=~/-gcc=(.*)/);
	  $self->{ccomp}=$1 if ($Ostr=~/-g\+\+=(.*)/);
	}
    }
  $self->{gsl}*=$nogsl;

  print STDERR "INIT Opt=",$self->{optimise},"\n";
  print STDERR "INIT Deb=",$self->{debug},"\n";
  print STDERR "GSL == ",$self->{gsl},"\n";
  print STDERR "G++ == ",$self->{ccomp},"\n";
  print STDERR "GCC == ",$self->{bcomp},"\n";
  return;
}

sub setPrintFile
  ## 
  ## Sets a file for output
  ## 
{
  my $self=shift;
  my $FName=shift;
  
  if ($FName)
    {
      open($self->{OFLE},'>',$FName) or die("Unable to open file :".$FName);
    }
  select($self->{OFLE});
#  $self->{OFLE}->autoflush(1);
  return;
}

sub appPrintFile
  ## 
  ## Sets a file for output
  ## 
{
  my $self=shift;
  my $FName=shift;

  if ($FName)
    {
      open($self->{OFLE},'>>',$FName);
      select($self->{OFLE});
    }
  select($self->{OFLE});
  return;
}

sub printFullDep
{
  my $self=shift;
  my $CppArray=shift;           ##  Cpp/C 
  my $CArray=shift;             ##  Cpp/C 
  my $FArray=shift;             ##  For/f

  my @incCA= sort { (lc($a) ge lc($b)) ? 1 : -1 } (keys(%{$self->{incCppFiles}}));
  my @incFA= sort { (lc($a) ge lc($b)) ? 1 : -1 } (keys(%{$self->{incFortFiles}}));
  
  
  ## Sorted list of all files (*.cxx)
  my @fileCPP= sort { (lc($a) ge lc($b)) ? 1 : -1 } (@{$CppArray});
  my @fileC= sort { (lc($a) ge lc($b)) ? 1 : -1 } (@{$CArray});
  my @fileFort= sort { (lc($a) ge lc($b)) ? 1 : -1 } (@{$FArray});

  my $incString="CSRC = ";
  for my $nx (@fileCPP,@fileC)
    {
      $incString.=$nx." ";
    }
  printString($incString,66,12);
  print "\n";
  
  $incString="FSRC = ";
  for my $nx (@fileFort)
    {
      $incString.=$nx." ";
    }
  printString($incString,66,12);
  print "\n";
  
  $incString="CINC = ";       ## Include files
  for my $nx (@incCA)
    {
      $incString.=$nx." ";
    }
  printString($incString,66,12);
  print "\n";

  $incString="FINC = ";        ## Fortran include
  for my $nx (@incFA)
    {
      $incString.=$nx." ";
    }
  printString($incString,66,12);
  print "\n";

  $incString="ODEP = ";
  for my $nx (@fileCPP,@fileC,@fileFort)
    {
      if ($nx=~/(.*)\./)
        {
	  $incString.=$1.".o ";
	}
    }

  printString($incString,66,12);
  print "\n";

  return;
}

sub printHeaders
{
  my $self=shift;
  my $GcovFlag="";
  my $OFlag=$self->{debug}." ".$self->{optimise};
  
  
  $GcovFlag.=" --coverage " if ($self->{gcov});

  print "#!-----------------------------------------------------------------------\n";
  print "SHELL=/bin/sh\n";
  print "#\n";
  print "#\n";
  print "# First where are we going to get the sources\n";
  print "SRCDIR=.\n";
  print "#\n";
  print "## Fortran compiler and compilation flags\n";
  print "#\n";
  print "FCOMPL=".$self->{fcomp}."\n";
  print "FFLAG=".$OFlag." -fPIC -fno-automatic -C -ffixed-line-length-132 -fno-backslash \\\n";
  print "         -Wall \n";
  print "#CCOMPL=/usr/local/bin/g++\n";
  print "BCOMPL=".$self->{bcomp}."\n";
  print "CCOMPL=".$self->{ccomp}."\n";

  
  print "CFLAGC=".$OFlag.$GcovFlag.$self->{cflag}."\n";
  print "CFLAGE=".$OFlag.$self->{cflag}."\n";
  print "BCFLAGC=".$OFlag.$GcovFlag.$self->{cflag}."\n";
      
  print "MCOMPL=\n";
  print "FLAGC=\n";
  print "\n";
  print "AR= ar\n";
  print "RANLIB= ranlib\n";
  print "\n";
  my $pwd=`pwd`;
  chomp $pwd;
  print "LIBOUTDIR= ".$pwd."\/".$self->{output}."\n";
  
  if ($self->{boost})
    {
      my $incL= "INCBOOST = ".$self->{boostInc};
      printString($incL,66,5);
    }

  if ($self->{gtk})
    {
      my $incL="INCGTK = ";
      $incL.=`pkg-config --cflags gtk+-2.0` if ($self->{gtk}==1);
      $incL.=`pkg-config --cflags gtkmm-2.4` if ($self->{gtk}==2);
      printString($incL,66,5);
    }
  if ($self->{gsl}>0)
    {
      my $incL= "INCGSL=".`pkg-config --cflags gsl`;
      printString($incL,66,5);
    }
  elsif ($self->{gsl}<0)
    {
      my $incL= "INCGSL= -DNO_GSL ";
      printString($incL,66,5);
    }
  if ($self->{lua})
    {
      my $incL="INCLUA="." -I\/usr/local/include/toluaxx5.1";
      printString($incL,66,5);
    }      
  print "\n";
  
  for(my $i=1;$i<=scalar(@{$self->{incdir}});$i++)
    {
      print incDirName($i)."=../".$self->{incdir}[$i-1]."\n";
    }
  print "\n";

  print "LIBS=-L/usr/X11/lib -lX11 -L/usr/lib -lm\n";
  print "FORTLIBS=-lgfortranbegin -lgfortran \n";
  print "OPENLIBS=-lglut -lGLU -lGL -L/usr/X11R6/lib -lXmu -lX11\n";
  print "MOTIF_LIBS=-L/usr/X11R6/lib -lXm -lXtst -lX11\n";
  if ($self->{boost})
    {
      my $boostlibs;
      $boostlibs="BOOSTLIBS= ".$self->{boostLib};
      printString($boostlibs,66,5);
    }
  my $gtklibs;
  $gtklibs="GTKLIBS=".`pkg-config --libs gtk+-2.0` if ($self->{gtk}==1);
  $gtklibs="GTKLIBS=".`pkg-config --libs gtkmm-2.4` if ($self->{gtk}==2);
  printString($gtklibs,66,5) if ($self->{gtk});

  my $gsllibs;
  $gsllibs="GSLLIBS=".`pkg-config --libs gsl` if ($self->{gsl}>0);
  printString($gsllibs,66,5) if ($self->{gsl}>0);

  my $xlibs;
  $xlibs="XLIBS= -L/usr/X11R6/lib -lXm -lXt -lXmu -lXp -lX11 -lSM -lICE" if ($self->{xlib});
  printString($xlibs,66,5) if ($self->{xlib});
  my $pglibs;
  if ($self->{pglib})
    {
      $pglibs="PGLIBS= -L/usr/local/pgplot ";
##      $pglibs.=" /usr/local/pgnormal/libXmPgplot.a " if ($self->{xlib});     ## Do I need -lXm
      $pglibs.="-lXmPgplot -lcpgplot -lpgplot";
      printString($pglibs,66,5);
    }
  my $lualibs;
  $lualibs="LUALIBS= -ltoluaxx5.1 ".`pkg-config --libs lua` if ($self->{lua});
  $lualibs="LUALIBS= ".`pkg-config --libs lua`." -ldl" if ($self->{swig});
  printString($lualibs,66,5) if ($self->{lua} || $self->{swig});
  print "\n";
  print "ASRC = \$(CSRC) \$(CINC) \$(FSRC) \n";
  print "\n";
  return;
}

sub processIncludeCpp
 ## 
 ## Take a list of C++ files and determine the include files.
 ## param CppAR :: Array of c++ files
 ##
{
  my $self=shift;
  my $CppAR=shift;
  my $DTA;
  foreach my $fname (@{$CppAR})
    {
      my $line;
      open($DTA,'<',$fname) or next;# die ("File not seen:".$fname);
      if ($fname!~/^test/)
        {
	  while(defined($line=<$DTA>))
	    {
	      if ($line=~/^\s*\#include\s+\"(.*)\"/)
	        {
		  $self->{incCppFiles}{$1}++;
		  ## Check for dependent include HERE:
		}
	    }
	}
      else
        {
	  while(defined($line=<$DTA>))
	    {
	      if ($line=~/^\s*\#include\s+\"(.*)\"/)
	        {
		  $self->{incTestCppFiles}{$1}++;
		  ## Check for dependent include HERE:
		}
	    }
	}
      close($DTA);  
    }
  
  return;
}

sub processIncludeFor
  ##
  ## Check a fortran file for include 
  ## 
{
  my $self=shift;
  my $ForAR=shift;
  
  foreach my $fname (@{$ForAR})
    {
      my $line;
      open(my $DTA,'<',$fname) or next;# die ("File not seen:".$fname);
      while(defined($line=<$DTA>))
        {
	  if ($line=~/^\s*\#include\s+\"(.*)\"/)
	    {
	      $self->{incForFiles}{$1}++;
	      ## Check for dependent include HERE:
	    }
	}
      close($DTA);  
    }

  return;
}


sub processCPP
  ##
  ## Takes a list of files an creates a list
  ## of dependencys
  ## Self/libName/array of CPP files
  ##
  ## Flags : 0 == Cpp
  ## Flags : 1 == Fortran
  ## Flags : 2 == C
{
  my $self=shift;
  my $libName=shift;
  my $array=shift;
  my $fortFlag=(@_) ? shift : 0;  

  my $DTA;
  my $line;
  for my $cname (@{$array})
    {
      my $activeGTK=0;
      my $activeGSL=0;
      my $activeBOOST=0;
      ## Determine the active Include 
      my %incActive;
      my $basename=$cname;
      $basename=$1 if ($basename=~/\/(.*)/);
      my $namePlusExt=$basename;
      ##
      ## Loop to check different include files:
      ## 
      my %activeInclude;
      my $activeBase=0;
      $basename=~s/\..*//;               ## Remove .xxx
      my $compName=$basename."\.h";      ## Allow base.h to come to the top
      my %incC;
      my $incType=0;
      ## Actual loop
      do
	{
	  $incType=$self->openHFile($libName,\$DTA,$cname);  ## CNAME is cxx file
##	  open(DTA,$cname) or die ("Unable to open c++ file:  ".$cname);
	  while (defined($line=<$DTA>))
	    {
	      chomp $line;
	      $activeBOOST=1 if ($self->{boost} && $line=~/^\s*\#include\s+\<boost/);
	      $activeGTK=1 if ($self->{gtk} && $line=~/^\s*\#include\s+\<gtk/);
	      $activeGTK=1 if ($self->{gtk} && $line=~/^\s*\#include\s+\<gdk/);
	      $activeGSL=1 if ($self->{gsl} && $line=~/^\s*\#include\s+\<gsl/);
	      my $found=($fortFlag==1) ?  
		  ($line=~/^\s*include\s+\'(.*)\'/i) :                 
		  ($line=~/^\s*\#include\s+\"(.*)\"/);
		  
	      if ($found)
		{
		  my $tname=$1;
		  my $incType = $self->IncFile($libName,$tname);
		  $incActive{$incType}=1;
		  if ($tname eq $compName)
		    {
		      $activeBase = $incType;
		    }
		  else
		    {
		      $incC{$tname}= $incType;
		    }
		  ## Make a list of to be included files
		  $activeInclude{$tname}=0 if (!exists($activeInclude{$tname}));  
		}
	    }
	  close($DTA);
	  $cname="NULL";
	  foreach my $ky (keys %activeInclude)
	    {
	      if (!$activeInclude{$ky})
		{
		  $cname=$ky;
		  $activeInclude{$ky}=1;
		  last;
		}
	    }
	} until ($cname eq "NULL");
      
      my @incOut= sort { (lc($a) ge lc($b)) ? 1 : -1 } keys(%incC);
      if ($activeBase)
	 {
	   print $basename,".o: \$(SRCDIR)\/",$namePlusExt," ";
	   $self->printIncType($libName,$activeBase-1);
	   print $basename,".h";
	 }
      else
	{
	  print $basename,".o: \$(SRCDIR)\/",$namePlusExt,"                        ";
	}
      print " \\" if (@incOut);
      print "\n";
      
      my $cnt=1;
      for(my $i=0;$i<=$#incOut;$i++)
	{
	  my $item=$incOut[$i];
	  if ($cnt % 2)
	    {
	      print "             ";
	      $self->printIncType($libName,$incC{$item}-1);
	      print $item," ";
	    }
	  else 
	    {
	      $self->printIncType($libName,$incC{$item}-1);
	      print $item," ";
	      if ($i!=$#incOut)
		{
		  print " \\\n";
		}
	    }
	  $cnt++;
	}
      
      print "\n" if (@incOut);
      my $compLine;
      $compLine="\t\$(CCOMPL) -c \$(CFLAGC) " if ($fortFlag==0);
      $compLine="\t\$(FCOMPL) -c \$(FFLAG) " if ($fortFlag==1);
      $compLine="\t\$(BCOMPL) -c \$(BCFLAGC) " if ($fortFlag==2);
      
      foreach my $ii (keys %incActive)
	{
	  if (incDirName($ii-1) ne "NULL")
	    {
	      $compLine.="-I\$(".incDirName($ii-1).") ";
	    }
	}
      $compLine.="\$(INCGTK) " if ($activeGTK);
      $compLine.="\$(INCBOOST) " if ($activeBOOST);
      $compLine.="\$(INCGSL) " if ($activeGSL);
      $compLine.="\$(SRCDIR)\/".$namePlusExt."\n";
      printString($compLine,72,15);
      print "\n";
    }
  return;
}

sub printMainAll
 ##
 ## Prints the Main get all 
 ##
{
  my $self=shift;
  

  my $Target="all : ";
  my $Phony=".PHONY : ";
  foreach my $name (@{$self->{libnames}})
    {
      $Target.="lib".ucfirst($name)." ";
      $Phony.="lib".ucfirst($name)." ";
    }
  # Add lua library (not a getMakefile call)
  if ($self->{lua})
    {
      $Target.="libScript ";
      $Phony.="libScript ";
    }
  # Add swig/lua library (not a getMakefile call)
  if ($self->{swig})
    {
      $Target.="libSwig ";
      $Phony.="libSwig ";
    }

  $Target.="mainProg ";
  $Phony.="mainProg ";
  foreach my $name (@{$self->{masterProg}})
    {
      $Target.=$name." " if ($name ne "null");
    }
  printString($Target,66,12);
  print "\n";
  printString($Phony,66,12);
  print "\n";


  ## MASTER PROG BUILD
  for(my $i=0;$i<scalar(@{$self->{masterProg}});$i++)
    {
      my $name=$self->{masterProg}[$i];
      my $depLine = $name." : mainProg Main/".$name.".o ";
      my $compLine;
      my $fileExt=getFileExt("Main/".$name);
      if ($fileExt eq "cxx")
        {
	  $compLine = "\t\$(CCOMPL) \$(CFLAGC) -o ".$name." Main/".$name.".o ";
	}
      elsif ($fileExt eq "c")
        {
	  $compLine = "\t\$(CCOMPL) \$(CFLAGC) -o ".$name." Main/".$name.".o ";
	}
      elsif ($fileExt eq "for")
        {
	  $compLine = "\t\$(FCOMPL) \$(FFLAG) -o ".$name." Main/".$name.".o ";
	}
      my $NLib= scalar(@{$self->{depList}[$i]});
      my $libExt=($self->{shared}) ? ".so" : ".a";

#      $compLine.="-fPIC -Wl,--as-needed -Wl,-rpath,\$(LIBOUTDIR) "
#	  if ($self->{shared});
      $compLine.="-fPIC -Wl,-rpath,\$(LIBOUTDIR) "
	  if ($self->{shared});

      for(my $j=0;$j<$NLib;$j++)
	{
#	  $depLine.="\$(LIBOUTDIR)/lib".ucfirst($self->{libnames}[$self->{depList}[$i][$j]]).$libExt." ";
	  $depLine.= "lib".ucfirst($self->{libnames}[$self->{depList}[$i][$j]])." "; 
	  $compLine.="\$(LIBOUTDIR)/lib".ucfirst($self->{libnames}[$self->{depList}[$i][$j]]).$libExt." ";
	}
      if ($self->{lua})
	{
	  $compLine.="\$(LIBOUTDIR)/libScript".$libExt." ";
	  $depLine.="\$(LIBOUTDIR)/libScript".$libExt." ";
	}

      if ($self->{swig})
	{
	  $compLine.="\$(LIBOUTDIR)/libSwig".$libExt." ";
	  $depLine.="\$(LIBOUTDIR)/libSwig".$libExt." ";
	}
      if ($self->{ccomp} eq "clang")
	{
	  $compLine.="-lstdc++ ";
	}

      $compLine.= "-lgcov " if ($self->{gcov});
      $compLine.=addFlags($self->{controlflags}[$i]); 
      printString($depLine,66,10);
      printString($compLine,66,12);
      print "\n";
    }

  ## NOW PRINT downlevel makes
  for(my $i=0;$i<scalar(@{$self->{libnames}});$i++)
    {
      my $name=$self->{libnames}[$i];
      print "lib".ucfirst($name)." : \n";
      print "\t\@\${MAKE} -C ./".$self->{sublibdir}[$i]." all\n";
      print "\n";
    }
  if ($self->{lua})
    {
      print "lib".$self->{luaShared}." : \n";
      print "\t\@\${MAKE} -C ./".$self->{luaShared}." all\n";
      print "\n";
    }
  if ($self->{swig})
    {
      print "lib".$self->{swigShared}." : \n";
      print "\t\@\${MAKE} -C ./".$self->{swigShared}." all\n";
      print "\n";
    }

  ## BUILD MAIN
  print "mainProg : \n";
  print "\t\@\${MAKE} -C ./Main all\n";
  print "\n";


  ##  TAR / DOXYGEN / CCCC / WORDS / ETAGS

  push(@{$self->{sublibdir}},"Main");

  my $PWDname=`pwd`;
  $PWDname=~/.*\/(.*)/;
  my $tarname=$1;
  my $tarOut="\ttar zcvf ".$tarname.".tgz ";
  my $coreOut="\ttar zcvf ".$tarname."_core.tgz ";
  my $doxOut="\t(cat Doxyfile; echo \"INPUT = \" \`ls ";
  my $etagsOut="\tetags ";
  my $cccOut="\tcccc ";
  my $wordsOut="\tgrep -v -e \'\^[[:space:][:cntrl:]]\*\$\$\' ";
  my $slocOut="\tsloccount ";
  my $uniqOut="\t/home/ansell/exe/C++/uniqLines.pl > AllLines.txt ";
  my $cleanOut="\trm -f ";

  foreach my $tname (@{$self->{sublibdir}})
    {
      my $cxxItem=checkExists("./".$tname."/*.cxx ");
      $coreOut.=$self->checkCoreExists("./".$tname,"*.cxx ");
      my $cItem=checkExists("./".$tname."/*.c ");
      $coreOut.=$self->checkCoreExists("./".$tname,"*.c ");
      my $fItem=checkExists("./".$tname."/*.f ");
      $coreOut.=$self->checkCoreExists("./".$tname,"*.f ");
      my $allItem=$cxxItem.$cItem.$fItem;
      $tarOut .= $allItem;
      $doxOut .= $allItem;
      $etagsOut.= $allItem;
      $cccOut.= $cxxItem.$cItem;
      $wordsOut.= $allItem;
      $slocOut.= $allItem;
      $uniqOut.= $allItem;
      $cleanOut.= "./".$tname."/*.o "
    }
#  $tarOut.= checkExists("./old/*.cxx ");
#  $tarOut.= checkExists("./old/*.c ");
#  $tarOut.= checkExists("./old/*.f ");
#  $tarOut.= checkExists("./old/*.h ");
  $tarOut.= checkExists("./".$self->{luaShared}."/*.pkg ") if ($self->{lua});
  $tarOut.= checkExists("./".$self->{luaShared}."/*.hxx ") if ($self->{lua});
  $tarOut.= checkExists("./".$self->{swigShared}."/*.i ") if ($self->{swig});
  $tarOut.= "Doxyfile ";
  foreach my $tname (@{$self->{incdir}})
    {
      my $hItem=checkExists("./".$tname."/*.h ");
      $coreOut.=$self->checkCoreExists("./".$tname,"*.h ");
      $tarOut.= $hItem;
      $tarOut.= checkExists("./".$tname."/*.hxx ");
      $tarOut.= checkExists("./".$tname."/*.inc ");

      $doxOut.= $hItem;
      $etagsOut.= $hItem;
      $cccOut.= $hItem;
      $wordsOut.= $hItem;
      $slocOut.= $hItem;
      $uniqOut.= $hItem;
    }
  
  $tarOut.="getMake.pm getMk.pl ";
  $coreOut.="getMake.pm getMk.pl ";

  $cleanOut.= "./lib/*.so ";
  $wordsOut.=" | wc";

  print "tar:\n";
  printString($tarOut,60,12);
  print "\n";
  print "tarcore:\n";
  printString($coreOut,60,12);
  print "\n";
  print "doxygen:\n";
  $doxOut.="\` \) \| doxygen \-";
  printString($doxOut,60,12);
  print "\n";
  print "cccc:\n";
  printString($cccOut,60,12);
  print "\n";
  print "words:\n";
  printString($wordsOut,60,12);
  print "sloc:\n";
  printString($slocOut,60,12);
  print "unique:\n";
  printString($uniqOut,60,12);
  print "\n";
  print "tags:\n";
  printString($etagsOut,60,12);
  print "\n";
  print "clean:\n";
  printString($cleanOut,60,12);
  
  return;
}

sub printLibDep
 ## 
 ## Prints the lib dependency for a sub-library
 ## Makefile.
 ##
{
  my $self=shift;
  my $libname=shift;

  my $compName=($self->{fortranlib}) ? "FCOMPL" : "CCOMPL";
  if (!$self->{shared})
    {
      print "all: \$(LIBOUTDIR)/lib".ucfirst($libname)."\.a\n";
      print "\n";
      print "\$(LIBOUTDIR)/lib".ucfirst($libname).".a : \$(ODEP) \n";
      print "\t\$(AR) ru \$(LIBOUTDIR)/lib".ucfirst($libname).".a \\\n"; 
      print "       \$(ODEP) \n";
      print "\t\$(RANLIB) \$(LIBOUTDIR)/lib".ucfirst($libname).".a\n"; 
      print "\n";
    }
  else
    {
      print "all: \$(LIBOUTDIR)/lib".ucfirst($libname)."\.so \n";
      print "\n";
      print "\$(LIBOUTDIR)/lib".ucfirst($libname).".so : \$(ODEP) \n";
      print "\t\$(".$compName.") ";
      print "-lgcov " if ($self->{gcov});
      print "-fPIC -shared ".$self->{debug}." "
	  .$self->{optimise}." -o \$(LIBOUTDIR)\/lib".
	  ucfirst($libname).".so \\\n"; 
      print "       \$(ODEP) \n";
      print "\n";
    }
  return;
}

sub processSwigCpp
  ##
  ## Read a CPP file and determine the include 
  ## files 
  ##
{
  my $self=shift;  
  my $incFile=shift;  ## Name of the include file
  my $DTX;
  my @SLIB;
  my @ILIB;
  
  my $name=$incFile;
  ## Determine the equivilent c++ name:
  if ($incFile=~/(.*)\/(.*)\.h/)
    {
      $name=$2;
    }
  elsif ($incFile=~/(.*)\.h/)
    {
      $name=$1;
    }  

  ## Find the cpp existance point
  ##  my $srcFile;

  ## This dies if it failes
  $self->openCxxFile($self->{swigShared},\$DTX,$name.".cxx");
  my $line;
  while(defined($line=<$DTX>))
    {
      if ($line=~/^\s*\#include\s+\"(.*)\"/)
        {
	  push(@ILIB,$1);
	}
      elsif ($line=~/^s*\#include\s+\<(.*)\>/)
        {
	  push(@SLIB,$1);
	}
    }
  close($DTX);
  buildMaster($self->{StdLib},\@SLIB);
  buildMaster($self->{IncLib},\@ILIB);
  return;
}

sub processSwigPackage
  ## 
  ## Create a swig package file
  ## Read a .i file
  ##
{
  my $self=shift;
  my $file=shift;    ## .I to process
  my $tag;
  ## calculate the tag

  die ("NO TAG for ",$file) if ($file!~/(.+)\.i/);
  $tag=$1;
  $tag=$1 if ($tag=~/.*\/(.*)/);
  my $outLine="INCDEP_".uc($tag)."=";

  $self->{StdLib}=[ ];
  $self->{IncLib}=[ ];

  my $line;
  my $pBcount=0;
  open(my $DTA,'<',$file) or return;
  while(defined($line=<$DTA>))
    {
      chomp $line;
      $pBcount-- if ($line=~/\%\}/);
      $pBcount++ if ($line=~/\%\{/);

      if ($pBcount==0 && $line=~/^\%include\s+\"(.*)\"/)
	{
	  my $tname=$1;
	  if ($tname!~/\.i\s*$/)
	    {
	      $tname=$1 if ($tname=~/.*\/(.*)/);
	      ##
	      $self->processSwigCpp($tname);
	    }
	}
    }
  close($DTA);
## 
## BuildMaster has set arrays
## 
  foreach my $inc (@{$self->{IncLib}})
    {
      my $index=$self->IncFile($self->{swigShared},$inc);
      $self->{swigIndex}{$tag}{$index}++;
      $outLine.="\$(" .incDirName($index-1).")\/".$inc." ";
    }
  printString($outLine,66,5);
  print "\n";
  return;
}


sub runSubDir
  ## Creates the makefile for a library directory
{
  my $self=shift;
  my $index=0;
  foreach my $libdir (@{$self->{sublibdir}})
    {
## Clear include for this Makefile
      $self->clearInclude();
      $self->setPrintFile($libdir."/Makefile");
      
      $self->printHeaders();
      my @CppFiles=getCppFiles($libdir);
      $self->processIncludeCpp(\@CppFiles);

      my @CFiles=getCFiles($libdir);
      $self->processIncludeCpp(\@CFiles);

      my @ForFiles=getForFiles($libdir);
      $self->processIncludeFor(\@ForFiles);
      
      $self->printFullDep(\@CppFiles,\@CFiles,\@ForFiles);

      $self->printLibDep($self->{libnames}[$index]);
      $self->processCPP($libdir,\@CppFiles);
      $self->processCPP($libdir,\@ForFiles,1);
      $self->processCPP($libdir,\@CFiles,2);

      printTail($libdir);
      $index++;
    }
  return;
}

sub runMainDir
  ## 
  ## Write out the Makefile to the main directory
  ## 
{
  my $self=shift;
  my $libdir="./Main";
## Clear include for this Makefile
  $self->clearInclude();
  $self->setPrintFile($libdir."/Makefile");
  
  $self->printHeaders();
  my @CppFiles=getCppFiles($libdir);
  @CppFiles=$self->selectMaster(\@CppFiles);
  $self->processIncludeCpp(\@CppFiles);

  my @CFiles=getCFiles($libdir);
  @CFiles=$self->selectMaster(\@CFiles);
  $self->processIncludeCpp(\@CFiles);

  my @ForFiles=getForFiles($libdir);
  @ForFiles=$self->selectMaster(\@ForFiles);
  $self->processIncludeFor(\@ForFiles);

  $self->printFullDep(\@CppFiles,\@CFiles,\@ForFiles);


  $self->createMainDep();

  print "all: \$(MAINDEP)\n\n";  

  $self->processCPP($libdir,\@CppFiles);
  $self->processCPP($libdir,\@ForFiles,1);
  $self->processCPP($libdir,\@CFiles,2);

  printTail($libdir);
  return;
}


sub runSwigDir
  ## Create the makefile in the swig directory
{
  my $self=shift;
  return if (!$self->{swig});
  
  $self->clearInclude();
  $self->setPrintFile($self->{swigShared}."/Makefile");
  $self->printHeaders();
  my @SwigFiles=getSwigFiles($self->{swigShared});
  # Loop over all .i files and process


  my $ODEP="ODEP = ";
  foreach my $item (@SwigFiles)
    {
      $self->processSwigPackage($item);
      my $part=$self->writeSwigInsertion($item);
      $ODEP.=$part."_wrap.o ";
    }
  printString($ODEP,65,6);
  print "\n";

  
  $self->printLibDep("Swig");


## UNMOD  
  foreach my $item (keys(%{$self->{swigIndex}}))
    {
      print $item."_wrap.o: ".$item."_wrap.cpp\n";
      my $outStr="\t\$(CCOMPL) \$(CFLAGC) -c ";
      $outStr.="\$(INCGTK) " if $self->{gtk};
      $outStr.="\$(INCGSL) " if $self->{gsl};

      foreach my $kv (keys(%{$self->{swigIndex}{$item}}))
        {
	  $outStr.="-I\$(" .incDirName($kv-1).") ";
	}
      $outStr.=" ".$item."_wrap.cpp";
      printString($outStr,66,12);
      print "\n";
      print $item."_wrap.cpp: \$(INCDEP_".uc($item).") ".
	  $item.".i\n";
      print "\tswig -c++ -lua -o ".$item."_wrap.cpp ".$item.".i \n";
      print "\n";
    }
  
  printTail($self->{swigShared});
  return;
}


sub addDepUnit
  ## Ugly function to set dep list.
  ## Does it actually work??
{
  my $self=shift;
  my $name = shift;
  my $Ar=shift;
  
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
  for(my $i=scalar(@{$self->{depList}});$i<=$index;$i++)
    {
      push(@{$self->{depList}}, [ ]);
    }

  push(@{$self->{depList}[$index]},@{$Ar});  
  return;
}

sub findNonUniq
## Find no unique files
{
  my $self=shift;
  my $seaStr="find ./ -name \"*.[ch]*\" | grep -v old".
      " | cut -d \\/ -f 3 | sort | uniq -d";
  my $items=`$seaStr`;
  if (length($items))
    {
      my @pts=split /\n/,$items;
      print "PT == ",scalar(@pts),"\n";
      print "Repeated FILES :: ",join(" ",@pts),"\n";
    }

  $seaStr="find ./ -name \"*.o\" | grep -v old".
      " | cut -d \\/ -f 3 | sort | uniq -d";
  $items=`$seaStr`;
  if (length($items))
    {
      my @pts=split /\n/,$items;
      print "PT == ",scalar(@pts),"\n";
      print "Repeated OBJECTS :: ",join(" ",@pts),"\n";
    }

  return;
}

1;
