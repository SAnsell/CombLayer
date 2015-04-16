/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tally/tallyFactory.cxx
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
#include <sstream>
#include <cmath>
#include <string>
#include <vector>
#include <list>
#include <map>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Transform.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Tally.h"
#include "surfaceTally.h"
#include "cellFluxTally.h"
#include "pointTally.h"
#include "heatTally.h"
#include "tallyFactory.h"

namespace tallySystem
{

tallyFactory*
tallyFactory::Instance() 
  /*!
    Effective new command / this command 
    \returns Single instance of tallyFactory
  */
{
  static tallyFactory TItem;
  return &TItem;
}

tallyFactory::tallyFactory() 
  /*!
    Constructor
  */
{
  registerTally();
}

tallyFactory::tallyFactory(const tallyFactory& A) 
  /*! 
    Copy constructor 
    \param A :: Object to copy
  */
{
  MapType::const_iterator vc;
  for(vc=A.TGrid.begin();vc!=A.TGrid.end();vc++)
    TGrid.insert(std::pair<int,Tally*>
		 (vc->first,vc->second->clone()));
}
  
tallyFactory::~tallyFactory()
  /*!
    Destructor removes memory for atom/cluster list
  */
{
  MapType::iterator vc;
  for(vc=TGrid.begin();vc!=TGrid.end();vc++)
    delete vc->second;
}

void
tallyFactory::registerTally()
  /*!
    Register tallies to be used
  */
{
  ELog::RegMethod RegA("tallyFactory","registerTally");

  //  TGrid[0]=new Tally(0);  
  TGrid[1]=new surfaceTally(1);  
  TGrid[4]=new cellFluxTally(4);  
  TGrid[5]=new pointTally(5);
  TGrid[6]=new heatTally(6);  

  return;
}
  
Tally*
tallyFactory::createTally(const int Key) const
  /*!
    Creates an instance of tally
    given a valid key. 
    
    \param Key :: Item to get 
    \throw InContainerError for the key if not found
    \return new tally object.
  */    
{
  ELog::RegMethod RegA("tallyFactory","createTally");

  MapType::const_iterator vc;
  vc=TGrid.find(Key % 10);
  if (vc==TGrid.end())
    throw ColErr::InContainerError<int>(Key,"Unknown tally key type");

  Tally *X= vc->second->clone();
  X->setKey(Key);
  return X;
}
 
 

} // NAMESPACE tallySystem
