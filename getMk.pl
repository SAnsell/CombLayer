#!/usr/bin/perl -w

## use lib "/home/ansell/exe/getMk";

use getMake;
use strict;

## EXECUTABLES
my @masterprog=("fullBuild","ess","muBeam","bnct","bilbau",
		"sns","t1Real","t1MarkII","testMain"); 

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
	       "poly","process","scatMat","snsBuild",
	       "source","support", "tally","t1Build",
	       "t1Upgrade", "transport","visit","weights",
	       "world","work","xml","zoom",
	       "special","test");                            

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
	    "processInc","scatMatInc","snsBuildInc","sourceInc","specialInc",
	    "supportInc","tallyInc","t1BuildInc","t1UpgradeInc",
	    "transportInc","visitInc","weightsInc","workInc",
	    "worldInc","xmlInc","zoomInc","testInclude");   ## Includes

## Flags on executables
my @controlflags=("-S -B","-S -B","-S -B","-S -B",
		  "-S -B","-S -B","-S -B",
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


# $gM->addDepUnit("bilbau",   [2,10,38,0,27,18,32,25,14,21,34,6,7,37,30,22,11,29,40,41,25,15,24,0,42,28,33,39,16,1,38,4]);


$gM->addDepUnit("bilbau", [2,4,38,5,36,17,23,4,43,6,7,37,30,11,35,0,27,18,29,32,25,14,21,15,34,24,0,41,42,28,33,40,39,22,16,1,38,28]);

$gM->addDepUnit("pressure", [38,43,0,25,16,14,21,5,25,24,27,15,34,41,42,28,18,29,38,39,22,1,38]);
$gM->addDepUnit("divide",   [44,38,5,4,43,0,27,25,14,21,25,24,0,15,18,29,41,42,28,33,39,22,16,1,38]);
$gM->addDepUnit("fullBuild",[4,38,5,23,4,43,17,0,27,6,37,30,7,11,18,32,25,14,21,34,29,34,40,25,15,24,0,41,42,28,33,39,22,16,1,38]);
$gM->addDepUnit("d4c",      [9,20,38,0,27,18,19,32,25,14,21,6,7,37,30,11,37,30,34,29,34,40,6,25,15,24,0,42,28,33,39,41,22,16,1,38]);
$gM->addDepUnit("lens",     [20,38,0,27,18,32,25,14,21,6,7,37,30,11,37,30,34,29,34,40,6,25,15,24,0,42,28,33,39,22,16,1,38]);
$gM->addDepUnit("simple",   [38,27,0,18,32,25,14,21,7,37,30,11,29,34,40,25,15,24,0,42,28,33,39,22,16,1,29,41,34,38]);
$gM->addDepUnit("t1MarkII", [36,35,17,5,4,38,23,43,0,6,7,37,30,11,27,18,29,32,25,14,21,25,34,15,24,0,40,41,42,28,33,39,22,16,1,38,28]);
$gM->addDepUnit("ts1layer", [4,38,5,23,4,43,0,27,18,29,25,14,21,25,34,15,24,0,41,42,28,33,39,22,16,1,38,28]);
$gM->addDepUnit("t1Real",   [35,17,5,4,38,23,43,0,6,7,37,30,11,27,18,29,32,25,14,21,25,34,15,24,0,40,41,42,28,33,39,22,16,1,38,28]);
$gM->addDepUnit("reactor",  [10,38,0,27,18,32,25,14,21,6,37,30,7,11,34,29,34,40,25,15,24,0,42,28,33,39,22,41,16,1,38]);
$gM->addDepUnit("siMod",    [38,0,27,18,32,25,14,21,34,6,7,37,30,11,29,40,25,15,24,0,42,28,33,39,16,1,38]);
$gM->addDepUnit("cuBuild",  [8,10,38,0,27,18,32,25,14,21,34,6,7,37,30,22,11,29,40,41,25,15,24,0,42,28,33,39,16,1,38]);
$gM->addDepUnit("ess",      [13,38,0,27,18,32,25,14,21,34,6,7,37,30,22,11,29,40,41,25,15,24,0,42,28,33,39,16,1,38]);
$gM->addDepUnit("sns",      [31,38,0,27,18,32,25,14,21,34,6,7,37,30,22,11,29,40,41,25,15,24,0,42,28,33,39,16,1,38]);
$gM->addDepUnit("epb",      [12,10,38,0,27,18,32,25,14,21,34,6,7,37,30,22,11,29,40,41,25,15,24,0,42,28,33,39,16,1,38]);
$gM->addDepUnit("muBeam",   [26,35,17,5,4,38,23,43,0,6,7,37,30,11,27,18,29,32,25,14,21,25,34,15,24,0,40,41,42,28,33,39,22,16,1,38,28]);
$gM->addDepUnit("bnct",     [3,35,17,5,4,38,23,43,0,6,7,37,30,11,27,18,29,32,25,14,21,25,34,15,24,0,40,41,42,28,33,39,22,16,1,38,28]);

$gM->addDepUnit("testMain", [45,4,38,5,36,17,23,4,43,6,7,37,30,11,35,0,27,18,29,32,25,14,21,15,34,24,0,41,42,28,33,40,39,22,16,1,38,28]);

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
