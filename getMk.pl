#!/usr/bin/perl -w

## use lib "/home/ansell/exe/getMk";

use getMake;
use strict;

## EXECUTABLES
my @masterprog=("fullBuild","ess","photonMod","sinbad","t1Real",
		"sns","reactor","t1MarkII","t1Eng","testMain"); 


my @noncompile=("bilbau","clayer","cuBuild","d4c","detectSim",
		"epb","ess","fussion","lens","muBeam",
                "pressure","vcn","power");


## CXX Directory
my @sublibdir=("src","attachComp","beamline","bibBuild",
               "bnctBuild","build","chip","construct", 
	       "crystal","cuBlock","d4cModel","delft",         
	       "endf","epbBuild","essBuild","funcBase",           # 12
	       "gammaBuild","geometry","global","imat",
	       "input","instrument","lensModel","log",
	       "md5","moderator","mersenne","monte",
	       "muon","photon","physics","poly","process",
	       "scatMat","simMC","sinbadBuild","snsBuild","source",
	       "support","tally","t1Build","t1Engineer",
               "t1Upgrade","transport","visit","weights",
	       "world","work","xml","zoom","special",
	       "test");                            

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
            "funcBaseInc","gammaBuildInc","geomInc","globalInc",
	    "imatInc","inputInc","instrumentInc","lensModelInc",
	    "logInc","md5Inc","mersenneInc","moderatorInc",
	    "monteInc","muonInc","photonInc","physicsInc","polyInc",
	    "processInc","scatMatInc","simMCInc","sinbadBuildInc",
	    "snsBuildInc","sourceInc","specialInc","supportInc","tallyInc",
	    "t1BuildInc","t1EngineerInc","t1UpgradeInc","transportInc",
	    "visitInc","weightsInc","workInc","worldInc","xmlInc",
	    "zoomInc","testInclude");   ## Includes

## Flags on executables
my @controlflags=("-S -B","-S -B","-S -B","-S -B",
		  "-S -B","-S -B","-S -B","-S -B",
		  "-S -B","-S -B","-S -B","-S -B",
                  "-S -B","-S -B");

## Lib flags
my @libflags=("","","","","",
	      "","","","","","",
	      "","","","","-S","",
	      "","","","",
	      "","","","",
	      "","","","",
	      "","","","","","","","");

my $gM=new getMake;

$gM->addMasterProgs(\@masterprog,\@controlflags);
$gM->addCoreItems(\@core,\@coreInc);
$gM->addLibs(\@libnames,\@sublibdir,\@libflags);
$gM->addIncDir(\@incdir);

# my @libnames=("mcnpx","build","funcBase","geometry",  0,1,2,3
#	      "log","monte","mersenne","poly",          4,5,6,7,
#	      "process","support","test","lensModel");                  


# $gM->addDepUnit("bilbau",   ["bibBuild","delft","visit","src","physics","input","source","monte","funcBase","log","tally","construct","crystal","transport","scatMat","md5","endf","process","world","work","monte","geometry","mersenne","src","xml","poly","support","weights","global","attachComp","visit","build"]);


$gM->addDepUnit("bilbau", ["bibBuild","build","visit","chip","t1Upgrade",
			   "imat","moderator","build","zoom","construct",
			   "crystal","transport","scatMat","endf","t1Build",
			   "src","physics","input","process","source",
			   "monte","funcBase","log","geometry","tally",
			   "mersenne","src","work","xml","poly","support",
			   "world","weights","md5","global","attachComp",
			   "visit","poly"]);

$gM->addDepUnit("pressure", ["visit","zoom","src","monte","global",
			     "funcBase","log","chip","monte","mersenne",
			     "physics","geometry","tally","work","xml",
			     "poly","input","process","visit","weights",
			     "md5","attachComp","visit"]);

$gM->addDepUnit("divide",   ["special","visit","chip","build","zoom",
			     "src","physics","monte","funcBase","log",
			     "monte","mersenne","src","geometry","input",
			     "process","work","xml","poly","support",
			     "weights","md5","global","attachComp","visit"]);

$gM->addDepUnit("fullBuild",["build","visit","chip","moderator","build",
			     "zoom","imat","src","simMC","physics","construct",
			     "transport","scatMat","crystal","endf","input",
			     "source","monte","funcBase","log","process",
			     "tally","world","monte","geometry","mersenne",
			     "src","work","xml","poly","support","weights",
			     "md5","global","attachComp","visit"]);

$gM->addDepUnit("d4c",      ["d4cModel","lensModel","visit","src","physics",
			     "input","instrument","source","simMC","monte",
			     "funcBase","log","construct","crystal","transport",
			     "scatMat","endf","transport","scatMat","tally",
			     "process","tally","world","construct","monte",
			     "geometry","mersenne","src","xml","poly",
			     "support","weights","work","md5","global",
			     "attachComp","visit"]);

$gM->addDepUnit("lens",     ["lensModel","visit","src","physics","input",
			     "source","monte","funcBase","log","construct",
			     "crystal","transport","scatMat","endf",
			     "transport","scatMat","tally","process","tally",
			     "world","construct","monte","geometry",
			     "mersenne","src","xml","work","poly","support",
			     "weights","md5","global","attachComp","visit"]);

$gM->addDepUnit("simple",   ["visit","physics","src","input","source",
			     "monte","funcBase","log","crystal","transport",
			     "scatMat","endf","process","tally","world",
			     "monte","geometry","mersenne","src","xml",
			     "poly","support","weights","md5","global",
			     "attachComp","process","work","tally","visit"]);

$gM->addDepUnit("t1MarkII", ["t1Upgrade","t1Build","imat","chip","build",
			     "visit","moderator","zoom","src","simMC",
			     "construct","crystal","transport","scatMat",
			     "endf","physics","input","process","source",
			     "monte","funcBase","log","monte","tally",
			     "geometry","mersenne","src","world","work",
			     "xml","poly","support","weights","md5",
			     "global","attachComp","visit","poly"]);

$gM->addDepUnit("t1Eng", ["t1Engineer","t1Upgrade","t1Build","imat","chip",
			  "build","visit","moderator","zoom","src","simMC",
			  "construct","crystal","transport","scatMat","endf",
			  "physics","input","process","source","monte",
			  "funcBase","log","monte","tally","geometry",
			  "mersenne","src","world","work","xml","poly",
			  "support","weights","md5","global","attachComp",
			  "visit","poly"]);

$gM->addDepUnit("photonMod", ["photon","visit","src","simMC",
			      "construct","physics","input","process",
			      "transport","scatMat","endf","crystal",
			      "source","monte","funcBase","log","monte",
			      "tally","geometry","mersenne","src","world",
			      "work","xml","poly","support","weights",
			      "md5","global","attachComp","visit","poly"]);

$gM->addDepUnit("ts1layer", ["build","visit","chip","moderator","build",
			     "zoom","src","physics","input","process",
			     "monte","funcBase","log","monte","tally",
			     "geometry","mersenne","src","work","xml",
			     "poly","support","weights","md5","global",
			     "attachComp","visit","poly"]);

$gM->addDepUnit("t1Real",   ["t1Build","imat","chip","build","visit",
			     "moderator","zoom","src","simMC","construct",
			     "crystal","transport","scatMat","endf",
			     "physics","input","process","source","monte",
			     "funcBase","log","monte","tally","geometry",
			     "mersenne","src","world","work","xml",
			     "poly","support","weights","md5","global",
			     "attachComp","visit","poly"]);

$gM->addDepUnit("reactor",  ["delft","visit","src","simMC","physics",
			     "input","source","monte","funcBase","log",
			     "construct","transport",
			     "scatMat","crystal","endf","process","tally",
			     "world","monte","geometry","mersenne","src",
			     "xml","poly","support","weights","md5","work",
			     "global","attachComp","visit"]);

$gM->addDepUnit("siMod",    ["visit","src","physics","input","source","monte",
			     "funcBase","log","tally","construct","crystal",
			     "transport","scatMat","endf","process","world",
			     "monte","geometry","mersenne","src","xml",
			     "poly","support","weights","global","attachComp",
			     "visit"]);

$gM->addDepUnit("cuBuild",  ["cuBlock","delft","visit","src","physics",
			     "input","source","monte","funcBase","log",
			     "tally","construct","crystal","transport",
			     "scatMat","md5","endf","process","world",
			     "work","monte","geometry","mersenne","src",
			     "xml","poly","support","weights","global",
			     "attachComp","visit"]);

$gM->addDepUnit("ess",      ["essBuild","visit","src","simMC","beamline","physics",
			     "input","source","monte","funcBase","log",
			     "tally","construct","crystal","transport",
			     "scatMat","md5","endf","process","world","work",
			     "monte","geometry","mersenne","src","xml","poly",
			     "support","weights","global","attachComp",
			     "visit"]);

$gM->addDepUnit("sinbad",   ["sinbadBuild","visit","src","simMC",
			     "physics","input",
			     "source","monte","funcBase","log","tally",
			     "construct","crystal","transport","scatMat",
			     "md5","endf","process","world","work","monte",
			     "geometry","mersenne","src","xml","poly",
			     "support","weights","global","attachComp",
			     "visit"]);

$gM->addDepUnit("sns",      ["snsBuild","visit","src","simMC","physics","input",
			     "source","monte","funcBase","log","tally",
			     "construct","crystal","transport","scatMat",
			     "md5","endf","process","world","work","monte",
			     "geometry","mersenne","src","xml","poly",
			     "support","weights","global","attachComp",
			     "visit"]);

$gM->addDepUnit("epb",      ["epbBuild","delft","visit","src","physics",
			     "simMC","input","source","monte","funcBase","log",
			     "tally","construct","crystal","transport",
			     "scatMat","md5","endf","process","world",
			     "work","monte","geometry","mersenne","src",
			     "xml","poly","support","weights","global",
			     "attachComp","visit"]);

$gM->addDepUnit("muBeam",   ["muon","t1Build","imat","chip","build","visit",
			     "moderator","zoom","src","construct","crystal",
			     "transport","scatMat","endf","physics","input",
			     "process","source","monte","funcBase","log",
			     "monte","tally","geometry","mersenne","src",
			     "world","work","xml","poly","support","weights",
			     "md5","global","attachComp","visit","poly"]);

$gM->addDepUnit("bnct",     ["bnctBuild","t1Build","imat","chip","build",
			     "visit","moderator","zoom","src","construct",
			     "crystal","transport","scatMat","endf","physics",
			     "input","process","source","monte","funcBase",
			     "log","monte","tally","geometry","mersenne",
			     "src","world","work","xml","poly","support",
			     "weights","md5","global","attachComp","visit",
			     "poly"]);

$gM->addDepUnit("gamma",    ["gammaBuild","t1Build","imat","chip","build",
			     "visit","moderator","zoom","src","construct",
			     "crystal","transport","scatMat","endf",
			     "physics","input","process","source","monte",
			     "funcBase","log","monte","tally","geometry",
			     "mersenne","src","world","work","xml","poly",
			     "support","weights","md5","global","attachComp",
			     "visit","poly"]);

$gM->addDepUnit("testMain", ["test","build","visit","chip","t1Upgrade",
			     "imat","moderator","build","zoom","construct",
			     "crystal","transport","scatMat","endf","t1Build",
			     "src","simMC","physics","input","process","source",
			     "monte","funcBase","log","geometry","tally",
			     "mersenne","src","work","xml","poly","support",
			     "world","weights","md5","global","attachComp",
			     "visit","poly"]);

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
    print STDERR "   -NR :: No Regex \n";
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
