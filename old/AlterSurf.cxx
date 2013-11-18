/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/AlterSurf.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "Exception.h"
#include "MapSupport.h"
#include "Triple.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "AlterSurf.h"

AlterSurf::AlterSurf(FuncDataBase& A) :
  ConVar(A)
  /*!
    Constructor. Requires that the FuncDataBase object
    has been fully populated.
    \param A :: Reference to FuncDataBase object 
  */
{
  setMap();
  setMapZero();
}

AlterSurf::AlterSurf(const AlterSurf& A) :
  ConVar(A.ConVar),AMap(A.AMap)
  /*!
    Basic copy constructor
    \param A :: AlterSurf object to copy
  */
{}

AlterSurf&
AlterSurf::operator=(const AlterSurf& A) 
  /*!
    Assignment operator 
    \param A :: AlterSurf object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      AMap=A.AMap;
    }
  return *this;
}

AlterSurf::~AlterSurf()
  /*!
    Destructor
  */
{}

void
AlterSurf::setChange(const int A,const int B)
  /*!
    Given a new surf->surf global mapping
    this function adds to the AMap.
    It also updates the remainder of the map 
    so that any X->A become X->B and a
    B->Y which then make A->Y not A->B
    \param A :: Surface to change
    \param B :: Surface to be modified to (signed)
  */
{
  if (A<=0)
    throw ColErr::RangeError<int>(A,1,99999,"AlterSurf::setChange");
  if (A==B || A==-B)
    throw ColErr::MisMatch<int>(A,B,"AlterSurf::setChange");
  
  std::map<int,int>::iterator vc;
  const int signB((B>0) ? 1 : -1);
  const int outPlane(B*signB);   // +ve only [search positive ]

  if (outPlane)
    {
      vc=AMap.find(outPlane);
      if (vc!=AMap.end())
	AMap[A]=vc->second*signB;
      else
	AMap[A]=B;
    }
  else
    AMap[A]=B;

  // Positive search FIRST:
  vc=find_if(AMap.begin(),AMap.end(),
	     MapSupport::valEqual<int,int>(A));
  while (vc!=AMap.end())
    {
      vc->second= B;
      vc=find_if(vc,AMap.end(),
		 MapSupport::valEqual<int,int>(A));
    }

  // Negative search FIRST:
  vc=find_if(AMap.begin(),AMap.end(),
	     MapSupport::valEqual<int,int>(-A));
  while (vc!=AMap.end())
    {
      vc->second= -B;
      vc=find_if(vc,AMap.end(),
		 MapSupport::valEqual<int,int>(-A));
    }
  return;
}

void
AlterSurf::setMapZero()
  /*!
    Given a set of strings set the exchange on the string
    The principle is that if the variable equal 0, then
    the first surface is substituted for the second.
    A cascade is run for each.
  */
{
  std::vector<DTriple<std::string,int,int> >DX;

      DX.push_back(DTriple<std::string,int,int>("tarcage",12,11));
      DX.push_back(DTriple<std::string,int,int>("inconnelwin",198,197));
      DX.push_back(DTriple<std::string,int,int>("inccool",197,196));
      DX.push_back(DTriple<std::string,int,int>("inconnelwin",196,195));
      DX.push_back(DTriple<std::string,int,int>("vacspacer",43002,41002));
      DX.push_back(DTriple<std::string,int,int>("outfrontal",41002,39002));
      DX.push_back(DTriple<std::string,int,int>("gsndal",35003,35002));
      DX.push_back(DTriple<std::string,int,int>("thermoft",33015,33005));
      DX.push_back(DTriple<std::string,int,int>("thermost",14503,14502));
      DX.push_back(DTriple<std::string,int,int>("thermost",14403,14402));
      
      DX.push_back(DTriple<std::string,int,int>("sndhvac",39002,37002));
      DX.push_back(DTriple<std::string,int,int>("midfrontal",37002,35002));
      DX.push_back(DTriple<std::string,int,int>("goutal",39003,35002));
      DX.push_back(DTriple<std::string,int,int>("fsthvac",35002,33002));
      DX.push_back(DTriple<std::string,int,int>("outhalface",42003,40003));
      DX.push_back(DTriple<std::string,int,int>("sndhvac",40003,38003));
      DX.push_back(DTriple<std::string,int,int>("midhalface",38003,36003));
      DX.push_back(DTriple<std::string,int,int>("fsthvac",36003,34003));
      DX.push_back(DTriple<std::string,int,int>("outhal",42005,40005));
      DX.push_back(DTriple<std::string,int,int>("sndhvac",40005,38005));
      DX.push_back(DTriple<std::string,int,int>("midhal",38005,36005));
      DX.push_back(DTriple<std::string,int,int>("sidefsthvac",36005,34005));
      DX.push_back(DTriple<std::string,int,int>("outhal",42006,40006));
      DX.push_back(DTriple<std::string,int,int>("sndhvac",40006,38006));
      DX.push_back(DTriple<std::string,int,int>("midhal",38006,36006));
      DX.push_back(DTriple<std::string,int,int>("sidefsthvac",36006,34006));
      DX.push_back(DTriple<std::string,int,int>("outhal",42007,40007));
      DX.push_back(DTriple<std::string,int,int>("sndhvac",40007,38007));
      DX.push_back(DTriple<std::string,int,int>("midhal",38007,36007));
      DX.push_back(DTriple<std::string,int,int>("fsthvac",36007,34007));
      DX.push_back(DTriple<std::string,int,int>("outhal",42008,40008));
      DX.push_back(DTriple<std::string,int,int>("sndhvac",40008,38008));
      DX.push_back(DTriple<std::string,int,int>("midhal",38008,36008));
      DX.push_back(DTriple<std::string,int,int>("fsthvac",36008,34008));
      DX.push_back(DTriple<std::string,int,int>("premodspace",42107,42007));
      DX.push_back(DTriple<std::string,int,int>("pinneral",42207,42107));
      DX.push_back(DTriple<std::string,int,int>("modspace",54003,42003));
      DX.push_back(DTriple<std::string,int,int>("castlefgap",32000,31005));
      DX.push_back(DTriple<std::string,int,int>("castlebgap",32001,32));
      DX.push_back(DTriple<std::string,int,int>("radiator",10191,10111));
      DX.push_back(DTriple<std::string,int,int>("radiator",10391,0));
      DX.push_back(DTriple<std::string,int,int>("radiator",10392,0));
      DX.push_back(DTriple<std::string,int,int>("lmiddcsep",10111,10101));
      DX.push_back(DTriple<std::string,int,int>("rmiddcsep",10112,10102));
      DX.push_back(DTriple<std::string,int,int>("dalheat",10302,12902));
      DX.push_back(DTriple<std::string,int,int>("midvacgap",12902,12802));
      DX.push_back(DTriple<std::string,int,int>("daltern",12802,10402));
      DX.push_back(DTriple<std::string,int,int>("vacgap",10402,10502));
      DX.push_back(DTriple<std::string,int,int>("dalpress",10502,10602));
      DX.push_back(DTriple<std::string,int,int>("radalthick",11291,10291));
      DX.push_back(DTriple<std::string,int,int>("radalthick",11292,10292));
      DX.push_back(DTriple<std::string,int,int>("dalheat",11302,11902));
      DX.push_back(DTriple<std::string,int,int>("midvacgap",11902,11802));
      DX.push_back(DTriple<std::string,int,int>("daltern",11802,11402));
      DX.push_back(DTriple<std::string,int,int>("vacgap",11402,11502));
      DX.push_back(DTriple<std::string,int,int>("dalpress",11502,11602));
      DX.push_back(DTriple<std::string,int,int>("dalheat",10203,12903));
      DX.push_back(DTriple<std::string,int,int>("midvacgap",12903,12803));
      DX.push_back(DTriple<std::string,int,int>("daltern",12803,10403));
      DX.push_back(DTriple<std::string,int,int>("vacgap",10403,10503));
      DX.push_back(DTriple<std::string,int,int>("dalpress",10503,10603));
      DX.push_back(DTriple<std::string,int,int>("dalheat",10204,12904));
      DX.push_back(DTriple<std::string,int,int>("midvacgap",12904,12804));
      DX.push_back(DTriple<std::string,int,int>("daltern",12804,10404));
      DX.push_back(DTriple<std::string,int,int>("vacgap",10404,10504));
      DX.push_back(DTriple<std::string,int,int>("dalpress",10504,10604));
      DX.push_back(DTriple<std::string,int,int>("dalheat",10205,12905));
      DX.push_back(DTriple<std::string,int,int>("dalheat",10206,12906));
      DX.push_back(DTriple<std::string,int,int>("vacgap",10405,10505));
      DX.push_back(DTriple<std::string,int,int>("vacgap",10406,10506));
      DX.push_back(DTriple<std::string,int,int>("dalpress",10506,10606));
      DX.push_back(DTriple<std::string,int,int>("dalpress",10505,10605));
      DX.push_back(DTriple<std::string,int,int>("midvacgap",12905,12805));
      DX.push_back(DTriple<std::string,int,int>("daltern",12805,10405));
      DX.push_back(DTriple<std::string,int,int>("midvacgap",12906,12806));
      DX.push_back(DTriple<std::string,int,int>("daltern",12806,10406));
      DX.push_back(DTriple<std::string,int,int>("poisonsep",10201,133));
      DX.push_back(DTriple<std::string,int,int>("shieldthick",90001,200));
      DX.push_back(DTriple<std::string,int,int>("shieldthick",90002,200));
      DX.push_back(DTriple<std::string,int,int>("iradBeThick",21102,21101));
      DX.push_back(DTriple<std::string,int,int>("iradSteelThick",21103,21102));
      DX.push_back(DTriple<std::string,int,int>("iradWaterThick",21104,21103));
      DX.push_back(DTriple<std::string,int,int>("iradEAThick",21202,21201));
      DX.push_back(DTriple<std::string,int,int>("iradEBThick",21203,21202));
      DX.push_back(DTriple<std::string,int,int>("iradECThick",21204,21203));
      
      DX.push_back(DTriple<std::string,int,int>("cdlayer",164,168));
      DX.push_back(DTriple<std::string,int,int>("cdlayer",165,169));
      DX.push_back(DTriple<std::string,int,int>("cdlayer",166,170));
      DX.push_back(DTriple<std::string,int,int>("cdlayer",167,171));
      DX.push_back(DTriple<std::string,int,int>("cdlayer",106,172));
      DX.push_back(DTriple<std::string,int,int>("cdlayer",107,173));
      DX.push_back(DTriple<std::string,int,int>("wallalnarrow",160,164));
      DX.push_back(DTriple<std::string,int,int>("wallalnarrow",161,165));
      DX.push_back(DTriple<std::string,int,int>("wallalbroad",160,164));
      DX.push_back(DTriple<std::string,int,int>("wallalbroad",161,165));
      DX.push_back(DTriple<std::string,int,int>("wallalbroad",10605,172));
      DX.push_back(DTriple<std::string,int,int>("wallalbroad",10606,173));
      
      DX.push_back(DTriple<std::string,int,int>("ngvwidth",11103,0));
      DX.push_back(DTriple<std::string,int,int>("ngvwidth",11104,0));
      DX.push_back(DTriple<std::string,int,int>("ngvwidth",11203,0));
      DX.push_back(DTriple<std::string,int,int>("ngvwidth",11204,0));
      DX.push_back(DTriple<std::string,int,int>("ngvwidth",11303,0));
      DX.push_back(DTriple<std::string,int,int>("ngvwidth",11304,0));
      DX.push_back(DTriple<std::string,int,int>("ngvwidth",11403,0));
      DX.push_back(DTriple<std::string,int,int>("ngvwidth",11404,0));
      DX.push_back(DTriple<std::string,int,int>("ngvwidth",11503,0));
      DX.push_back(DTriple<std::string,int,int>("ngvwidth",11504,0));

      DX.push_back(DTriple<std::string,int,int>("ngvheight",11105,0));
      DX.push_back(DTriple<std::string,int,int>("ngvheight",11106,0));
      DX.push_back(DTriple<std::string,int,int>("ngvheight",11205,0));
      DX.push_back(DTriple<std::string,int,int>("ngvheight",11206,0));
      DX.push_back(DTriple<std::string,int,int>("ngvheight",11305,0));
      DX.push_back(DTriple<std::string,int,int>("ngvheight",11306,0));
      DX.push_back(DTriple<std::string,int,int>("ngvheight",11405,0));
      DX.push_back(DTriple<std::string,int,int>("ngvheight",11406,0));
      DX.push_back(DTriple<std::string,int,int>("ngvheight",11505,0));
      DX.push_back(DTriple<std::string,int,int>("ngvheight",11506,0));

      std::vector<DTriple<std::string,int,int> >::const_iterator vc;
      for(vc=DX.begin();vc!=DX.end();vc++)
        {
	  if (ConVar.EvalVar<double>(vc->first)<=0.0)
	    {
	      setChange(vc->second,vc->third);
	    }
	}
      return; 
}

void
AlterSurf::setMap()
  /*!
    Creates the altered plane list
  */
{
  const int lobactive=static_cast<int>(ConVar.EvalVar<double>("lobactive"));
  const int nvane=static_cast<int>(ConVar.EvalVar<double>("nvane"));
  const int nrad=static_cast<int>(ConVar.EvalVar<double>("nrad"));
  const int nslice=static_cast<int>(ConVar.EvalVar<double>("nslice"));
  const int ncastle=static_cast<int>(ConVar.EvalVar<double>("ncastle"));

  const double fsteel=ConVar.EvalVar<double>("fsteel");  
  const double pmsize=ConVar.EvalVar<double>("pmsize");
  const double lead=ConVar.EvalVar<double>("lead");
  const double lobheight=ConVar.EvalVar<double>("lobheight");
  const double dcpm=ConVar.EvalVar<double>("dcpm");
  const double dclead=ConVar.EvalVar<double>("dclead");
  const double conerad=ConVar.EvalVar<double>("conerad");
  const double conelength=ConVar.EvalVar<double>("conelength");
  const double steel=ConVar.EvalVar<double>("steel");
  const double radius=ConVar.EvalVar<double>("radius");
  const double hfaceoffset=ConVar.EvalVar<double>("hfaceoffset");
  const double wallalnarrow=ConVar.EvalVar<double>("wallalnarrow");
  const double wallalbroad=ConVar.EvalVar<double>("wallalbroad");
  const double cladding=ConVar.EvalVar<double>("cladding");
  const double fextn=ConVar.EvalVar<double>("fextn");
  const double fwater=ConVar.EvalVar<double>("fwater");
  const double tsa=ConVar.EvalVar<double>("tsa");
  const double tsb=ConVar.EvalVar<double>("tsb");
  const double lbewidth=ConVar.EvalVar<double>("lbewidth");
  const double rbewidth=ConVar.EvalVar<double>("rbewidth");
  const double gdlayer=ConVar.EvalVar<double>("gdlayer");
  const double daltern=ConVar.EvalVar<double>("daltern");
  const double midvacgap=ConVar.EvalVar<double>("midvacgap");
  const double poisonsep=ConVar.EvalVar<double>("poisonsep");
  const double dcpmext=ConVar.EvalVar<double>("dcpmext");  
  const double cdtopsize=ConVar.EvalVar<double>("cdtopsize");  
  const double cdcycsize=ConVar.EvalVar<double>("cdcycsize");  
  const double cdbasesize=ConVar.EvalVar<double>("cdbasesize");  
  const double cradius=ConVar.EvalVar<double>("cradius");  
  const double vanelength=ConVar.EvalVar<double>("vanelength");  
  const double castleextra=ConVar.EvalVar<double>("castleextra");  
  const double sndhvac=ConVar.EvalVar<double>("sndhvac");  
  const double outhalface=ConVar.EvalVar<double>("outhalface");  
  const double ibe=ConVar.EvalVar<double>("iradBeThick");
  const double isteel=ConVar.EvalVar<double>("iradSteelThick");
  const double iwat=ConVar.EvalVar<double>("iradWaterThick");
  const double iea=ConVar.EvalVar<double>("iradEAThick");
  const double ieb=ConVar.EvalVar<double>("iradEBThick");
  const double iec=ConVar.EvalVar<double>("iradECThick");
//  const double cdlayer=ConVar.EvalVar<double>("cdlayer");

  if (fsteel<=0.0)   // Surface window
    setChange(3,5009);

  if (lead+pmsize<=0.0)
    {
      setChange(57,19);
      setChange(3001,19);
    }
  else if (lead+pmsize<=0.55)
    setChange(3001,19);
 				       
  if (lobactive<=0 || lobheight<=0.0)
    setChange(202,201);
  
  if (lobactive<=0)
    {
      setChange(217,0);
      setChange(218,0);
      setChange(221,0);
      setChange(222,0);
      setChange(231,0);
      setChange(232,0);
      setChange(237,0);
      setChange(238,0);
      setChange(241,0);
      setChange(242,0);
      setChange(247,0);
      setChange(248,0);
    }

  if (dcpm+dclead<=0.0)
    setChange(139,18);

  if (lead<0.0)
    setChange(1901,19);

  
//  if (hfaceoffset<=0)
//    setChange(2102,21);

  if (conerad<=0.0 or conelength<0.0 or radius==conerad)
    {
      setChange(2066,0);
      setChange(2099,0);
      setChange(2566,0);
      setChange(6066,0);
    }
  
  // Target cladding
  if (cladding<=0)
    {
      setChange(2555,2);
      if (fextn<=0.0)
	setChange(2556,5);
      else
	setChange(2556,5001);
    }
  

// Forward of target units
  if (fextn<=0.0)
    setChange(5002,5001);

// Forward cladding 
  if (cladding<=0)
    {
      if (fextn<=0)       
	setChange(5003,5001);
      else
	setChange(5003,5002);
    }

  if (fwater<=0)
    {
      if (cladding>0)
	setChange(5009,5003);
      else if  (fextn>0)
	setChange(5009,5002);
      else
	setChange(5009,5001);
    }
 // Flight lines on the decoupled Poisoned:
    
 // Decoupled moderator
  if (lbewidth<=0.0 && rbewidth<=0.0)
    setChange(10101,10102);
    
//  Extra layer for premoderator seperator
  if (dcpm<=0.0)
    setChange(10106,18);
  
// Outside poison layer
  if (gdlayer<=0.0)
    {
      setChange(10201,133);
      setChange(10203,10103);
      setChange(10204,10104);
      setChange(10205,10105);
      setChange(10206,10106);
    }
  
    // Mid Aluminium layer
  if (daltern<=0.0)
    {
      setChange(12802,10402);
      setChange(12803,10403);
      setChange(12804,10404);
      setChange(12805,10405);
      setChange(12806,10406);
    }
  
    // Mid vac layer
  if (midvacgap<=0.0)
    {
      const int surNum=(daltern<=0.0) ? 12802 : 10402;
      setChange(12902,surNum);
      setChange(12903,surNum+1);
      setChange(12904,surNum+2);
      setChange(12905,surNum+3);
      setChange(12906,surNum+4);
    }
  
  // Vanes narrow side
  for(int i=nvane+1;i<17;i++)  
    setChange(10700+i,0);

  if (poisonsep<=0.0)
    setChange(10601,10112);

  // Radiator cross cylinders
//  for(int i=nrad;i<10;i++)
//    setChange(10390+i,0);
  
  // Radiator horr. cyclinder (vacuum)
  for(int i=nrad+1;i<10;i++)
    {
      setChange(10490+i,0);
      setChange(11490+i,0);
    }
  
  // Nslice 
  for(int i=nslice;nslice!=0 && i<20;i++)
    setChange(10610+i,0);
    
  // Extension of premod
  if (dcpmext<=0.0)
    {
      setChange(14001,132);
      setChange(14101,133);
      setChange(14201,10103);
      setChange(14301,10104);
    }
  
  if (dclead<=0)
    setChange(18001,18);
  
  // Top decoupler
  if (cdtopsize<=0.0)
    {
      setChange(19101,19001);
      setChange(19102,19002);      
      setChange(19103,19003);      
      setChange(19104,19004);      
    }
  
  // vane depth field (narrow)
  if (nvane<=0 || vanelength<=0)       
    setChange(10802,0);      

  // cylinder decoupler
  if (cdcycsize<=0.0)
    setChange(19301,19201);

  // base decoupler
  if (cdbasesize<=0.0)
    setChange(19501,19500);
  
//
// New Groove
//
  
  if (castleextra<=0.0 || ncastle<=0)
    setChange(31804,31004);
  
  if (sndhvac<=0.0)                          
    setChange(40008,36008);
  
// Outside Groove Hydrogen Face
  if (outhalface<=0.0)
    setChange(42003,40003);
//
// Castle
//
  for(int i=ncastle+1;i<10;i++)
    {
      setChange(32100+i,0);
      setChange(32200+i,0);
    }
//
// Pipework
//
// Outside Groove Pipe (check)
  if (cradius<=0.0)                        
    setChange(80000,0);

// Irradiation Channel
  if (ibe+isteel+iwat <=0.0)
    {
      setChange(21101,0);
      setChange(21102,0);
      setChange(21103,0);
      setChange(21104,0);
    }

  if (iea+ieb+iec <=0.0)
    {
      setChange(21201,0);
      setChange(21202,0);
      setChange(21203,0);
      setChange(21204,0);
    }

  if (isteel)
    {
      setChange(90001,0);
      setChange(90002,0);
      setChange(90003,0);
      setChange(90004,0);
      setChange(90013,0);
      setChange(90014,0);
    }

  return;
}
