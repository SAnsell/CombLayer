#!/usr/bin/perl -w

## use lib "/home/ansell/exe/getMk";

use getMake;
use strict;

## EXECUTABLES
my @masterprog=("fullBuild","ess","t1Real","t1MarkII","testMain"); 

my @noncompile=("bilbau","clayer","cuBuild","d4c","detectSim",
		"epb","ess","fussion","lens",
                "pressure","reactor","vcn","power");


## CXX Directory
my @sublibdir=("src","attachComp","bibBuild","build",                      # 0
	       "chip","construct","crystal","cuBlock",                     # 4
	       "d4cModel","delft","endf","epbBuild",                       # 8
               "essBuild","funcBase","geometry","global",                  # 12
	       "imat","input","instrument","lensModel",                    # 16
               "log","md5","moderator","monte",                            # 20
               "mersenne","physics","poly","process",                      # 24
               "scatMat","source","support","tally",                       # 28
               "t1Build","t1Upgrade","transport","visit",                  # 32
	       "weights","world","work","xml",                             # 36
	       "zoom","special","test");                                   # 40

my @core=qw( src attachComp construct crystal endf funcBase
             geometry global input instrument log md5 monte 
             mersenne physics poly process scatMat source 
             support tally transport visit weights
             world work xml special test);

my @coreInc=qw( include  attachCompInc constructInc crystalInc 
             endfInc funcBaseInc geomInc globalInc inputInc
             instrumentInc logInc md5Inc monteInc 
             mersenneInc physicsInc polyInc processInc scatMatInc
             sourceInc supportInc tallyInc transportInc visitInc
             weightsInc worldInc workInc xmlInc specialInc testInclude);

my @libnames=@sublibdir;

## INCLUDES
my @incdir=("include","attachCompInc","bibBuildInc","buildInc","chipInc",
	    "constructInc","crystalInc","cuBlockInc","d4cModelInc",
	    "delftInc","endfInc","epbBuildInc","essBuildInc",
            "funcBaseInc","geomInc",
	    "globalInc","imatInc","inputInc","instrumentInc",
	    "lensModelInc","logInc","md5Inc","mersenneInc",
	    "moderatorInc","monteInc","physicsInc","polyInc",
	    "processInc","scatMatInc","sourceInc","specialInc",
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
	      "","","","");

my $gM=new getMake;

$gM->addMasterProgs(\@masterprog,\@controlflags);
$gM->addCoreItems(\@core,\@coreInc);
$gM->addLibs(\@libnames,\@sublibdir,\@libflags);
$gM->addIncDir(\@incdir);

# my @libnames=("mcnpx","build","funcBase","geometry",  0,1,2,3
#	      "log","monte","mersenne","poly",          4,5,6,7,
#	      "process","support","test","lensModel");                  


$gM->addDepUnit("bilbau",   [2,9,35,0,25,17,29,23,13,20,31,5,6,34,28,21,10,27,37,38,23,14,24,0,39,26,30,36,15,1,35,3]);

$gM->addDepUnit("pressure", [35,40,0,23,15,13,20,4,23,24,25,14,31,38,39,26,17,27,35,36,21,1,35]);
$gM->addDepUnit("divide",   [41,35,4,3,40,0,25,23,13,20,23,24,0,14,17,27,38,39,26,30,36,21,15,1,35]);
$gM->addDepUnit("fullBuild",[3,35,4,22,3,40,16,0,25,5,34,28,6,10,17,29,23,13,20,31,27,31,37,23,14,24,0,38,39,26,30,36,21,15,1,35]);
$gM->addDepUnit("d4c",      [8,19,35,0,25,17,18,29,23,13,20,5,6,34,28,10,34,28,31,27,31,37,5,23,14,24,0,39,26,30,36,38,21,15,1,35]);
$gM->addDepUnit("lens",     [19,35,0,25,17,29,23,13,20,5,6,34,28,10,34,28,31,27,31,37,5,23,14,24,0,39,26,30,36,21,15,1,35]);
$gM->addDepUnit("simple",   [35,25,0,17,29,23,13,20,6,34,28,10,27,31,37,23,14,24,0,39,26,30,36,21,15,1,27,38,31,35]);
$gM->addDepUnit("t1MarkII", [33,32,16,4,3,35,22,40,0,5,6,34,28,10,25,17,27,29,23,13,20,23,31,14,24,0,37,38,39,26,30,36,21,15,1,35,26]);
$gM->addDepUnit("ts1layer", [3,35,4,22,3,40,0,25,17,27,23,13,20,23,31,14,24,0,38,39,26,30,36,21,15,1,35,26]);
$gM->addDepUnit("t1Real",   [32,16,4,3,35,22,40,0,5,6,34,28,10,25,17,27,29,23,13,20,23,31,14,24,0,37,38,39,26,30,36,21,15,1,35,26]);
$gM->addDepUnit("reactor",  [9,35,0,25,17,29,23,13,20,5,34,28,6,10,31,27,31,37,23,14,24,0,39,26,30,36,21,38,15,1,35]);
$gM->addDepUnit("siMod",    [9,35,0,25,17,29,23,13,20,31,5,6,34,28,10,27,37,23,14,24,0,39,26,30,36,15,1,35]);
$gM->addDepUnit("cuBuild",  [7,9,35,0,25,17,29,23,13,20,31,5,6,34,28,21,10,27,37,38,23,14,24,0,39,26,30,36,15,1,35]);
$gM->addDepUnit("ess",      [12,9,35,0,25,17,29,23,13,20,31,5,6,34,28,21,10,27,37,38,23,14,24,0,39,26,30,36,15,1,35]);
$gM->addDepUnit("epb",      [11,9,35,0,25,17,29,23,13,20,31,5,6,34,28,21,10,27,37,38,23,14,24,0,39,26,30,36,15,1,35]);

$gM->addDepUnit("testMain", [42,3,35,4,33,16,22,3,40,5,6,34,28,10,32,0,25,17,27,29,23,13,20,14,31,24,0,38,39,26,30,37,36,21,15,1,35,26]);

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
