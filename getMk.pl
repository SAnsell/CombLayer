#!/usr/bin/perl -w

## use lib "/home/ansell/exe/getMk";

use getMake;
use strict;

## EXECUTABLES
my @masterprog=("fullBuild","ess","pipe","sinbad","t1Real",
		"sns","reactor","t1MarkII","t1Eng","t3Expt",
		"testMain"); 


my @noncompile=("bilbau","clayer","cuBuild","d4c","detectSim",
		"epb","ess","photonMod","fussion","lens","muBeam",
                "pressure","vcn","power");

## SYSTEM Directory
my @systemLibDir=qw( attachComp construct compWeights crystal 
                     endf funcBase geometry input log md5 
                     mersenne monte physics poly process 
                     simMC source support tally  visit weights 
                     world work xml 
                     );

my @systemNames= @systemLibDir;

## Model Directory
my @modelLibDir=qw( bibBuild bnctBuild build chip 
                    cuBlock d4cModel delft epbBuild essBuild
                    gammaBuild imat lensModel moderator 
                    muon pipeBuild photon sinbadBuild snsBuild t1Build 
                    t1Engineer t1Upgrade t3Model zoom );

my @modelNames= @modelLibDir;


## GENERAL Directory 
my @sublibdir=("src","beamline","global",
	       "instrument","scatMat","transport","special",
	       "test");                            

my @core=qw( src attachComp beamline compWeights
             construct crystal endf funcBase
             geometry global input instrument log md5 monte 
             mersenne physics poly process scatMat source 
             support tally transport visit weights
             world work xml special test);

my @coreInc=qw( include  attachCompInc beamlineInc 
             compWeightsInc constructInc crystalInc 
             endfInc funcBaseInc geomInc globalInc inputInc
             instrumentInc logInc md5Inc  mersenneInc
             monteInc muonInc physicsInc polyInc processInc scatMatInc
             sourceInc supportInc tallyInc transportInc visitInc
             weightsInc worldInc workInc xmlInc specialInc testInclude);

my @libnames=@sublibdir;

## INCLUDES

## MODEL Directory
my @modelInclude = qw( bibBuildInc bnctBuildInc buildInc chipInc 
                       cuBlockInc d4cModelInc delftInc epbBuildInc 
                       essBuildInc gammaBuildInc imatInc lensModelInc 
                       moderatorInc muonInc pipeBuildInc photonInc
                       sinbadBuildInc snsBuildInc t1BuildInc t1EngineerInc 
                       t1UpgradeInc t3ModelInc zoomInc );


## SYSTEM Directory
my @systemInclude = qw(
                     attachCompInc constructInc compWeightsInc crystalInc 
                     endfInc funcBaseInc geomInc inputInc logInc md5Inc 
                     mersenneInc monteInc physicsInc polyInc processInc 
                     simMCInc sourceInc supportInc tallyInc 
                     visitInc weightsInc 
                     worldInc workInc xmlInc 
                     );

my @incdir=qw( include beamlineInc  chipInc  globalInc  
               instrumentInc polyInc  scatMatInc  
               specialInc  transportInc  worldInc testInclude );

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

$gM->addSubDirLibs("Model/ESSBeam");
$gM->addDirLibs("Model",\@modelNames,\@modelLibDir,\@libflags);
$gM->addDirLibs("System",\@systemNames,\@systemLibDir,\@libflags);

$gM->addIncDir(\@incdir);
$gM->addIncSubDir("System",\@systemInclude);
$gM->addIncSubDir("Model",\@modelInclude);
$gM->addSubIncDir("Model/ESSBeam");


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

$gM->addDepUnit("t3Expt",    ["t3Model","visit","src","physics",
			     "input","instrument","source","simMC","monte",
			     "funcBase","log","construct","crystal","transport",
			     "scatMat","endf","transport","scatMat","tally",
			     "process","tally","world","construct","monte",
			     "geometry","mersenne","src","xml","poly",
			     "support","weights","work","md5","global",
			     "attachComp","visit"]);

$gM->addDepUnit("lens",     ["lensModel","visit","src","physics","input",
			     "source","simMC","monte","funcBase","log",
			     "construct","crystal","transport","scatMat",
			     "endf","transport","scatMat","tally",
			     "process","tally","world","construct",
			     "monte","geometry","mersenne","src",
			     "xml","work","poly","support",
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

$gM->addDepUnit("pipe", ["pipeBuild","visit","src","simMC",
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

$gM->addDepUnit("ess",      ["essBuild","visit","src","simMC",
			     "beamline","physics","support",
			     "input","instrument","source","monte","funcBase","log",
			     "tally","construct","crystal","transport",
			     "scatMat","md5","endf","process","world","work",
			     "monte","geometry","mersenne","src","xml","poly",
			     "weights","global","attachComp",
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
