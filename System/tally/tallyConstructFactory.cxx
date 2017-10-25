/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/tallyConstructFactory.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Tally.h"
#include "surfaceTally.h"
#include "cellFluxTally.h"
#include "pointTally.h"
#include "heatTally.h"
#include "tallyFactory.h"

#include "pointConstruct.h" 
#include "gridConstruct.h"
#include "meshConstruct.h"
#include "tmeshConstruct.h"
#include "fmeshConstruct.h" 
#include "fluxConstruct.h" 
#include "heatConstruct.h" 
#include "itemConstruct.h" 
#include "surfaceConstruct.h" 
#include "fissionConstruct.h"
#include "sswConstruct.h" 
#include "tallyConstructFactory.h"

namespace tallySystem
{

pointConstruct*
tallyConstructFactory::makePoint() const
  /*!
    Return point tally
    \return pointConstruct
  */
{
  return new pointConstruct();
}

gridConstruct*
tallyConstructFactory::makeGrid() const
  /*!
    Return grid tally
    \return gridConstruct
  */
{
  return new gridConstruct();
}

tmeshConstruct*
tallyConstructFactory::makeTMesh() const
  /*!
    Return tmesh tally
    \return tmeshConstruct
  */
{
  return new tmeshConstruct();
}

fmeshConstruct*
tallyConstructFactory::makeFMesh() const
  /*!
    Return tmesh tally
    \return tmeshConstruct
  */
{
  return new fmeshConstruct();
}

fluxConstruct*
tallyConstructFactory::makeFlux() const
  /*!
    Return flux tally
    \return fluxConstruct
  */
{
  return new fluxConstruct();
}

heatConstruct*
tallyConstructFactory::makeHeat() const
  /*!
    Return Heat tally
    \return HeatConstruct
  */
{
  return new heatConstruct();
}

itemConstruct*
tallyConstructFactory::makeItem() const
  /*!
    Return item tally
    \return itemConstruct
  */
{
  return new itemConstruct();
}

surfaceConstruct*
tallyConstructFactory::makeSurf() const
  /*!
    Return surf tally
    \return surfConstruct
  */
{
  return new surfaceConstruct();
}

fissionConstruct*
tallyConstructFactory::makeFission() const
  /*!
    Return flux tally
    \return fluxConstruct
  */
{
  return new fissionConstruct();
}
 
sswConstruct*
tallyConstructFactory::makeSSW() const
  /*!
    Return SSW tally constructor
    \return sswConstruct
  */
{
  return new sswConstruct();
}

} // NAMESPACE tallySystem
