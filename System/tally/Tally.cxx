/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/Tally.cxx
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
#include <sstream>
#include <fstream>
#include <complex>
#include <cmath>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "Tally.h"

namespace tallySystem
{

std::ostream&
operator<<(std::ostream& OX,const Tally& TX)
  /*!
    Generic writing function
    \param OX :: Output stream
    \param TX :: Tally
    \return Output stream
   */
{
  TX.write(OX);
  return OX;
}

Tally::Tally(const int ID)  :
  active(1),rotRNumFlag(0),IDnum(ID)
  /*!
    Constructor 
    \param ID :: Tally ID number
  */
{}

Tally::Tally(const Tally& A)  :
  active(A.active),rotRNumFlag(A.rotRNumFlag),
  printField(A.printField),Etab(A.Etab),
  cosTab(A.cosTab),timeTab(A.timeTab),
  fModify(A.fModify),Comment(A.Comment),
  Treatment(A.Treatment),IDnum(A.IDnum),
  Particles(A.Particles)
  /*!
    Copy constructor
    \param A :: Tally object to copy
  */
{}

Tally*
Tally::clone() const
  /*!
    Basic clone class
    \return new this
  */
{
  return new Tally(*this);
}

Tally&
Tally::operator=(const Tally& A) 
  /*!
    Assignment operator 
    \param A :: Tally object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      active=A.active;
      rotRNumFlag=A.rotRNumFlag;
      printField=A.printField;
      Etab=A.Etab;
      cosTab=A.cosTab;
      timeTab=A.timeTab;
      fModify=A.fModify;
      Comment=A.Comment;
      Treatment=A.Treatment;
      IDnum=A.IDnum;
      Particles=A.Particles;
    }
  return *this;
}

Tally::~Tally()
 /*!
   Destructor
 */
{}

int
Tally::getID(const std::string& Line) 
  /*!
    Function to determine the id from the line
    of type xxNN.
    \param Line :: Line to check
    \retval -1 :: Empty string
    \retval -2 :: no Numbers
    \retval -3 :: Not a valid number
    \retval ID number [success]
  */
{
  std::string LX;
  if (!StrFunc::convert(Line,LX) || LX.empty())
    return -1;
  std::string::size_type pos=LX.find_first_of("1234567890");
  // no numbers
  if (pos==std::string::npos || pos==0)
    return -2;
  
  LX.erase(0,pos);
  int num;
  if (!StrFunc::sectPartNum(LX,num))
    return -3;

  return num;
}

std::pair<std::string,int>
Tally::getElm(const std::string& Line) 
  /*!
    Function to determine the Elm type from the line
    of type xxNN:yyy 
    \param Line :: Line to check
    \retval <xx,ID>
    \retval <"",-1> Failure
  */
{
  ELog::RegMethod RegA("Tally","getElm");

  typedef std::pair<std::string,int> retType;

  std::string LX;
  if (!StrFunc::convert(Line,LX) || LX.empty())
    return retType("",-1);
  std::string::size_type pos=LX.find_first_of("1234567890");
  // no numbers
  if (pos==std::string::npos || pos==0)
    return retType("",-1);

  retType Out;
  Out.first=LX.substr(0,pos);
  LX.erase(0,pos);
  int num;
  // Note :: need to avoid problems with f1:n
  if (!StrFunc::sectPartNum(LX,num))
    return retType("",-1);
  Out.second=num;
  return Out;
}

int
Tally::firstLine(const std::string& Line)
  /*!
    Simple static decider function that determines 
    if Line is the start of a new line 
    or at least not the end of a line
    \param Line :: Line to check
    \retval 0 :: line is not a first line
    \retval -1 :: line is a comment 
    \retval 1 :: new (real) first line
  */
{
  std::string LX;
  if (!StrFunc::convert(Line,LX) || LX.empty())
    return -1;
  
  // c<spc> comment
  if (LX.size()==1 && tolower(LX[0])=='c')
    return -1;
  
  // Expression
  if (LX.find('$')!=std::string::npos)
    return 0;

  // digit
  if (isdigit(LX[0]) || LX[0]=='-')
    return 0;

  // everything else must be an identifier
  return 1;
}

int
Tally::processParticles(std::string& Line)
  /*!
    Process a particle list.
    This is assumed to be :n,b,c,sd, sd,as Number
    \param Line :: list to be changed
    \return number of particles
  */
{
  ELog::RegMethod RegA("Tally","processParticles");

  Particles.clear();
  double testNum;
  std::string::size_type pos=Line.find(":");
  if (pos!=std::string::npos)
    {
      Line.erase(0,pos+1);
      Line=StrFunc::fullBlock(Line);
      pos=Line.find_first_of(", ");
      while(!Line.empty())
        {
	  std::string Unit=Line.substr(0,pos);
	  // Found a number (therefore it is not a particle)
	  if (StrFunc::convert(Unit,testNum))
	    return static_cast<int>(Particles.size());

	  // End of everthing 
	  if (pos==std::string::npos)
	    {
	      Particles.push_back(Line);
	      return static_cast<int>(Particles.size());
	    }
	  Particles.push_back(Line.substr(0,pos));

	  Line.erase(0,pos+1);
	  pos=Line.find_first_not_of(" ,");
	  Line.erase(0,pos);
	  pos=Line.find_first_of(", ");
	}
      // might not be able to get here
      return static_cast<int>(Particles.size());
    }
  return 0;
}

void
Tally::setParticles(const std::string& PLine)
  /*!
    Allows the setting of particles
    \param PLine :: Line of comma separated particles
  */
{
  ELog::RegMethod RegA("Tally","setParticles");
  std::string Lpart=":"+PLine;
  
  processParticles(Lpart);
  return;
}

std::string
Tally::getParticles() const
  /*!
    Access the individual particles
    \return string outside
  */
{
  ELog::RegMethod RegA("Tally","getPartciles");

  std::ostringstream cx; 
  copy(Particles.begin(),Particles.end()-1,
       std::ostream_iterator<std::string>(cx,","));
  cx<<Particles.back();
  return cx.str();
}

int
Tally::setTime(const std::string& TVec)
  /*!
    Set the timeTab line
    \param TVec :: vector of the time
    \return 0 on success/-ve on failure
  */
{
  ELog::RegMethod RegA("Tally","setTime");
  timeTab.clear();
  return timeTab.processString(TVec);
}

int
Tally::setAngle(const std::string& AVec)
  /*!
    Set the cos angular tally
    \param AVec :: vector of the cosine as a string
    \return 0 on success/-ve on failure
  */
{
  ELog::RegMethod RegA("Tally","setAngle");

  cosTab.clear();
  if (cosTab.processString(AVec))
    return (AVec=="empty" || AVec=="Empty") ? 0  : -1;

  std::vector<double> Val;
  cosTab.writeVector(Val);
  if (Val.empty()) return -1;

  std::sort(Val.begin(),Val.end());

  if (Val.front()< -1.0+Geometry::zeroTol)
    Val.erase(Val.begin());

  // convert to cos(degrees): 0 implies 1.0
  if (Val.back()>1.0+Geometry::zeroTol)
    {
      for(double& CN : Val)
        CN=cos(M_PI*CN/180.0);

      std::reverse(Val.begin(),Val.end());
    }
  cosTab.setVector(Val);

  return 0;
}

int
Tally::setComment(const std::string& C)
  /*!
    Set the timeTab line
    \param C :: New comment
    \return 1 on success/0 on failure
  */
{
  Comment=C;
  return 1;
}

int
Tally::setEnergy(const std::string& EVec)
  /*!
    Set the energyTab line
    \param EVec :: vector of the energy
    \return 0 on success/-ve on failure
  */
{
  Etab.clear();
  return Etab.processString(EVec);
}

void
Tally::cutEnergy(const double ECut)
  /*!
    Cut the energy at the cut point
    \param ECut :: Energy to cut
  */
{
  ELog::RegMethod RegA("Tally","cutEnergy");

  std::vector<double> EV;
  Etab.writeVector(EV);
  if (!EV.empty())
    {
      long int index=indexPos(EV,ECut);
      if (index<0 || index>static_cast<long int>(EV.size()))
	return;
      EV.erase(EV.begin(),EV.begin()+index);
      Etab.setVector(EV);
    }
  return;
}

int
Tally::setModify(const std::string& FVec)
  /*!
    Set the timeTab line
    \param FVec :: vector of the modification
    \return 1 on success/0 on failure
  */
{
  fModify.clear();
  return fModify.processString(FVec);
}

int
Tally::setFormat(const std::string& FVec)
  /*!
    Set the timeTab line
    \param FVec :: vector of the modification
    \return 1 on success/0 on failure
  */
{
  // special case for empty:
  if (FVec=="empty" || FVec=="void")
    {
      printField.clear();
      return 1;
    }
  
  const std::vector<std::string> eParts=
    StrFunc::splitParts(FVec,' ');

  std::vector<char> newField;
  for(const std::string& Item : eParts)
    {
      if (Item.size()>1)
        return 0;
      newField.push_back(Item[0]);
    }
  printField=newField;
  return 1;
}

int
Tally::setSpecial(const std::string& SLine)
  /*!
    Set the special line
    \param SLine :: Line to add
    \return 1
  */
{
  Treatment=SLine;
  return 1;
}

int
Tally::setPrintField(std::string PField)
  /*!
    Set the energyTab line
    \param PField :: Full line to be split
    \return number of components.
  */
{
  printField.clear();
  std::string Unit;
  while(StrFunc::section(PField,Unit))
    {
      printField.push_back(static_cast<char>(tolower(Unit[0])));
    }
  return static_cast<int>(printField.size());
}

int
Tally::setSDField(const std::string& NItems)
  /*!
    Set sd field
    \param NItems :: list of items
    \return 1 on success
   */
{
  ELog::RegMethod RegA("Tally","setSDField");
  SDfield.clear();

  if (StrFunc::isEmpty(NItems))
    return 1;
  // Items to process
  
  if (SDfield.processString(NItems))
    {
      ELog::EM<<"Failed to process SDField :"<<NItems<<ELog::endErr;
      return 0;
    }

  return 1;
}
  
int
Tally::setSDField(const double V)
  /*!
    Sets a constant value for all FS fields.
    \param V :: Item to add
    \return success
  */
{
  SDfield.clear();
  SDfield.addComp(V);
  return 1;
}

void
Tally::setCinderEnergy(const std::string&)
  /*!
    Set the energyTab line
    \param :: Particle type [not used]
  */
{
  ELog::RegMethod RegA("Tally","setCinderEnergy");

  const std::vector<double> EPts
    ({ 1.e-11,5.e-9,1.e-8,1.5e-8,2.e-8,2.5e-8,
        3.0e-8,3.5e-8,4.2e-8,5.e-8,5.8e-8,6.7e-8,
        8.0e-8,1.0e-7,1.52e-7,2.51e-7,4.14e-7,6.83e-7,
        1.125e-6,1.855e-6,3.059e-6,5.043e-6,
        8.315e-6,1.371e-5,2.26e-5,3.727e-5,6.144e-5,
        1.013e-4,1.67e-4,2.754e-4,4.54e-4,
        7.485e-4,1.234e-3,2.035e-3,2.404e-3,2.840e-3,
        3.355e-3,5.531e-3,9.119e-3,1.503e-2,
        1.989e-2,2.554e-2,4.087e-2,6.738e-2,1.111e-1,
        1.832e-1,3.02e-1,3.887e-1,4.979e-1,
        0.639279,0.82085,1.10803,1.35335,1.73774,2.2313,
        2.86505,3.67879,4.96585,6.065,
        10.0,14.9182,16.9046,20.0,25.0});

  Etab.setVector(EPts);
  if (Etab.empty())
    ELog::EM<<"Etab empty"<<ELog::endErr;
  return;
}

int
Tally::addLine(const std::string& LX)
  /*!
    Adds a specific string
    \param LX :: section to process
    \retval -1 :: ID not correct
    \retval -2 :: Type object not correct
    \retval -3 :: Line not correct
    \retval 0 :: Line successful
  */
{
  std::string Line(LX);
  std::string Tag;
  if (!StrFunc::section(Line,Tag))
    return -1;

  // get tag+ number
  std::pair<std::string,int> FUnit=Tally::getElm(Tag);
  if (FUnit.second<0)
    return -2;
  
  if (FUnit.first=="e")             /// Energy
    return Etab.processString(Line);

  if (FUnit.first=="c")             /// Cosine
    return cosTab.processString(Line);

  if (FUnit.first=="t")             /// Time
    return timeTab.processString(Line);

  if (FUnit.first=="fm")             /// Modification line
    return fModify.processString(Line);

  if (FUnit.first=="fc")             /// Comment
    {
      Comment=Line;
      return 0;
    }
  if (FUnit.first=="ft")             /// Special treatment
    {
      Treatment=Line;
      return 0;
    }
  
  if (FUnit.first=="f")
    {
      std::string Lpart(LX);
      processParticles(Lpart);
      return 0;
    }

  return -3;
}

void
Tally::write(std::ostream& OX) const
  /*!
    Writes out the tally depending on the 
    fields that have been set.
    \param OX :: Output Stream
  */
{
  if (!active)
    return;
  
  std::stringstream cx;
  if (IDnum)  // maybe default 
    {
      cx<<"f"<<IDnum;
      if (!Particles.empty())
        {
	  cx<<":";
	  copy(Particles.begin(),Particles.end()-1,
	       std::ostream_iterator<std::string>(cx,","));
	  cx<<Particles.back();
	}
      StrFunc::writeMCNPX(cx.str(),OX);
      cx.str("");
    }
  writeFields(OX);
  return;
}

void
Tally::writeParticles(std::ostream& OX) const
  /*!
    Adds the particle stream if required
    \param OX :: Output stream
   */
{			
  if (!Particles.empty())
    {
      OX<<":";
      copy(Particles.begin(),Particles.end()-1,
	   std::ostream_iterator<std::string>(OX,","));
      OX<<Particles.back();
    }
  OX<<" ";
  return;
}

void
Tally::writeFields(std::ostream& OX) const
  /*!
    Writes out the tally field depending on the 
    fields that have been set. It is here
    since we need a way to allow the old
    fields to be written but not mess up the main
    line.
    \param OX :: Output Stream
  */  
{
  std::ostringstream cx;
  if (!active)
    return;

  if (!printField.empty())
    {
      cx<<"fq"<<IDnum<<"   ";
      copy(printField.begin(),printField.end(),
	   std::ostream_iterator<char>(cx," "));
      StrFunc::writeMCNPX(cx.str(),OX);
      cx.str("");
    }
  if (!Treatment.empty())
    {
      cx<<"ft"<<IDnum<<" ";
      cx<<Treatment;      
      StrFunc::writeMCNPX(cx.str(),OX);
      cx.str("");
    }
  
  if (!Comment.empty())
    {
      cx<<"fc"<<IDnum<<" ";
      cx<<Comment;
      StrFunc::writeMCNPX(cx.str(),OX);
      cx.str("");
    }

  if (!Etab.empty())
    {
      cx<<"e"<<IDnum<<"    ";
      cx<<Etab;
      StrFunc::writeMCNPX(cx.str(),OX);
      cx.str("");
    }

  if (!cosTab.empty())
    {
      cx<<"c"<<IDnum<<"     ";
      cx<<cosTab;
      StrFunc::writeMCNPX(cx.str(),OX);
      cx.str("");
    }
  if (!fModify.empty())
    {
      cx<<"fm"<<IDnum<<"     ";
      cx<<fModify;
      StrFunc::writeMCNPX(cx.str(),OX);
      cx.str("");
    }

  if (!timeTab.empty())
    {
      cx<<"t"<<IDnum<<"     ";
      cx<<timeTab;
      StrFunc::writeMCNPX(cx.str(),OX);
      cx.str("");
    }
  if (!SDfield.empty())
    {
      cx<<"sd"<<IDnum<<" "<<SDfield;
      StrFunc::writeMCNPX(cx.str(),OX);
      cx.str("");
    }

  return;
}

}  // NAMESPACE tallySystem
