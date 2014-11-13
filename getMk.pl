#!/usr/bin/perl -w

## use lib "/home/ansell/exe/getMk";

use getMake;
use strict;

## EXECUTABLES
my @masterprog=("fullBuild","ess","muBeam","sinbad","t1Real",
		"d4c","reactor","t1MarkII","testMain"); 


my @noncompile=("bilbau","clayer","cuBuild","d4c","detectSim",
		"epb","ess","fussion","lens",
                "pressure","vcn","power");


## CXX Directory
my @sublibdir=("src","attachComp","beamline","bibBuild",
               "bnctBuild","build","chip","construct",
	       "crystal","cuBlock","d4cModel","delft",
	       "endf","epbBuild","essBuild","funcBase",
	       "geometry","global","imat","input",
	       "instrument","lensModel","log","md5",
	       "moderator","mersenne","monte","muon",
	       "physics","poly","process","scatMat",
	       "sinbadBuild","snsBuild","source","support",
	       "tally","t1Build","t1Upgrade", "transport",
	       "visit","weights","world","work",
	       "xml","zoom","special","test");                            

my @core=qw( src attachComp beamline construct crystal endf funcBase
             geometry global input instrument log md5 monte 
             mersenne physics poly process scatMat source 
             support tally transport visit weights
             world work xml special test);

my @coreInc=qw( include  attachCompInc beamlineInc constructInc crystalInc 
             endfInc funcBaseInc geomInc globalInc inputInc
             instrumentInc logInc md5Inc  mersenneInc
             monteInc muonInc physicsInc polyInc processInc scatMatInc
             sourceInc supportInc tallyInc transportInc visitInc
             weightsInc worldInc workInc xmlInc specialInc testInclude);

my @libnames=@sublibdir;

## INCLUDES
my @incdir=("include","attachCompInc","beamlineInc","bibBuildInc",
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


# $gM->addDepUnit("bilbau",   [3,11,40,0,28,19,34,26,15,22,36,7,8,39,31,23,12,30,42,43,26,16,25,0,44,29,35,41,17,1,40,5]);


$gM->addDepUnit("bilbau", [3,5,40,6,38,18,24,5,45,7,8,39,31,12,37,0,28,19,30,34,26,15,22,16,36,25,0,43,44,29,35,42,41,23,17,1,40,29]);

$gM->addDepUnit("pressure", [40,45,0,26,17,15,22,6,26,25,28,16,36,43,44,29,19,30,40,41,23,1,40]);
$gM->addDepUnit("divide",   [46,40,6,5,45,0,28,26,15,22,26,25,0,16,19,30,43,44,29,35,41,23,17,1,40]);
$gM->addDepUnit("fullBuild",[5,40,6,24,5,45,18,0,28,7,39,31,8,12,19,34,26,15,22,30,36,42,26,16,25,0,43,44,29,35,41,23,17,1,40]);
$gM->addDepUnit("d4c",      [10,21,40,0,28,19,20,34,26,15,22,7,8,39,31,12,39,31,36,30,36,42,7,26,16,25,0,44,29,35,41,43,23,17,1,40]);
$gM->addDepUnit("lens",     [21,40,0,28,19,34,26,15,22,7,8,39,31,12,39,31,36,30,36,42,7,26,16,25,0,44,29,35,41,23,17,1,40]);
$gM->addDepUnit("simple",   [40,28,0,19,34,26,15,22,8,39,31,12,30,36,42,26,16,25,0,44,29,35,41,23,17,1,30,43,36,40]);
$gM->addDepUnit("t1MarkII", [38,37,18,6,5,40,24,45,0,7,8,39,31,12,28,19,30,34,26,15,22,26,36,16,25,0,42,43,44,29,35,41,23,17,1,40,29]);
$gM->addDepUnit("ts1layer", [5,40,6,24,5,45,0,28,19,30,26,15,22,26,36,16,25,0,43,44,29,35,41,23,17,1,40,29]);
$gM->addDepUnit("t1Real",   [37,18,6,5,40,24,45,0,7,8,39,31,12,28,19,30,34,26,15,22,26,36,16,25,0,42,43,44,29,35,41,23,17,1,40,29]);
$gM->addDepUnit("reactor",  [11,40,0,28,19,34,26,15,22,7,39,31,8,12,30,36,42,26,16,25,0,44,29,35,41,23,43,17,1,40]);
$gM->addDepUnit("siMod",    [40,0,28,19,34,26,15,22,36,7,8,39,31,12,30,42,26,16,25,0,44,29,35,41,17,1,40]);
$gM->addDepUnit("cuBuild",  [9,11,40,0,28,19,34,26,15,22,36,7,8,39,31,23,12,30,42,43,26,16,25,0,44,29,35,41,17,1,40]);
$gM->addDepUnit("ess",      [14,40,0,2,28,19,34,26,15,22,36,7,8,39,31,23,12,30,42,43,26,16,25,0,44,29,35,41,17,1,40]);
$gM->addDepUnit("sinbad",   [32,40,0,28,19,34,26,15,22,36,7,8,39,31,23,12,30,42,43,26,16,25,0,44,29,35,41,17,1,40]);
$gM->addDepUnit("sns",      [33,40,0,28,19,34,26,15,22,36,7,8,39,31,23,12,30,42,43,26,16,25,0,44,29,35,41,17,1,40]);
$gM->addDepUnit("epb",      [13,11,40,0,28,19,34,26,15,22,36,7,8,39,31,23,12,30,42,43,26,16,25,0,44,29,35,41,17,1,40]);
$gM->addDepUnit("muBeam",   [27,37,18,6,5,40,24,45,0,7,8,39,31,12,28,19,30,34,26,15,22,26,36,16,25,0,42,43,44,29,35,41,23,17,1,40,29]);
$gM->addDepUnit("bnct",     [4,37,18,6,5,40,24,45,0,7,8,39,31,12,28,19,30,34,26,15,22,26,36,16,25,0,42,43,44,29,35,41,23,17,1,40,29]);

$gM->addDepUnit("testMain", [47,5,40,6,38,18,24,5,45,7,8,39,31,12,37,0,28,19,30,34,26,15,22,16,36,25,0,43,44,29,35,42,41,23,17,1,40,29]);

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
