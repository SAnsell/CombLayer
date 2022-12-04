/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essModel/WedgeFlightLine.cxx
 *
 * Copyright (c) 2004-2022 by Konstantin Batkov
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "FixedRotateUnit.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "BasicFlightLine.h"
#include "WedgeItem.h"
#include "WedgeFlightLine.h"

namespace essSystem
{

WedgeFlightLine::WedgeFlightLine(const std::string& TKey) :
  moderatorSystem::BasicFlightLine(TKey)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param TKey :: Name for item in search
  */
{}

WedgeFlightLine::WedgeFlightLine(const WedgeFlightLine& A) : 
  moderatorSystem::BasicFlightLine(A),
  nWedges(A.nWedges),wedges(A.wedges)
  /*!
    Copy constructor
    \param A :: WedgeFlightLine to copy
  */
{}

WedgeFlightLine&
WedgeFlightLine::operator=(const WedgeFlightLine& A)
  /*!
    Assignment operator
    \param A :: WedgeFlightLine to copy
    \return *this
  */
{
  if (this!=&A)
    {
      moderatorSystem::BasicFlightLine::operator=(A);
      nWedges=A.nWedges;
      wedges=A.wedges;
    }
  return *this;
}

  
WedgeFlightLine::~WedgeFlightLine() 
 /*!
   Destructor
 */
{}

void
WedgeFlightLine::populate(const FuncDataBase& Control)
/*!
  Populate all the variables
  \param Control :: Variable data base
*/
{
  ELog::RegMethod RegA("WedgeFlightLine","populate");
  
  nWedges=Control.EvalDefVar<size_t>(keyName+"NWedges", 0);
  return;
}

void
WedgeFlightLine::buildWedges(Simulation& System,
			   const attachSystem::FixedComp& innerFC,
			   const long int innerIndex,
			   const attachSystem::FixedComp& outerFC,
			   const long int outerIndex)
  /*!
    Builds the flight line wedges.
    Arguments are the same as in createAll.
    \param System :: Simulation to add vessel to
    \param originFC :: Origin
    \param originIndex :: Use side index from Origin
    \param innerFC :: Moderator Object
    \param innerIndex :: Use side index from moderator
    \param outerFC :: Edge of bulk shield 
    \param outerIndex :: Side index of bulk shield
  */
{
  ELog::RegMethod RegA("WedgeFlightLine","buildWedges");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  ELog::EM<<"N Wedge == "<<nWedges<<ELog::endDiag;
  if (nWedges<1) return;

  for(size_t i=0;i<nWedges;i++)
    {
      std::shared_ptr<WedgeItem> wedgeObj(new WedgeItem(keyName+"Wedge",i+1));
      OR.addObject(wedgeObj);
      wedgeObj->setCutSurf("OuterCyl",outerFC,outerIndex);
      wedgeObj->setCutSurf("OuterCyl",outerFC,outerIndex);
      wedgeObj->createAll(System,outerFC,0,outerIndex,*this,-11,-12);
      wedges.push_back(wedgeObj);
    }

  // rebuild the Inner cell
  const std::pair<int,double> MatInfo=deleteCellWithData(System,"innerVoid");

  std::string Out;

  const std::string baseOut=
    innerFC.getLinkString(innerIndex)+
    outerFC.getLinkString(outerIndex)+
    this->getLinkString(-11)+
    this->getLinkString(-12);
  ELog::EM<<"Out["<<keyName<<"] == "<<baseOut<<ELog::endDiag;

  // Create the radial surfaces that divide the wedges 
  int index(buildIndex+1001);
  for(size_t i=0;i<nWedges;i++,index++)
    ModelSupport::buildPlaneRotAxis(SMap,index,
                                    wedges[i]->getCentre(),
                                    wedges[i]->getY(),Z,90.0);

  // Create the void radial objects
  index=buildIndex+1000;
  int prevIndex(-(buildIndex+2));  // trick to get buildIndex+3 surface
  std::string prevWedge;
  for (size_t i=0;i<nWedges;i++)
    {
      Out= ModelSupport::getComposite(SMap,index,prevIndex," 1 -1M ")+
        "("+wedges[i]->getLinkString(2)+":"+
        wedges[i]->getLinkString(4)+")"+prevWedge;
      
      System.addCell(MonteCarlo::Object(cellIndex++,
                                       MatInfo.first,MatInfo.second,
                                       Out+baseOut));
      prevWedge=" ("+wedges[i]->getLinkString(2)+":"+
        wedges[i]->getLinkString(3)+") ";
      
      prevIndex=index++;
    }
  Out=ModelSupport::getComposite(SMap,index-1,buildIndex," -1 -4M ")
    +prevWedge;

  System.addCell(MonteCarlo::Object(cellIndex++,
                                   MatInfo.first,MatInfo.second,
                                   Out+baseOut));
  
  return;
}
  

void
WedgeFlightLine::createAll(Simulation& System,
			   const attachSystem::FixedComp& originFC,
			   const long int originIndex,
			   const attachSystem::FixedComp& innerFC,
			   const long int innerIndex,
			   const attachSystem::FixedComp& outerFC,
			   const long int outerIndex)
  /*!
    Global creation of the basic flight line connecting two
    objects
    \param System :: Simulation to add vessel to
    \param originFC :: Origin
    \param originIndex :: Use side index from Origin
    \param innerFC :: Moderator Object
    \param innerIndex :: Use side index from moderator
    \param outerFC :: Edge of bulk shield 
    \param outerIndex :: Side index of bulk shield

  */
{
  ELog::RegMethod RegA("WedgeFlightLine","createAll");

  moderatorSystem::BasicFlightLine::createAll(System,
                                              originFC, originIndex,
                                              innerFC, innerIndex,
                                              outerFC, outerIndex);
  populate(System.getDataBase());
  buildWedges(System,innerFC,innerIndex,
	      outerFC,outerIndex);
    
  return;
}



  
}  // NAMESPACE essSystem
