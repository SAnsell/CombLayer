/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/MeshCreate.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <complex> 
#include <vector>
#include <list> 
#include <map> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <boost/functional.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

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
#include "NList.h"
#include "NRange.h"
#include "pairRange.h"
#include "Tally.h"
#include "Surface.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "Source.h"
#include "Simulation.h"
#include "meshTally.h"
#include "MeshCreate.h"

namespace tallySystem
{

void 
rectangleMesh(Simulation& System,const int type,
	      const std::string& KeyWords,
	      const Geometry::Vec3D& APt,
	      const Geometry::Vec3D& BPt,
	      const int* MPts)
  /*!
    An amalgamation of values to determine what sort of mesh to put
    in the system.
    \param System :: Simulation to add tallies
    \param type :: type of tally[1,2,3]
    \param KeyWords :: KeyWords to add to the tally
    \param APt :: Lower point 
    \param BPt :: Upper point 
    \param MPts :: Points ot use
  */
{
  ELog::RegMethod RegA("MeshCreate","meshSection");

  // Find next available number
  int tallyN(type);
  while(System.getTally(tallyN))
    tallyN+=10;

  // Create tally:
  meshTally MT(tallyN);
  MT.setParticles("n");
  ELog::EM<<"Coordinates ="<<APt<<" "<<BPt<<ELog::endTrace;
  MT.setCoordinates(APt,BPt);
  MT.setIndex(MPts);
  MT.setActive(1);
  if (KeyWords=="DOSE")
    {
      MT.setKeyWords("DOSE 1");
      MT.setResponse(getDoseConversion());
    }
  if (KeyWords=="InternalDOSE")
    {
      MT.setKeyWords("DOSE");
      MT.setIndexLine("40 1 2 1e6");
    }

  ELog::EM<<"Adding tally ";
  MT.write(ELog::EM.Estream());
  ELog::EM<<ELog::endTrace;
  System.addTally(MT);

  return;
}

const std::string& 
getDoseConversion()
/*!
  Return the dose string  for a mshtr
  Uses the FCD files values
  \return FCD string
  */
{
  static std::string fcdString=
    " 0.         1.8864E-03  1.0000E-09  1.8864E-03  1.0000E-08  2.3580E-03 "
    "2.5000E-08  2.7360E-03  1.0000E-07  3.5820E-03  2.0000E-07  4.0320E-03 "
    "5.0000E-07  4.6080E-03  1.0000E-06  4.9680E-03  2.0000E-06  5.2200E-03 "
    "5.0000E-06  5.4000E-03  1.0000E-05  5.4360E-03  2.0000E-05  5.4360E-03 "
    "5.0000E-05  5.3280E-03  1.0000E-04  5.2560E-03  2.0000E-04  5.1840E-03 "
    "5.0000E-04  5.1120E-03  1.0000E-03  5.1120E-03  2.0000E-03  5.1840E-03 "
    "5.0000E-03  5.6520E-03  1.0000E-02  6.5880E-03  2.0000E-02  8.5680E-03 "
    "3.0000E-02  1.0440E-02  5.0000E-02  1.3860E-02  7.0000E-02  1.6992E-02 "
    "1.0000E-01  2.1528E-02  1.5000E-01  2.8872E-02  2.0000E-01  3.5640E-02 "
    "3.0000E-01  4.7880E-02  5.0000E-01  6.7680E-02  7.0000E-01  8.3160E-02 "
    "9.0000E-01  9.6120E-02  1.0000E+00  1.0152E-01  1.2000E+00  1.1160E-01 "
    "2.0000E+00  1.3788E-01  3.0000E+00  1.5552E-01  4.0000E+00  1.6488E-01 "
    "5.0000E+00  1.7064E-01  6.0000E+00  1.7388E-01  7.0000E+00  1.7640E-01 "
    "8.0000E+00  1.7784E-01  9.0000E+00  1.7892E-01  1.0000E+01  1.7964E-01 "
    "1.2000E+01  1.7964E-01  1.4000E+01  1.7856E-01  1.5000E+01  1.7784E-01 "
    "1.6000E+01  1.7676E-01  1.8000E+01  1.7496E-01  2.0000E+01  1.7280E-01 "
    "3.0000E+01  1.6488E-01  5.0000E+01  1.5732E-01  7.5000E+01  1.5444E-01 "
    "1.0000E+02  1.5444E-01  1.3000E+02  1.5552E-01  1.5000E+02  1.5768E-01 "
    "1.8000E+02  1.6020E-01  2.0000E+02  1.7028E-01  5.0000E+02  2.2680E-01 "
    "1.0000E+03  3.4920E-01  2.0000E+03  5.0724E-01  ";

  return fcdString;
}

}  // NAMESPACE tallySystem
