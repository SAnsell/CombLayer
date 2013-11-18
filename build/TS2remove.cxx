/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   build/TS2remove.cxx
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
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <vector>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "RemoveCell.h"
#include "TS2remove.h"

TS2remove::TS2remove() : RemoveCell()
 /*! 
   Default constructor
 */		       
{}

TS2remove::TS2remove(FuncDataBase& FDB) : 
  RemoveCell(FDB)
  /*!
    Constructor
    \param FDB :: Reference function database
  */
{
  populateZero();
}



TS2remove::TS2remove(const TS2remove& A) : 
  RemoveCell(A)
  /*!
    Copy constructor 
    \param A :: object to copy
  */
{}

TS2remove&
TS2remove::operator=(const TS2remove& A) 
  /*!
    Assignement operator
    \param A :: object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      RemoveCell::operator=(A);
    }
  return *this;
}

void
TS2remove::sliceReprocess(const int ICN,MonteCarlo::Qhull& Cell)
  /*!
    This handles the stupid mess that is the vanes
    Slices and dices : Expecting 20->40 and 60->80
    \param ICN :: Cell number
    \param Cell :: Cell Object ot modify
  */
{
  const int nvane=static_cast<int>(RemoveCell::ConVar->EvalVar<double>("nvane"));
  const int nslice=static_cast<int>(RemoveCell::ConVar->EvalVar<double>("nslice"));
  if (nslice<=1 || nvane<=0)  // No work
    return;

  const int ch4mat=static_cast<int>(RemoveCell::ConVar->EvalVar<double>("chtype"));
  const int bMat=static_cast<int>(RemoveCell::ConVar->EvalVar<double>("vaneback"));
  const int fMat=static_cast<int>(RemoveCell::ConVar->EvalVar<double>("vanemat"));


  const int Part( ((ICN % 100)<40) ? 0 : 1);
  const int fItem(((ICN % 100)/20) % 2);
  const int N(ICN % 2);
  // (1 1)  or (0 0) : start value is offset 
  const int offset( (Part==N) ? 2 : 1);

  if ((!fItem && fMat==ch4mat) ||
      (fItem && ch4mat==bMat))
    {
      if (ICN % 100>40)
	RemoveCell::ZeroCellComp.insert(ZCCType::value_type(ICN,"zero"));
      return;
    }    

  if (offset==2)          // remove top layer
    substituteSurf(Cell,10605,10611);

  std::ostringstream cx;
  int lineNum;
  for(lineNum=offset;lineNum<nslice-1;lineNum+=2)
    {
      cx.str("");
      cx<<"( "<<-(10610+lineNum)<<" : "<<10610+lineNum+1<<" )";
      Cell.addSurfString(cx.str());
    }

  if (lineNum==nslice-1)
    substituteSurf(Cell,10606,10610+nslice-1);

  return;
}

int
TS2remove::spaceReq(const int idx)
  /*!
    Calculates the spacer requirements
    for the curve part of the vanes.
    \param idx :: Index on vane
    \retval true :: then the item should be eliminated
    \retval false :: Item is ok.
  */
{  
  const int vn=static_cast<int>(RemoveCell::ConVar->EvalVar<double>("nvane"));
//  const double vd=ConVar->EvalVar<double>("vanelength");
  const double ps=RemoveCell::ConVar->EvalVar<double>("poisonsep");
  const double yp=ps+RemoveCell::ConVar->EvalVar<double>("rbewidth");     // y intersect point
  const double cw=RemoveCell::ConVar->EvalVar<double>("centwidth");       // width
  double zp1=cw*((2.0*idx-1.0)-vn)/(vn+1.0);          // z point 1
  double zp2=zp1+RemoveCell::ConVar->EvalVar<double>("vanewidth");  // z point 2
  
  const double centY=RemoveCell::ConVar->EvalVar<double>("ncentre");
  RemoveCell::ConVar->Parse("alradius-(dalheat+vacgap+dalpress+vanelength)");
  double Rad=RemoveCell::ConVar->Eval();

  Rad*=Rad;

// USE (Y-off)^2+Z^2=R^2
  zp1*=zp1;
  zp2*=zp2;

  double y2=Rad-zp1;
  if (y2<=0.0)        // missed
    return 0;

  y2=sqrt(y2)+centY;
  if (y2<=yp)
    return 0;

  y2=Rad-zp2;
  if (y2<=0.0)        // missed
    return 0;

  y2=sqrt(y2)+centY;
  if (y2<yp)
    return 0;

  return 1;
}

void
TS2remove::populateZero()
  /*!
    Populate ZeroCellComp list
  */
{
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(66,"inconnelwin"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(67,"inccool"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(68,"inconnelwin"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(613,"fsthvac"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(615,"midhal"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(617,"sndhvac"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(619,"outhal"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(5001,"tarcage"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(6401,"fsthvac"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(6701,"sndhvac"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(6801,"outhal"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(10502,"modspace"));
  // Thermal holder
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(6223,"thermoft"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(6224,"thermost"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(6225,"thermost"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(6227,"tflapout"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(6228,"tflapout"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(6231,"tboltradius"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(6232,"tboltradius"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(6302,"tflapout"));

  // Aluminium Wall (groove):
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(10003,"wallAlGroove"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(10004,"wallAlGroove"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(10005,"wallAlGroove"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(10006,"wallAlGroove"));


  // Aluminium Wall (decoupled):
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(46,"wallalbroad"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(47,"wallalbroad"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(50,"wallalbroad"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(51,"wallalbroad"));

  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(48,"wallalnarrow"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(49,"wallalnarrow"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(52,"wallalnarrow"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(53,"wallalnarrow"));

  // Decoupler (flight lines):
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(54,"cdBroadL"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(55,"cdBroadL"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(56,"cdNarrowL"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(57,"cdNarrowL"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(58,"cdBroadL"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(59,"cdBroadL"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(60,"cdNarrowL"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(61,"cdNarrowL"));

  // Pre-mod:
  
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(12002,"pinneral"));  
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(12007,"pinneral"));  
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(12101,"premodspace"));  
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(12102,"premodspace"));  
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(12111,"premodspace"));  

  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21001,"lmiddcsep"));    // Spacer (Be normally)
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21002,"rmiddcsep"));    // Spacer (Be normally)
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21106,"poisonsep"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21107,"poisonsep"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21201,"dalheat"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21203,"vacgap"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21203,"dalpress"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21208,"daltern"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21209,"midvacgap"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(22105,"DCleftGap"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(22106,"DCrightGap"));

   // Decouple pre-mod
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(25001,"DCPreModTThick")); 

   // Forward target hole
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(62051,"ncastle"));           // Forward castle segments
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(62052,"ncastle"));           // Back castle segments
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(64001,"fsteel"));            // front steel window
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(76001,"cdtopsize"));         // Cd bucket top
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(76101,"cdcycsize"));          // Cd bucket side 
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(76102,"cdcycsize"));          // Cd bucket side 
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(76201,"cdcycsize"));          // Cd bucket side 
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(76301,"cdbasesize"));         // Cd bucket base

  // Irradiation channel
/*  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(80001,"iradHsize"));      
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(80002,"iradHsize"));      
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(80003,"iradHsize"));      
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(80004,"iradHsize"));      
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(80011,"iradHsize"));      
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(80012,"iradHsize"));      
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(80013,"iradHsize"));      
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(80031,"iradHsize"));      
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(80032,"iradHsize"));      
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(80041,"iradHsize"));
*/
  // Decoupled Extension BROAD:
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21291,"bgvwidth"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21293,"bgvwidth"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21296,"bgvwidth"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21298,"bgvwidth"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21299,"bgvwidth"));
//  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(62011,"ngvwidth"));
  
  //Needs to be changed to deal with negative hole
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21301,"bgvwidth"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21302,"bgvholedepth"));

  // Decoupled Extension NARROW:
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(2121,"ngvwidth"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21283,"ngvwidth"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21286,"ngvwidth"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21288,"ngvwidth"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21289,"ngvwidth"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(63011,"ngvwidth"));
  //Needs to be changed to deal with negative hole
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21281,"ngvlength"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21381,"ngvlength"));
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(21382,"ngvholedepth"));

// Stuff for   
  return;
}


bool
TS2remove::toBeRemoved(const int ICN,MonteCarlo::Qhull& Cell)
  /*!
    Function to used QHull and cell to determine if
    a cell is valid
    \param ICN :: Cell number (from MCNPX file)
    \param Cell :: Cell object in Qhull format
    \return true if the cell should be removed
  */
{
  if (!RemoveCell::ConVar)
    {
      ELog::EM.error("TS2remove::toBeRemoved not initialized");
      exit(1);
    }
  
  ZCCType::const_iterator vc;
  vc=RemoveCell::ZeroCellComp.find(ICN);
  while (vc!=RemoveCell::ZeroCellComp.end() && vc->first==ICN)
    {
      if (RemoveCell::ConVar->EvalVar<double>(vc->second)<=0.0)
	return 1;
      vc++;
    }

  double a,b;
  int ia,ib,ipart;
  switch (ICN) 
    {
    case 6205:
      if (RemoveCell::ConVar->EvalVar<double>("ncastle")>0)
	return 1;
      break;

    case 11001:                               // Pre-moderator
      if (RemoveCell::ConVar->EvalVar<double>("pmsize")<=0 ||
	  RemoveCell::ConVar->EvalVar<double>("skins")+RemoveCell::ConVar->EvalVar<double>("hfaceoffset")>=0)
	return 1;
      break;

    case 12111:                       // spacer between H pre and main-pre
      if (!Cell.calcIntersections())
        {
	  ELog::EM.diagnostic("Removing cell 12111");
	  return 1;
	}
      break;

    case 12212:                        // spacer between H pre and main-pre
      if (!Cell.calcIntersections())
	return 1;
      break;

    case 21000:                               // Decoupled separator
      if (RemoveCell::ConVar->EvalVar<double>("lbewidth")<=0 &&
	  RemoveCell::ConVar->EvalVar<double>("rbewidth")<=0)
	return 1;
      break;

    case 21107:                                 // Poison layer (traditional)
      if (RemoveCell::ConVar->EvalVar<double>("poisonsep")<=0.0)
	return 1;
      break;

    case 21101:                                  // Gd layers
    case 21106:
    case 21191:
    case 21192:
    case 21193:
    case 21194:
      if (RemoveCell::ConVar->EvalVar<double>("gdlayer")<=0.0)
	return 1;
      break;

    case 21181:                                 // corner spacers
    case 21182:
    case 21183:
    case 21184:
    case 21185:
    case 21186:
      if (!Cell.calcIntersections())
	return 1;
      break;

      // Extension to the narrow
    case 21201:
    case 21203:
    case 21208:
    case 21209:
      if (RemoveCell::ConVar->EvalVar<double>("ngvwidth")<=0.0)
	Cell.removeSurface(10102);
      break;

    // De-coupled poisioning seperator
    case 21401:                                     
      if (RemoveCell::ConVar->EvalVar<double>("poisonsep")<=0.0)
	return 1;

    case 21501:                                      // Vanes methane spacers
    case 21502:
    case 21503:
    case 21504:
    case 21505:
    case 21506:
    case 21507:
    case 21508:
    case 21509:
    case 21510:
    case 21511:
    case 21512:
    case 21513:
    case 21514:
    case 21515:
    case 21516:
      ia=static_cast<int>(RemoveCell::ConVar->EvalVar<double>("nvane"));
      ipart=ICN % 100;
      if (ipart>ia+1)
	return 1;
      if (ipart==ia+1 && ipart!=16)  	    // End sub in plane 
        {
	  substituteSurf(Cell,10700+ipart,10604);
	  return 0;
	}
      
      // substitution heere 
      break;

    case 22711:                             //  Al vertical surrounds
      if (RemoveCell::ConVar->EvalVar<double>("radiator")<=0 ||
	  RemoveCell::ConVar->EvalVar<double>("radalthick")<=0)
	return 1;
      if (RemoveCell::ConVar->EvalVar<double>("nrad")<1)
	Cell.removeSurface(10298);
      break;

    case 22712:
      if (RemoveCell::ConVar->EvalVar<double>("radiator")<=0 ||
	  RemoveCell::ConVar->EvalVar<double>("radalthick")<=0)
	return 1;
      if (RemoveCell::ConVar->EvalVar<double>("nrad")<1)
	Cell.removeSurface(10299);
      break;
    
    case 22901:                                         // Spacer in corner of narrow side
    case 22903:
    case 22904:
      if (!Cell.calcIntersections())
	return 1;
      break;

    case 62030:
    case 62031:
    case 62032:
    case 62033:
    case 62034:
    case 62035:
    case 62036:
    case 62037:
    case 62038:
    case 62039:
      if (RemoveCell::ConVar->EvalVar<double>("ncastle")<=0 ||
	  RemoveCell::ConVar->EvalVar<double>("castleextra")<=0)
	return 1;
      break;

    case 66011:              // groove secondary al [cur]
      if (RemoveCell::ConVar->EvalVar<double>("gsndal")<=0.0 ||
	  RemoveCell::ConVar->EvalVar<double>("midfrontal")<=0.0 )
	return 1;
      break;

    case 66012:              // groove secondary al vacuum [cur]
      a=RemoveCell::ConVar->EvalVar<double>("gsndal");
      b=RemoveCell::ConVar->EvalVar<double>("midfrontal");
      if (b<=0.0 || a>=b)
	return 1;
      break;

    case 68011:                   // groove tertiary al [cur]
      if (RemoveCell::ConVar->EvalVar<double>("gsndal")<=0.0 ||
	  RemoveCell::ConVar->EvalVar<double>("outfrontal")<=0.0 )
	return 1;
      break;

    case 68012:              // groove tertiary al vacuum [cur]
      a=RemoveCell::ConVar->EvalVar<double>("goutal");
      b=RemoveCell::ConVar->EvalVar<double>("outfrontal");
      if (b<=0.0 || a>=b)
	return 1;
      break;

    case 71301:          // Coupled moderator
    case 71302:
    case 73013:
    case 73014:
    case 73015:
    case 73016:
      if (!Cell.calcIntersections())
	return 1;
      break;
/*
  if (Icell/100==246):
*/
	
    default:

      if (ICN/100==215)               // Methane spacer
        {
	  ia=static_cast<int>(RemoveCell::ConVar->EvalVar<double>("nvane"));
	  ib=ICN % 100;
	  if (ib>ia+1) 	      // "Eliminating methane section ",nm
	    return 1;
	  return 0;
	}

      if (ICN/100==216) // The Vanes
        {
	  ia=static_cast<int>(RemoveCell::ConVar->EvalVar<double>("nvane"));
	  ib=static_cast<int>(RemoveCell::ConVar->EvalVar<double>("nslice"));
	  a=RemoveCell::ConVar->EvalVar<double>("vanelength");
	  b=RemoveCell::ConVar->EvalVar<double>("poisonsep");
	  ipart= ICN % 20;
	  int itype= (ICN % 100)/20;
	  if (ia<ipart)        // full vanes
	    return 1;
	  if (!itype)           // Vane (not a spacer)
	    {
	      if (a<0.0 && b<0.0)
		substituteSurf(Cell,10802,10112);
	      else if (a<0.0)          // No actual poisoning
		substituteSurf(Cell,10802,10601);
	      else if (!spaceReq(ipart))
		substituteSurf(Cell,10802,10601);
	      return 0;
	    }
	  else                   // Spacer (not vane)
	    {
	      if (a<0.0 || !spaceReq(ipart))      
		return 1;
	      if (b<0.0)
		substituteSurf(Cell,10602,10112);
	    }

	  if ((ICN % 100)>40 && ib<=1)
	    return 1;
	}

      if (ICN/100==226)            // 22600
        {
	  ia=static_cast<int>(RemoveCell::ConVar->EvalVar<double>("nrad"));
	  if (ia<(ICN % 10))
	    return 1;
	  return 0;
	}

      if (ICN/100==246)            // 24600
        {
	  ia=static_cast<int>(RemoveCell::ConVar->EvalVar<double>("nrad"));
	  a=RemoveCell::ConVar->EvalVar<double>("radalthick");
	  if (ia<(ICN % 10) || a<=0.0)
	    return 1;
	  return 0;
	}

      if (ICN/100==621)            // 62100
        {
	  ia=static_cast<int>(RemoveCell::ConVar->EvalVar<double>("ncastle"));
	  if (ia==0 || ia+1<(ICN % 10))
	    return 1;
	  if (ia+1==(ICN % 10))
	    {
	      ia+=32101;
	      substituteSurf(Cell,ia,-14401);
	    }
	  return 0;
	}

      if (ICN/100==622)            // 62200
        {
	  ia=static_cast<int>(RemoveCell::ConVar->EvalVar<double>("ncastle"));
	  if (ia==0 || ia<(ICN % 10))
	    return 1;
	  return 0;
	}
      
    }
  return 0;
}


