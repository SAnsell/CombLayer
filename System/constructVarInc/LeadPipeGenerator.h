/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/LeadPipeGenerator.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef setVariable_LeadPipeGenerator_h
#define setVariable_LeadPipeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class LeadPipeGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief LeadPipeGenerator for variables
*/

class LeadPipeGenerator :
  public SplitPipeGenerator
{
 private:

    
  double radius;               ///< void radius [inner]
  double thick;                ///< pipe thickness
  
  double claddingThick;        ///< cladding thickness
  double claddingStep;         ///< distance from front/back to cladding start    
  double flangeARadius;        ///< Joining FlangeA radius 
  double flangeBRadius;        ///< Joining FlangeB radius 

  double flangeALength;        ///< Joining Flange length
  double flangeBLength;        ///< Joining Flange length


  std::string voidMat;                 ///< Void material
  std::string pipeMat;                 ///< Pipe material
  std::string claddingMat;             ///< Pipe cladding material
  std::string flangeMat;               ///< Flange material
      
 public:

  LeadPipeGenerator();
  LeadPipeGenerator(const LeadPipeGenerator&);
  LeadPipeGenerator& operator=(const LeadPipeGenerator&);
  ~LeadPipeGenerator();

  template<typename CF> void setCF();
  template<typename CF> void setFlangeCF();
  template<typename CF> void setAFlangeCF();
  template<typename CF> void setBFlangeCF();
  
  void setPipe(const double,const double,const double,const double);
  void setFlangeLength(const double,const double);
  void setCladding(const double,const double);
  void setCladdingThick(const double T) { claddingThick=T; }
  void setMat(const std::string&,const std::string&);

  
  void generatePipe(FuncDataBase&,const std::string&,
		    const double) const;

};

}

#endif
 
