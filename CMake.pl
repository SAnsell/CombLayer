#!/usr/bin/perl -w

## use lib "/home/ansell/exe/getMk";
use lib "./";

use CMakeList;
use strict;

## EXECUTABLES
my @masterprog=("saxs","testMain");




## Model Directory
##my @modelLibDir=qw( bibBuild bnctBuild build chip 
##                    cuBlock d4cModel delft epbBuild essBuild
##                    gammaBuild imat lensModel moderator 
##                    muon pipeBuild photon sinbadBuild snsBuild t1Build 
##                    t1Engineer t1Upgrade t3Model zoom );

##my @modelNames= @modelLibDir;

## INCLUDES

## MODEL Directory
my @modelInclude = qw( saxsInc );



## SYSTEM Directory
my @systemInclude = qw(
                     attachCompInc constructInc compWeightsInc crystalInc 
                     endfInc flukaProcessInc flukaPhysicsInc
                     flukaTallyInc funcBaseInc geomInc 
                     inputInc logInc md5Inc 
                     mersenneInc monteInc 
                     phitsProcessInc phitsPhysicsInc
                     physicsInc polyInc processInc 
                     simMCInc sourceInc supportInc tallyInc 
                     visitInc weightsInc worldInc workInc xmlInc 
                     );

my @incdir=qw( include beamlineInc globalInc instrumentInc 
               scatMatInc specialInc transportInc testInclude );


my @mainLib=qw( visit src simMC  construct physics input process 
    transport scatMat endf crystal  source monte funcBase log monte 
    flukaMagnetic flukaProcess flukaPhysics 
    flukaTally phitsProcess phitsPhysics 
    phitsTally phitsSupport tally 
    geometry mersenne src world work 
    xml poly support weights 
    insertUnit md5 construct 
    global constructVar physics simMC 
    scatMat endf crystal transport 
    attachComp visit poly);

my $gM=new CMakeList;
$gM->setParameters(\@ARGV);
$gM->addMasterProgs(\@masterprog);
$gM->addIncDir("",\@incdir);
$gM->findSubIncDir("System");
$gM->findSubIncDir("Model");

$gM->findSubSrcDir("Model");
$gM->findSubSrcDir("System");
$gM->findSubSrcDir("","Aunit");

my @saxs = qw( saxs instrument ) ;
push(@saxs,@mainLib);
$gM->addDepUnit("saxs", [@saxs]);

my @testMain = qw( test ) ;
push(@testMain,@mainLib);
$gM->addDepUnit("testMain", [@testMain]);


$gM->writeCMake();

print "FINISH CMake.pl\n";
exit 0;

