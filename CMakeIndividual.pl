#!/usr/bin/perl -w
use strict;

sub makeInclude
{
  my $dir=shift;
  my $f=$dir;
  my %special =(
      "test" => "testInclude",
      "src" => "include",
      "geometry" => "System/geomInc"
      );  
    
  $f=$1 if ($dir=~/\/([^\/]+)$/);	
  return $special{$f} if (exists($special{$f}));
  
  return $dir."Inc";
}
	

    
sub getCXXFiles
{
  my $searchName=shift;

  my $out;
  my $files=`ls $searchName`;
  my @F=split ' ',$files;
  foreach my $f (@F)
    {
      if ($f=~/\.cxx$/)
        {
	  $f=$1 if ($f=~/\/([^\/]+)$/);	
	  $out.=$f." ";
	}
    }
  return $out;
}

sub getHFiles
{
  my $searchName=shift;

  my $out;
  my $files=`ls $searchName`;
  my @F=split ' ',$files;
  foreach my $f (@F)
    {
      if ($f=~/\.h$/)
        {
	  $f=$1 if ($f=~/\/([^\/]+)$/);	
	  $out.=$f." ";
	}
    }
  return $out;
}

sub printSet
{
  my $dir=shift;
  my $name=$dir;
  $name=$1 if ($dir=~/\/([^\/]+)$/);
      
  my $files=shift;

  print "set (",$name,"Sources";
  my @F=split ' ',$files;
  for(my $i=0;$i<scalar(@F);$i++)
  {
    my $fname=$F[$i];
    if ($fname=~/\.cxx$/)
    {
      print "\n    " if ( !($i % 3) );
      print $fname," ";
    }
  }
  print "\n)\n\n";
  return;
}

sub printLib
{
  my $dir=shift;
  my $name=$dir;
  $name=$1 if ($dir=~/\/([^\/]+)$/);
      

  print "add_library (",$name," SHARED\n";
  print "  \${".$name."Sources}\n";
  print ")\n";

  return;
}

sub printTarget
{
  my $dir=shift;
  my $extra=shift;
  my @Extra;

  @Extra=split ' ',$extra if (defined($extra));

  my %lookup = (@Extra) ? map {$_ => 1} @Extra : ();
  my $name=$dir;

  my $incName=makeInclude($dir);
  
  $name=$1 if ($dir=~/\/([^\/]+)$/);  ## remove Sytem/etc...
      
  print "target_include_directories (",$name," PUBLIC\n";
  print "   \${SYSTEM_INCLUDE}\n";
  print "   \${GENERAL_INCLUDE}\n";
  print "   \${ESS_INCLUDE}\n"  if (exists($lookup{"ess"}));
  print "   \${MAXIV_INCLUDE}\n"  if (exists($lookup{"maxiv"}));
  print "   \${CMAKE_SOURCE_DIR}/".$incName."\n" if (exists($lookup{"base"}));
  print "   \${CMAKE_SOURCE_DIR}/Model/ralBuildInc\n" if (exists($lookup{"ral"}));
  print "   \${CMAKE_SOURCE_DIR}/Model/photonInc\n" if (exists($lookup{"photon"}));
  print "   \${CMAKE_SOURCE_DIR}/Model/MaxIV/LinacInc\n" if (exists($lookup{"linac"}));
  print ")\n\n";

  return;
}

sub printFILE
{
  my $dir=shift;
  my $name=$dir;

  my $incName=makeInclude($dir);
  $incName=$1 if ($incName=~/\/([^\/]+)$/);
  
  print "file(RELATIVE_PATH tarDIR \n";
  print "    \"\${CMAKE_BINARY_DIR}\"\n";
  print "    \"\${CMAKE_CURRENT_SOURCE_DIR}\")\n\n";
  
  print "file(RELATIVE_PATH tarINC \n";
  print "    \"\${CMAKE_BINARY_DIR}\"\n";
  print "    \"\${CMAKE_CURRENT_SOURCE_DIR}\/..\/".$incName."\")\n\n";

  return;
}
  
sub printTAR
{
  my $dir=shift;
  my $srcFiles=shift;
  my $incFiles=shift;

  my $name=$dir;
  $name=$1 if ($dir=~/\/([^\/]+)$/);
      

  print "set (SRC_LIST \${SRC_LIST}\n";
  my @F=split ' ',$srcFiles;
  foreach my $fname (@F)
  {
    print "  \${tarDIR}\/".$fname."\n";
  }

  my @G=split ' ',$incFiles;
  foreach my $fname (@G)
  {
    print "  \${tarINC}\/".$fname."\n";
  }
  print "  \${tarDIR}/CMakeLists.txt PARENT_SCOPE)\n\n";
  return;
}



my $dir=$ARGV[0];

my $extra;
for(my $i=1;$i<@ARGV;$i++)
{
  $extra.=$ARGV[$i]." ";
}

my $files=getCXXFiles($dir);

my $incDir=makeInclude($dir);
my $incfiles=getHFiles($incDir);

printSet($dir,$files);
printLib($dir);
printTarget($dir,$extra);
printFILE($dir);
printTAR($dir,$files,$incfiles);
