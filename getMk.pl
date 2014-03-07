#!/usr/bin/perl -w

## use lib "/home/ansell/exe/getMk";

use getMake;
use strict;

## EXECUTABLES
my @masterprog=("fullBuild","ess","muBeam","sinbad","t1Real",
		"sns","reactor","t1MarkII","testMain"); 


my @noncompile=("bilbau","clayer","cuBuild","d4c","detectSim",
		"epb","ess","fussion","lens",
                "pressure","reactor","vcn","power");


## CXX Directory
my @sublibdir=("src","attachComp","bibBuild","bnctBuild",
                "build","chip","construct","crystal",
    	       "cuBlock","d4cModel","delft","endf",
	       "epbBuild","essBuild","funcBase","geometry",
	       "global","imat","input","instrument",
	       "lensModel","log","md5","moderator",
	       "mersenne","monte","muon","physics",
	       "poly","process","scatMat","sinbadBuild",
	       "snsBuild","source","support", "tally",
	       "t1Build","t1Upgrade", "transport","visit",
	       "weights","world","work","xml",
	       "zoom","special","test");                            

my @core=qw( src attachComp construct crystal endf funcBase
             geometry global input instrument log md5 monte 
             mersenne physics poly process scatMat source 
             support tally transport visit weights
             world work xml special test);

my @coreInc=qw( include  attachCompInc constructInc crystalInc 
             endfInc funcBaseInc geomInc globalInc inputInc
             instrumentInc logInc md5Inc  mersenneInc
             monteInc muonInc physicsInc polyInc processInc scatMatInc
             sourceInc supportInc tallyInc transportInc visitInc
             weightsInc worldInc workInc xmlInc specialInc testInclude);

my @libnames=@sublibdir;

## INCLUDES
my @incdir=("include","attachCompInc","bibBuildInc",
	    "bnctBuildInc","buildInc","chipInc",
	    "constructInc","crystalInc","cuBlockInc","d4cModelInc",
	    "delftInc","endfInc","epbBuildInc","essBuildInc",
            "funcBaseInc","geomInc","globalInc","imatInc",
	    "inputInc","instrumentInc","lensModelInc","logInc",
	    "md5Inc","mersenneInc","moderatorInc","monteInc",
	    "muonInc","physicsInc","polyInc",
	    "processInc","scatMatInc","sinbadBuildInc","snsBuildInc",
	    "sourceInc","specialInc","supportInc","tallyInc",
	    "t1BuildInc","t1UpgradeInc","transportInc","visitInc",
	    "weightsInc","workInc","worldInc","xmlInc",
	    "zoomInc","testInclude");   ## Includes

## Flags on executables
my @controlflags=("-S -B","-S -B","-S -B","-S -B",
		  "-S -B","-S -B","-S -B","-S -B",
		  "-S -B","-S -B","-S -B","-S -B",
                  "-S -B");

## Lib flags
my @libflags=("","","","","",
	      "","","","","","",
	      "","","","","-S","",
	      "","","","",
	      "","","","",
	      "","","","",
	      "","","","","",""."");

my $gM=new getMake;

$gM->addMasterProgs(\@masterprog,\@controlflags);
$gM->addCoreItems(\@core,\@coreInc);
$gM->addLibs(\@libnames,\@sublibdir,\@libflags);
$gM->addIncDir(\@incdir);

# my @libnames=("mcnpx","build","funcBase","geometry",  0,1,2,3
#	      "log","monte","mersenne","poly",          4,5,6,7,
#	      "process","support","test","lensModel");                  


# $gM->addDepUnit("bilbau",   [2,10,39,0,27,18,33,25,14,21,35,6,7,38,30,22,11,29,41,42,25,15,24,0,43,28,34,40,16,1,39,4]);


$gM->addDepUnit("bilbau", [2,4,39,5,37,17,23,4,44,6,7,38,30,11,36,0,27,18,29,33,25,14,21,15,35,24,0,42,43,28,34,41,40,22,16,1,39,28]);

$gM->addDepUnit("pressure", [39,44,0,25,16,14,21,5,25,24,27,15,35,42,43,28,18,29,39,40,22,1,39]);
$gM->addDepUnit("divide",   [45,39,5,4,44,0,27,25,14,21,25,24,0,15,18,29,42,43,28,34,40,22,16,1,39]);
$gM->addDepUnit("fullBuild",[4,39,5,23,4,44,17,0,27,6,38,30,7,11,18,33,25,14,21,35,29,35,41,25,15,24,0,42,43,28,34,40,22,16,1,39]);
$gM->addDepUnit("d4c",      [9,20,39,0,27,18,19,33,25,14,21,6,7,38,30,11,38,30,35,29,35,41,6,25,15,24,0,43,28,34,40,42,22,16,1,39]);
$gM->addDepUnit("lens",     [20,39,0,27,18,33,25,14,21,6,7,38,30,11,38,30,35,29,35,41,6,25,15,24,0,43,28,34,40,22,16,1,39]);
$gM->addDepUnit("simple",   [39,27,0,18,33,25,14,21,7,38,30,11,29,35,41,25,15,24,0,43,28,34,40,22,16,1,29,42,35,39]);
$gM->addDepUnit("t1MarkII", [37,36,17,5,4,39,23,44,0,6,7,38,30,11,27,18,29,33,25,14,21,25,35,15,24,0,41,42,43,28,34,40,22,16,1,39,28]);
$gM->addDepUnit("ts1layer", [4,39,5,23,4,44,0,27,18,29,25,14,21,25,35,15,24,0,42,43,28,34,40,22,16,1,39,28]);
$gM->addDepUnit("t1Real",   [36,17,5,4,39,23,44,0,6,7,38,30,11,27,18,29,33,25,14,21,25,35,15,24,0,41,42,43,28,34,40,22,16,1,39,28]);
$gM->addDepUnit("reactor",  [10,39,0,27,18,33,25,14,21,6,38,30,7,11,35,29,35,41,25,15,24,0,43,28,34,40,22,42,16,1,39]);
$gM->addDepUnit("siMod",    [39,0,27,18,33,25,14,21,35,6,7,38,30,11,29,41,25,15,24,0,43,28,34,40,16,1,39]);
$gM->addDepUnit("cuBuild",  [8,10,39,0,27,18,33,25,14,21,35,6,7,38,30,22,11,29,41,42,25,15,24,0,43,28,34,40,16,1,39]);
$gM->addDepUnit("ess",      [13,39,0,27,18,33,25,14,21,35,6,7,38,30,22,11,29,41,42,25,15,24,0,43,28,34,40,16,1,39]);
$gM->addDepUnit("sinbad",   [31,39,0,27,18,33,25,14,21,35,6,7,38,30,22,11,29,41,42,25,15,24,0,43,28,34,40,16,1,39]);
$gM->addDepUnit("sns",      [32,39,0,27,18,33,25,14,21,35,6,7,38,30,22,11,29,41,42,25,15,24,0,43,28,34,40,16,1,39]);
$gM->addDepUnit("epb",      [12,10,39,0,27,18,33,25,14,21,35,6,7,38,30,22,11,29,41,42,25,15,24,0,43,28,34,40,16,1,39]);
$gM->addDepUnit("muBeam",   [26,36,17,5,4,39,23,44,0,6,7,38,30,11,27,18,29,33,25,14,21,25,35,15,24,0,41,42,43,28,34,40,22,16,1,39,28]);
$gM->addDepUnit("bnct",     [3,36,17,5,4,39,23,44,0,6,7,38,30,11,27,18,29,33,25,14,21,25,35,15,24,0,41,42,43,28,34,40,22,16,1,39,28]);

$gM->addDepUnit("testMain", [46,4,39,5,37,17,23,4,44,6,7,38,30,11,36,0,27,18,29,33,25,14,21,15,35,24,0,42,43,28,34,41,40,22,16,1,39,28]);

##
## START OF MAIN:::
##

if (@ARGV==0)
  {
    print STDERR "Options \n";
    print STDERR "   -O :: optimise \n";
    print STDERR "   -P :: profile \n";
    print STDERR "   -G :: Glut \n";
    print STDERR "   -g  :: No debug\n";
    print STDERR "   -K  :: GTK \n";
    print STDERR "   -S  :: GSL \n";
    print STDERR "   -NS :: No GSL \n";
    print STDERR "   -M  :: GTKmm \n";
    print STDERR "   -L  :: Don't make library\n";
    print STDERR "   -m  :: masterprog : Set master prog\n";
    print STDERR "   -o  :: outputDir  \n";
    print STDERR "   -s  :: shared  \n";
    print STDERR "   -u :: lua  \n";
    print STDERR "   -v :: Add -v to the linker  \n";
    print STDERR "   -w :: SWIG lua  \n";
    print STDERR "   -x :: executable name \n"; 
    $gM->findNonUniq();
    exit(1);
  }


$gM->setParameters(\@ARGV);
$gM->setPrintFile("Makefile");
$gM->printHeaders();

$gM->runSubDir();
$gM->runMainDir();

$gM->runSwigDir();

$gM->appPrintFile("Makefile");
$gM->printMainAll();
