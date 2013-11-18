/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geometry/surfProg.cxx
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
#include <sstream>
#include <cmath>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <boost/regex.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "regexSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "surfProg.h"


namespace Geometry
{

namespace surfProg
{

int
possibleCone(const std::string& Line)
  /*! 
     Checks to see if the input string is a cone.
     Valid input is: 
     - number {transformNumber} c/x cen_x cen_y cen_z radius 
     - number {transformNumber} cx radius 
     \retval 1 :: all parts for a cone
     \retval -1 :: extension possible
     \retval 0 :: no a Cone object 
  */
{
  // Split line
  boost::regex divSea("\\s*(\\S+)");
  std::vector<std::string> Items=StrFunc::StrParts(Line,divSea);
  if (Items.size()<3)           //Indecyferable line
    return 0;

  unsigned int ix(1);

  if (tolower(Items[ix][0])!='k' &&         //Not a cone
      tolower(Items[++ix][0])!='k')
    return 0;                    

  if (Items[ix].length()<2)        // too short
    return 0;
  if (tolower(Items[ix][1])>='x' && 
      tolower(Items[ix][1])<='z')   // Simple cone ?
    {
      return (Items.size()>ix+1) ? 1 : -1;     
    }

  // need 3 characters for c/? 
  if (Items[ix].length()<3)
    return 0;
  if (tolower(Items[ix][2])>='x' && 
      tolower(Items[ix][2])<='z')   // Simple cone ?
    {
      return (Items.size()>ix+1) ? 1 : -1;     
    }

  return (Items.size()>ix+3) ? 1 : -1;          // k/x cone
}

int 
possibleCylinder(const std::string& Line)
  /*! 
     Checks to see if it is a cylinder 
     \param Line : Partial input line
     Valid input is:
     - number {transformNumber} c/x cen_y cen_z Radius
     - number {transformNumber} cx Radius
     \retval 1 if all parts for a cylinder
     \retval -1 if extension possible
  */
{
  // Split line
  boost::regex divSea("\\s*(\\S+)");
  std::vector<std::string> Items=StrFunc::StrParts(Line,divSea);
  if (Items.size()<3)           //Indecyferable line
    return 0;

  unsigned int ix(1);

  if (tolower(Items[ix++][0])!='c' &&         //Not a cyclinder
      tolower(Items[ix++][0])!='c')
    return 0;                    
  ix--;

  // need to test c/x and cx type
  // length must be either 2 or 3
  const unsigned long int sx=Items[ix].length()-1;
  if (sx!=1 && sx!=2)
    return 0;
  // now check last char == x,y,z 
  if (tolower(Items[ix][sx])>='x' && 
      tolower(Items[ix][sx])<='z')   // Simple cylinder ?
    {
      return (Items.size()>ix+1) ? 1 : -1;     
    }
  // ok definately failed.
  return 0;
}

int
possibleGeneral(const std::string& Line)
  /*!
    Checks to see if it is a general object
    \retval 1 if all parts for a general object exist
    \retval 0 if not a general surface
    \retval -1 if extension possible to Line
  */
{
  boost::regex divSea("\\s*(\\S+)");
  std::vector<std::string> Items=StrFunc::StrParts(Line,divSea);
  if (Items.size()<3)           //Indecyferable line
    return 0;
  
  size_t ix(1);
  size_t gtype(0);

  for(;!gtype && ix<3;ix++)
    {
      if (Items[ix].size()==2 && tolower(Items[ix][1])=='q')
	{
	  if (tolower(Items[ix][0])=='s')
	    gtype=1;
	  if (tolower(Items[ix][0])=='g')
	    gtype=2;
	}
    }
  
  if (!gtype)         // not a general surface
    return 0;
  
  return (Items.size()>=ix+gtype+8) ? 1 : -1;
}

int
possibleMBrect(const std::string& Line)
  /*!
     Checks to see if the string is sufficient
     and appropiate to be a plane 
     \param Line :: Line to process
     \retval 1 :: all parts for a box
     \retval -1 :: extension possible 
  */
{
  ELog::RegMethod RegA("surfProg","possibleMBrect");

  // Split line
  boost::regex divSea("\\s*(\\S+)");
  std::vector<std::string> Items=StrFunc::StrParts(Line,divSea);
  if (Items.size()<3)           // Indecyferable lineSu
    return 0;

  size_t ix(0);
  
  size_t boxType(0);
  while(!boxType && ix<2)
    {
      ix++;
      if (Items[ix]=="rpp" || Items[ix]=="RPP")
	boxType=6;
      
      if (Items[ix]=="box" || Items[ix]=="BOX")
	boxType=12;
    }

  return (boxType && Items.size()>=ix+boxType) ? 1 : -1;
}

int
possiblePlane(const std::string& Line)
  /*!
     Checks to see if the string is sufficient
     and appropiate to be a plane 
     \param Line :: Line to check [num {tr} px data]
     \retval 1 :: all parts for a plane
     \retval -1 :: extension possible 
  */
{
  // Split line
  boost::regex divSea("\\s*(\\S+)");
  std::vector<std::string> Items=StrFunc::StrParts(Line,divSea);
  if (Items.size()<2)           //Indecyferable lineSu
    return 0;

  unsigned int ix(1);

  if (tolower(Items[ix++][0])!='p' &&         //Not a plane
      tolower(Items[ix++][0])!='p')
    return 0;                    
  ix--;
  if (Items[ix][1]>='x' && Items[ix][1]<='z')       //Simple plane ?
    {
      return (Items.size()>ix+1) ? 1 : -1;
    }
  if (Items.size()==ix+5 || Items.size()==ix+10)
    return 1;
  if (Items.size()<10)
    return -1;
  return 0;
}
 

int
possibleSphere(const std::string& Line)
  /*!
     Checks to see the string is a sphere entry
     from MCNPX
     \param Line :: String to check
     \retval 1  :: all parts for a sphere
     \retval 0 :: is not a shere
     \retval -1 :: extension possible
  */
{
  // Split line
  boost::regex divSea("\\s*(\\S+)");
  std::vector<std::string> Items=StrFunc::StrParts(Line,divSea);
  if (Items.size()<3)           //Indecypherable line
    return 0;

  unsigned int ix(1);


  // Such that 
  // A so X  :: ix == 2
  // A T so Y :: ix == 3
  if (tolower(Items[ix][0])!='s' &&         //Not a sphere  
      tolower(Items[++ix][0])!='s')
    return 0;              

  const int cx=tolower(Items[ix][1]);
  switch (cx)
    {
    case 'o':             // origin (+1 item)
      return (Items.size()>ix+1) ? 1 : -1;

    case '\0':            // general sphere (x,y,z R)
      return (Items.size()>ix+4) ? 1 : -1;
     
    default:        //eq 'q' for ellipse
      return 0;
    }

  return 0;
}

int 
possibleTorus(const std::string& Line)
  /*! 
     Checks to see if the input string is a torus.
     Valid input is: 
     - number {transformNumber} tz cen_x cen_y cen_z A B C
     \retval 1 :: all parts for a torus
     \retval -1 :: extension possible
     \retval 0 :: no a Torus object 
  */
{
  // Split line
  boost::regex divSea("\\s*(\\S+)");
  std::vector<std::string> Items=StrFunc::StrParts(Line,divSea);
  if (Items.size()<3)           //Indecyferable line
    return 0;

  unsigned int ix(1);

  if (tolower(Items[ix][0])!='t' &&         //Not a cone
      tolower(Items[++ix][0])!='t')
    return 0;                    

  if (Items[ix].length()<2)        // too short
    return 0;

  if (tolower(Items[ix][1])>='x' && 
      tolower(Items[ix][1])<='z')   // Simple  ?
    {
      return (Items.size()>ix+1) ? 1 : -1;     
    }

  return 0;
}


}  // NAMESPACE surProg

}  // NAMESPACE Geometry
