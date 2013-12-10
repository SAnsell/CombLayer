#!/usr/bin/perl -w

## use lib "/home/ansell/exe/getMk";

use getMake;
use strict;

## EXECUTABLES
my @masterprog=("fullBuild","ess","sns","t1Real","t1MarkII","testMain"); 

my @noncompile=("bilbau","clayer","cuBuild","d4c","detectSim",
		"epb","ess","fussion","lens",
                "pressure","reactor","vcn","power");


## CXX Directory
my @sublibdir=("src","attachComp","bibBuild","build",                    # 0
	       "chip","construct","crystal","cuBlock",                   # 4
	       "d4cModel","delft","endf","epbBuild",                     # 8
               "essBuild","funcBase","geometry","global",                # 12
	       "imat","input","instrument","lensModel",                  # 16
               "log","md5","moderator","mersenne",                       # 20 
	       "monte","muon","physics","poly",                          # 24
	       "process","scatMat","snsBuild","source",                  # 28
	       "support", "tally","t1Build","t1Upgrade",
	       "transport","visit","weights","world",
	       "work","xml","zoom","special",
	       "test");                            

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
my @incdir=("include","attachCompInc","bibBuildInc","buildInc","chipInc",
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
	      "","","","","");

my $gM=new getMake;

$gM->addMasterProgs(\@masterprog,\@controlflags);
$gM->addCoreItems(\@core,\@coreInc);
$gM->addLibs(\@libnames,\@sublibdir,\@libflags);
$gM->addIncDir(\@incdir);

# my @libnames=("mcnpx","build","funcBase","geometry",  0,1,2,3
#	      "log","monte","mersenne","poly",          4,5,6,7,
#	      "process","support","test","lensModel");                  


$gM->addDepUnit("bilbau",   [2,9,37,0,26,17,31,24,13,20,33,5,6,36,29,21,10,28,39,40,24,14,23,0,41,27,32,38,15,1,37,3]);

$gM->addDepUnit("pressure", [37,42,0,24,15,13,20,4,24,23,26,14,33,40,41,27,17,28,37,38,21,1,37]);
$gM->addDepUnit("divide",   [43,37,4,3,42,0,26,24,13,20,24,23,0,14,17,28,40,41,27,32,38,21,15,1,37]);
$gM->addDepUnit("fullBuild",[3,37,4,22,3,42,16,0,26,5,36,29,6,10,17,31,24,13,20,33,28,33,39,24,14,23,0,40,41,27,32,38,21,15,1,37]);
$gM->addDepUnit("d4c",      [8,19,37,0,26,17,18,31,24,13,20,5,6,36,29,10,36,29,33,28,33,39,5,24,14,23,0,41,27,32,38,40,21,15,1,37]);
$gM->addDepUnit("lens",     [19,37,0,26,17,31,24,13,20,5,6,36,29,10,36,29,33,28,33,39,5,24,14,23,0,41,27,32,38,21,15,1,37]);
$gM->addDepUnit("simple",   [37,26,0,17,31,24,13,20,6,36,29,10,28,33,39,24,14,23,0,41,27,32,38,21,15,1,28,40,33,37]);
$gM->addDepUnit("t1MarkII", [35,34,16,4,3,37,22,42,0,5,6,36,29,10,26,17,28,31,24,13,20,24,33,14,23,0,39,40,41,27,32,38,21,15,1,37,27]);
$gM->addDepUnit("ts1layer", [3,37,4,22,3,42,0,26,17,28,24,13,20,24,33,14,23,0,40,41,27,32,38,21,15,1,37,27]);
$gM->addDepUnit("t1Real",   [34,16,4,3,37,22,42,0,5,6,36,29,10,26,17,28,31,24,13,20,24,33,14,23,0,39,40,41,27,32,38,21,15,1,37,27]);
$gM->addDepUnit("reactor",  [9,37,0,26,17,31,24,13,20,5,36,29,6,10,33,28,33,39,24,14,23,0,41,27,32,38,21,40,15,1,37]);
$gM->addDepUnit("siMod",    [9,37,0,26,17,31,24,13,20,33,5,6,36,29,10,28,39,24,14,23,0,41,27,32,38,15,1,37]);
$gM->addDepUnit("cuBuild",  [7,9,37,0,26,17,31,24,13,20,33,5,6,36,29,21,10,28,39,40,24,14,23,0,41,27,32,38,15,1,37]);
$gM->addDepUnit("ess",      [12,9,37,0,26,17,31,24,13,20,33,5,6,36,29,21,10,28,39,40,24,14,23,0,41,27,32,38,15,1,37]);
$gM->addDepUnit("sns",      [30,9,37,0,26,17,31,24,13,20,33,5,6,36,29,21,10,28,39,40,24,14,23,0,41,27,32,38,15,1,37]);
$gM->addDepUnit("epb",      [11,9,37,0,26,17,31,24,13,20,33,5,6,36,29,21,10,28,39,40,24,14,23,0,41,27,32,38,15,1,37]);
$gM->addDepUnit("muBeam",   [25,9,36,0,26,17,30,23,13,20,32,5,6,35,29,21,10,28,38,39,23,14,24,0,40,27,31,37,15,1,36]);

$gM->addDepUnit("testMain", [44,3,37,4,35,16,22,3,42,5,6,36,29,10,34,0,26,17,28,31,24,13,20,14,33,23,0,40,41,27,32,39,38,21,15,1,37,27]);

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
