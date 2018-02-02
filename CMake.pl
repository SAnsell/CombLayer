#!/usr/bin/perl -w

## use lib "/home/ansell/exe/getMk";

use CMakeList;
use strict;

## EXECUTABLES
my @masterprog=("fullBuild","ess","muBeam","pipe","photonMod2","t1Real",
		"sns","reactor","t1MarkII","essBeamline","bilbau",
		"filter","singleItem","balder","testMain"); 



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


my @libnames=@sublibdir;

## INCLUDES

## MODEL Directory
my @modelInclude = qw( bibBuildInc bnctBuildInc buildInc chipInc 
                       cuBlockInc d4cModelInc delftInc epbBuildInc 
                       essBuildInc gammaBuildInc imatInc lensModelInc 
                       moderatorInc muonInc pipeBuildInc photonInc
                       singleItemBuildInc sinbadBuildInc snsBuildInc t1BuildInc 
                       t1EngineerInc t1UpgradeInc t3ModelInc zoomInc );


## SYSTEM Directory
my @systemInclude = qw(
                     attachCompInc constructInc compWeightsInc crystalInc 
                     endfInc funcBaseInc geomInc inputInc logInc md5Inc 
                     mersenneInc monteInc physicsInc polyInc processInc 
                     simMCInc sourceInc supportInc tallyInc 
                     visitInc weightsInc worldInc workInc xmlInc 
                     );

my @incdir=qw( include beamlineInc globalInc instrumentInc 
               scatMatInc specialInc transportInc testInclude );


my $gM=new CMakeList;
$gM->setParameters(\@ARGV);
$gM->addMasterProgs(\@masterprog);
$gM->addIncDir("",\@incdir);
$gM->findSubIncDir("System");
$gM->findSubIncDir("Model");
$gM->findSubIncDir("Model/ESSBeam");

$gM->findSubSrcDir("Model");
$gM->findSubSrcDir("System");
$gM->findSubSrcDir("Model/ESSBeam");
$gM->findSubSrcDir("");

$gM->addDepUnit("ess",      ["essBuild","beamline","support","input",
			     "funcBase","log","construct","md5",
			     "process","world","monte","geometry",
                             "mersenne","src","xml","poly",
			     "weights","global","attachComp","visit",
                             "beer","bifrost","cspec","dream","estia",
			     "freia","heimdal","loki","magic","miracles",
			     "nmx","nnbar","odin","skadi","testBeam",
			     "trex","vor","vespa","common",
			     "shortDream","shortNmx","shortOdin","longLoki",
			     "commonVar","simpleItem","physics","simMC",
			     "constructVar","essConstruct","construct",
			     "transport","scatMat","endf","crystal",
			     "insertUnit","tally","source","instrument",
			     "work"
    	 	             ]);

#$gM->addDepUnit("linac",
#		["essLinac","visit","src","simMC",
#		 "beamline","physics","support",
#		 "input","instrument","source","monte",
#		 "funcBase","log","tally","construct",
#		 "crystal","transport","scatMat","md5",
#		 "endf","process","world","work",
#		 "monte","geometry","mersenne","src","xml","poly",
#		 "weights","global","attachComp","visit"
#		]);

$gM->addDepUnit("essBeamline",
                             ["essBuild","beamline","support","input",
			     "funcBase","log","construct","md5",
			     "process","world","monte","geometry",
                             "mersenne","src","xml","poly","weights",
			      "global","attachComp","visit","essConstruct",
                             "beer","bifrost","cspec","dream","estia",
			     "freia","heimdal","loki","magic","miracles",
			     "nmx","nnbar","odin","skadi","testBeam",
			     "trex","vor","vespa","common",
			     "shortDream","shortNmx","shortOdin","longLoki",
			     "commonVar","simpleItem","physics","simMC",
			     "constructVar","essConstruct","construct",
			     "transport","scatMat","endf","crystal",
			     "insertUnit","tally","source","instrument",
			     "work"
    	 	             ]);
$gM->addDepUnit("balder", ["balder","visit","src","simMC",
			   "construct","physics","input","process",
			   "transport","scatMat","endf","crystal",
			   "source","monte","funcBase","log","monte",
			   "tally","geometry","mersenne","src","world",
			   "work","xml","poly","support","weights",
			   "insertUnit","md5","global","constructVar",
			   "physics","simMC","scatMat",
                           "transport","attachComp","visit","poly"]);


$gM->addDepUnit("filter", ["filter","photon","visit","src","simMC",
			   "construct","physics","input","process",
			   "transport","scatMat","endf","crystal",
			   "source","monte","funcBase","log","monte",
			   "tally","geometry","mersenne","src","world",
			   "work","xml","poly","support","weights",
   		           "physics","simMC","transport","scatMat",
			   "endf","crystal","work",
			   "insertUnit","md5","global","attachComp",
			   "visit","poly"]);

$gM->addDepUnit("bilbau", ["bibBuild","visit","chip","t1Upgrade",
			   "imat","moderator","build","zoom","construct",
			   "crystal","transport","scatMat","endf","t1Build",
			   "src","physics","input","process","source",
			   "monte","funcBase","log","geometry","tally",
			   "mersenne","src","work","xml","poly","support",
      		           "physics","simMC",
                           "transport","scatMat","endf","crystal",
			   "world","weights","insertUnit",
			   "md5","global","attachComp",
			   "visit","poly"]);


$gM->addDepUnit("pressure", ["visit","zoom","src","monte","global",
			     "funcBase","log","chip","monte","mersenne",
			     "physics","geometry","tally","work","xml",
			     "poly","input","process","visit","weights",
			     "insertUnit","md5","attachComp","visit"]);

$gM->addDepUnit("divide",   ["special","visit","chip","build","zoom",
			     "src","physics","monte","funcBase","log",
			     "monte","mersenne","src","geometry","input",
			     "process","work","xml","poly","support",
			     "insertUnit",
			     "weights","md5","global","attachComp","visit"]);

$gM->addDepUnit("fullBuild",["build","visit","chip","moderator","build",
			     "zoom","imat","src","simMC","physics","construct",
			     "transport","scatMat","crystal","endf","input",
			     "source","monte","funcBase","log","process",
			     "tally","world","monte","geometry","mersenne",
			     "src","work","xml","poly","support","weights",
        		      "physics","simMC","source","chip","work",
                             "transport","scatMat","endf","crystal",
			     "insertUnit","md5","global","attachComp","visit"]);

$gM->addDepUnit("d4c",      ["d4cModel","lensModel","visit","src","physics",
			     "input","instrument","source","simMC","monte",
			     "funcBase","log","construct","crystal","transport",
			     "scatMat","endf","transport","scatMat","tally",
			     "process","tally","world","construct","monte",
			     "geometry","mersenne","src","xml","poly",
			     "support","weights","work","md5","global",
			     "insertUnit","attachComp","visit"]);

$gM->addDepUnit("t3Expt",    ["t3Model","visit","src","physics",
			     "input","instrument","source","simMC","monte",
			     "funcBase","log","construct","crystal","transport",
			     "scatMat","endf","transport","scatMat","tally",
			     "process","tally","world","construct","monte",
			     "geometry","mersenne","src","xml","poly",
			     "support","weights","work","md5","global",
			     "insertUnit","attachComp","visit"]);

$gM->addDepUnit("lens",     ["lensModel","visit","src","physics","input",
			     "source","simMC","monte","funcBase","log",
			     "construct","crystal","transport","scatMat",
			     "endf","transport","scatMat","tally",
			     "process","tally","world","construct",
			     "monte","geometry","mersenne","src",
			     "xml","work","poly","support","insertUnit",
			     "weights","md5","global","attachComp","visit"]);

$gM->addDepUnit("simple",   ["visit","physics","src","input","source",
			     "monte","funcBase","log","crystal","transport",
			     "scatMat","endf","process","tally","world",
			     "monte","geometry","mersenne","src","xml",
			     "poly","support","weights","md5","global",
			     "attachComp","insertUnit",
			     "process","work","tally","visit"]);

$gM->addDepUnit("t1MarkII", ["t1Upgrade","t1Build","imat","chip","build",
			     "visit","moderator","zoom","src","simMC",
			     "construct","crystal","transport","scatMat",
			     "endf","physics","input","process","source",
			     "monte","funcBase","log","monte","tally",
			     "geometry","mersenne","src","world",
             		      "physics","simMC","source","chip","work",
                             "transport","scatMat","endf","crystal",
			     "imat","chip",
			     "xml","poly","support","weights","md5",
			     "insertUnit","global","attachComp",
			     "visit","poly"]);

$gM->addDepUnit("t1Eng", ["t1Engineer","t1Upgrade","t1Build","imat","chip",
			  "build","visit","moderator","zoom","src","simMC",
			  "construct","crystal","transport","scatMat","endf",
			  "physics","input","process","source","monte",
			  "funcBase","log","monte","tally","geometry",
			  "mersenne","src","world","work","xml","poly",
			  "support","weights","md5","global","attachComp",
			  "insertUnit","visit","poly"]);

$gM->addDepUnit("photonMod", ["photon","visit","src","simMC",
			      "construct","physics","input","process",
			      "transport","scatMat","endf","crystal",
			      "source","monte","funcBase","log","monte",
			      "tally","geometry","mersenne","src","world",
			      "work","xml","poly","support","weights",
			      "insertUnit","md5","global",
			      "attachComp","visit","poly"]);

$gM->addDepUnit("photonMod2", ["photon","visit","src","simMC",
			      "construct","physics","input","process",
			      "transport","scatMat","endf","crystal",
			      "source","monte","funcBase","log","monte",
			      "tally","geometry","mersenne","src","world",
			       "work","xml","poly","support","weights",
     		               "physics","simMC","transport","scatMat",
			       "endf","crystal",
			       "insertUnit","md5","global",
			       "attachComp","visit","poly"]);

$gM->addDepUnit("photonMod3", ["photon","visit","src","simMC",
			      "construct","physics","input","process",
			      "transport","scatMat","endf","crystal",
			      "source","monte","funcBase","log","monte",
			      "tally","geometry","mersenne","src","world",
			      "work","xml","poly","support","weights",
			      "insertUnit","md5","global",
			       "attachComp","visit","poly"]);

$gM->addDepUnit("pipe", ["pipeBuild","visit","src","simMC",
			 "construct","physics","input","process",
			 "transport","scatMat","endf","crystal",
			 "source","monte","funcBase","log","monte",
			 "tally","geometry","mersenne","src","world",
			 "attachComp","beamline","support","commonVar",
			 "work","xml","poly","support","weights",
		         "physics","simMC",
                         "transport","scatMat","endf","crystal",
			 "insertUnit","md5","global",
			 "attachComp","visit","poly","construct"]);

$gM->addDepUnit("singleItem", ["singleItemBuild","visit","src",
			       "construct","physics","input","process",
			       "transport","scatMat",
			       "source","monte","funcBase","log","monte",
			       "tally","geometry","mersenne","src","world",
			       "work","xml","poly","support","weights",
			       "physics","simMC","transport","scatMat",
			       "endf","crystal",
			       "md5","global","attachComp","visit","poly",
			       "essConstruct","insertUnit","commonVar",
		               "construct"]);

$gM->addDepUnit("ts1layer", ["build","visit","chip","moderator","build",
			     "zoom","src","physics","input","process",
			     "monte","funcBase","log","monte","tally",
			     "geometry","mersenne","src","work","xml",
			     "poly","support","weights","md5","global",
			     "insertUnit","attachComp","visit","poly"]);

$gM->addDepUnit("t1Real",   ["t1Build","build","visit",
			     "moderator","zoom","src","simMC","construct",
			     "crystal","transport","scatMat","endf",
			     "physics","input","process","source","monte",
			     "funcBase","log","monte","tally","geometry",
			     "mersenne","src","world","work","xml",
     			     "physics","simMC","transport","scatMat",
			     "source","tally","work","crystal","chip",
			     "imat","endf",
			     "poly","support","weights","md5","global",
			     "insertUnit","attachComp","visit","poly"]);

$gM->addDepUnit("reactor",  ["delft","visit","src","simMC","physics",
			     "input","source","monte","funcBase","log",
			     "construct","transport","scatMat","crystal",
			     "endf","process","tally","world","monte",
			     "geometry","mersenne","src","physics",
			     "simMC","transport","scatMat","endf",
			     "crystal","source","xml","poly","support",
			     "weights","md5","work","insertUnit","global",
			     "attachComp","visit"]);

$gM->addDepUnit("siMod",    ["visit","src","physics","input","source","monte",
			     "funcBase","log","tally","construct","crystal",
			     "transport","scatMat","endf","process","world",
			     "monte","geometry","mersenne","src","xml",
			     "poly","support","weights","global","attachComp",
			     "insertUnit","visit"]);

$gM->addDepUnit("cuBuild",  ["cuBlock","delft","visit","src","physics",
			     "input","source","monte","funcBase","log",
			     "tally","construct","crystal","transport",
			     "scatMat","md5","endf","process","world",
			     "work","monte","geometry","mersenne","src",
			     "xml","poly","support","weights","global",
			     "insertUnit","attachComp","visit"]);

$gM->addDepUnit("sinbad",   ["sinbadBuild","visit","src","simMC",
			     "physics","input",
			     "source","monte","funcBase","log","tally",
			     "construct","crystal","transport","scatMat",
			     "md5","endf","process","world","work","monte",
			     "geometry","mersenne","src","xml","poly",
			     "support","weights","global","attachComp",
			     "insertUnit","visit"]);

$gM->addDepUnit("sns",      ["snsBuild","visit","src","input",
			     "monte","funcBase","log",
			     "construct","transport","scatMat",
			     "md5","process","world","monte",
			     "geometry","mersenne","src","xml","poly",
  			     "support","weights","global","attachComp",
			     "physics","simMC","transport","scatMat",
			     "source","tally","work",
			      "endf","crystal","insertUnit","visit"]);

$gM->addDepUnit("epb",      ["epbBuild","visit","src","physics",
			     "simMC","input","source","monte","funcBase","log",
			     "tally","construct","crystal","transport",
			     "scatMat","md5","endf","process","world",
			     "work","monte","geometry","mersenne","src",
			     "xml","poly","support","weights","global",
			     "insertUnit","attachComp","visit"]);


$gM->addDepUnit("muBeam",   ["muon","visit","src","physics",
			     "simMC","input","source","monte","funcBase","log",
			     "tally","construct","transport",
			     "scatMat","md5","process","world",
			     "work","monte","geometry","mersenne","src",
     			     "physics","simMC","transport","scatMat",
			     "source","tally","work","endf","crystal",
			     "xml","poly","support","weights","global",
			     "insertUnit","attachComp","visit"]);


$gM->addDepUnit("bnct",     ["bnctBuild","t1Build","imat","chip","build",
			     "visit","moderator","zoom","src","construct",
			     "crystal","transport","scatMat","endf","physics",
			     "input","process","source","monte","funcBase",
			     "log","monte","tally","geometry","mersenne",
			     "src","world","work","xml","poly","support",
			     "weights","md5","global","attachComp","visit",
			     "insertUnit","poly"]);

$gM->addDepUnit("gamma",    ["gammaBuild","t1Build","imat","chip","build",
			     "visit","moderator","zoom","src","construct",
			     "crystal","transport","scatMat","endf",
			     "physics","input","process","source","monte",
			     "funcBase","log","monte","tally","geometry",
			     "mersenne","src","world","work","xml","poly",
			     "support","weights","md5","global","attachComp",
			     "insertUnit","visit","poly"]);

$gM->addDepUnit("testMain", ["test","build","visit","chip","t1Upgrade",
			     "build","zoom","construct",
			     "crystal","transport","t1Build",
			     "src","simMC","physics","input","process","source",
			     "monte","funcBase","log","geometry","tally",
			     "build","imat","moderator","chip","zoom",
                             "simMC","transport","scatMat","crystal","endf",
			     "mersenne","src","work","xml","poly","support",
			     "world","weights","md5","global","attachComp",
			     "insertUnit","visit","poly","essConstruct"]);

$gM->writeCMake();

print "FINISH CMake.pl\n";
exit 0;

