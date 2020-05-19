#!/usr/bin/perl -w

## use lib "/home/ansell/exe/getMk";
use lib "./";

use CMakeList;
require "CMakeSupport.pm";
use strict;

## EXECUTABLES

my @masterProg=getAllMain();


#    ("example","fullBuild","ess","muBeam",
#		"pipe","photonMod2","t1Real",
#		"sns","saxs","reactor","essBeamline","bilbau",
#		"singleItem","maxiv","testMain");

# filter


## Model Directory
##my @modelLibDir=qw( bibBuild bnctBuild build chip
##                    cuBlock d4cModel delft epbBuild essBuild
##                    gammaBuild lensModel moderator
##                    muon pipeBuild photon sinbadBuild snsBuild t1Build
##                    zoom );

##my @modelNames= @modelLibDir;

## INCLUDES

my @incdir=qw( include beamlineInc globalInc instrumentInc
               scatMatInc specialInc transportInc testInclude );


my @mainLib=qw( visit src simMC  construct physics input process
    transport scatMat endf crystal source monte funcBase log monte
    flukaProcess flukaPhysics
    flukaTally phitsProcess
    phitsTally phitsSupport tally
    geometry mersenne src world work
    xml poly support weights
    insertUnit md5 construct
    global constructVar physics simMC
    transport attachComp visit poly
    flukaMagnetic phitsPhysics
    scatMat endf crystal );

my $gM=new CMakeList;
$gM->setParameters(\@ARGV);
$gM->addMasterProgs(\@masterProg);
$gM->addIncDir("",\@incdir);
$gM->findSubIncDir("System");
$gM->findSubIncDir("Model");
$gM->findSubIncDir("Model/ESSBeam");
$gM->findSubIncDir("Model/MaxIV");

$gM->findSubSrcDir("Model");
$gM->findSubSrcDir("System");
$gM->findSubSrcDir("Model/ESSBeam");
$gM->findSubSrcDir("Model/MaxIV");

$gM->findSubSrcDir("","Aunit");


foreach my $mainProg (@masterProg)
  {
    if ($mainProg eq "ess")
      {
	my @ess = qw( essBuild );
	my @essSupport = qw( essConstruct commonVar common
			     beer  bifrost  cspec  dream  estia
			     freia  heimdal  loki  magic  miracles
			     nmx  nnbar  odin  skadi  testBeam
			     trex  vor  vespa shortOdin shortNmx
			     shortDream simpleItem beamline instrument );

	push(@ess,@mainLib);
	$gM->addDepUnit("ess", [@ess,@essSupport]);
      }
    elsif ($mainProg eq "essBeamline")
      {
	my @essBeam = qw( essBuild );
	my @essSupport = qw( essConstruct commonVar common
			     beer  bifrost  cspec  dream  estia
			     freia  heimdal  loki  magic  miracles
			     nmx  nnbar  odin  skadi  testBeam
			     trex  vor  vespa shortOdin shortNmx
			     shortDream simpleItem beamline instrument );

	push(@essBeam,@mainLib);
	$gM->addDepUnit("essBeamline", [@essBeam,@essSupport]);
      }
    elsif ($mainProg eq "example")
      {
	my @example = qw( exampleBuild );
	push(@example,@mainLib);
	$gM->addDepUnit("example", [@example]);
      }

    elsif ($mainProg eq "maxiv")
      {
	my @maxiv = qw( maxivBuild );
	push(@maxiv,@mainLib);
	$gM->addDepUnit("maxiv", [@maxiv,
				  qw(balder cosaxs danmax R3Common
				  flexpes formax maxpeem  micromax
				  softimax
				  commonGenerator commonBeam Linac
				  R3Common R1Common species)]);
      }

    elsif ($mainProg eq "filter")
      {
	my @filter = qw( filter photon );
	push(@filter,@mainLib);
	$gM->addDepUnit("filter", [@filter]),
      }

    elsif ($mainProg eq "bilbau")
      {
	my @bilbau = qw( bibBuild );
	push(@bilbau,@mainLib);
	$gM->addDepUnit("bilbau", [@bilbau]),
      }

    elsif ($mainProg eq "fullBuild")
      {
	my @fullBuild = qw( moderator build chip  build ralBuild zoom  );
	push(@fullBuild,@mainLib);
	$gM->addDepUnit("fullBuild", [@fullBuild]),
      }

    elsif ($mainProg eq "d4c")
      {
	my @d4c = qw( d4cModel ) ;
	push(@d4c,@mainLib);
	$gM->addDepUnit("d4c", [@d4c]);
      }

    elsif ($mainProg eq "lens")
      {
	my @lens = qw( lensModel ) ;
	push(@lens,@mainLib);
	$gM->addDepUnit("lens", [@lens]);
      }

    elsif ($mainProg eq "simple")
      {
	$gM->addDepUnit("simple", [@mainLib]);
      }

    elsif ($mainProg eq "photonMod")
      {
	my @photonMod = qw( photon ) ;
	push(@photonMod,@mainLib);
	$gM->addDepUnit("photonMod", [@photonMod]);
      }

    elsif ($mainProg eq "photonMod2")
      {
	my @photonMod2 = qw( photon ) ;
	push(@photonMod2,@mainLib);
	$gM->addDepUnit("photonMod2", [@photonMod2]);
      }

    elsif ($mainProg eq "photonMod3")
      {
	my @photonMod3 = qw( photon ) ;
	push(@photonMod3,@mainLib);
	$gM->addDepUnit("photonMod3", [@photonMod3]);
      }

    elsif ($mainProg eq "pipe")
      {
	my @pipe = qw( pipeBuild ) ;
	push(@pipe,@mainLib);
	$gM->addDepUnit("pipe", [@pipe]);
      }

    elsif ($mainProg eq "singleItem")
      {
	my @singleItem = qw( singleItemBuild ) ;
	push(@singleItem,@mainLib);
	$gM->addDepUnit("singleItem",
			[@singleItem,
			 qw( commonVar commonGenerator R1Common R3Common
			     commonBeam  Linac )]);
      }


    elsif ($mainProg eq "t1Real")
      {
	my @t1Real = qw( t1Build ralBuild  ) ;
	push(@t1Real,@mainLib);
	$gM->addDepUnit("t1Real", [@t1Real]);
      }

    elsif ($mainProg eq "reactor")
      {
	my @reactor = qw( delft ) ;
	push(@reactor,@mainLib);
	$gM->addDepUnit("reactor", [@reactor]);
      }


    elsif ($mainProg eq "siMod")
      {
	$gM->addDepUnit("siMod", \@mainLib);
      }

    elsif ($mainProg eq "cuBuild")
      {
	my @cuBuild = qw( cuBlock delft ) ;
	push(@cuBuild,@mainLib);
	$gM->addDepUnit("cuBuild", [@cuBuild]);
      }

    elsif ($mainProg eq "sinbad")
      {
	my @sinbad = qw( sinbadBuild ) ;
	push(@sinbad,@mainLib);
	$gM->addDepUnit("sinbad", [@sinbad]);
      }


    elsif ($mainProg eq "sns")
      {
	my @sns = qw( snsBuild ralBuild ) ;
	push(@sns,@mainLib);
	$gM->addDepUnit("sns", [@sns]);
      }

    elsif ($mainProg eq "epb")
      {
	my @epb = qw( epbBuild ) ;
	push(@epb,@mainLib);
	$gM->addDepUnit("epb", [@epb]);
      }

    elsif ($mainProg eq "muon")
      {
	my @muBeam = qw( muon ) ;
	push(@muBeam,@mainLib);
	$gM->addDepUnit("muBeam", [@muBeam]);
      }

    elsif ($mainProg eq "gamma")
      {
	my @gamma = qw( gammaBuild ) ;
	push(@gamma,@mainLib);
	$gM->addDepUnit("gamma", [@gamma]);
      }

    elsif ($mainProg eq "saxs")
      {
	my @saxs = qw( saxs d4cModel instrument );
	push(@saxs,@mainLib);
	$gM->addDepUnit("saxs", [@saxs]),
      }

    elsif ($mainProg eq "testMain")
      {
	my @testMain = qw( test ) ;
	push(@testMain,@mainLib);
	$gM->addDepUnit("testMain", [@testMain]);
      }
    elsif ($mainProg eq "pik")
      {
	my @pik = qw( pik );
	push(@pik,@mainLib);
	$gM->addDepUnit("pik", [@pik]);
      }

    else
      {
	print STDERR "FAILURE : UNKNOWN Main file ::: ",$mainProg,"\n";
      }
   }

$gM->writeCMake();
print "FINISH CMake.pl\n";
exit 0;
