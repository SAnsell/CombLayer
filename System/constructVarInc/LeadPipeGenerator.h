/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/LeadPipeGenerator.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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

  double cladStep;              ///< clad step from flange
  double cladThick;             ///< clad thickness.
  
  std::string cladMat;        ///< Primary bellow material
    
 public:

  LeadPipeGenerator();
  LeadPipeGenerator(const LeadPipeGenerator&);
  LeadPipeGenerator& operator=(const LeadPipeGenerator&);
  ~LeadPipeGenerator();

  template<typename CF> void setCF();
  
  void setPipe(const double,const double,const double,const double);
  void setCladding(const double,const double);
  void setCladdingThick(const double T) { cladThick=T; }
  void setMat(const std::string&,const std::string&);

  
  void generateCladPipe(FuncDataBase&,const std::string&,
			const double,const double) const;

};

}

#endif
 
