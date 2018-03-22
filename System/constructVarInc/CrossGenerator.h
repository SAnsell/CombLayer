/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/CrossGenerator.h
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
#ifndef setVariable_CrossGenerator_h
#define setVariable_CrossGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class CrossGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief CrossGenerator for variables
*/

class CrossGenerator
{
 private:

  double wallThick;          ///< Wall thick 
  double topThick;           ///< Top thick 
  double baseThick;          ///< Base thick 

  double frontLen;           ///< Length from centre to port end
  double backLen;            ///< Length from centre to port end
    
  double flangeRadius;       ///< Flange radius
  double flangeLen;          ///< Flange length
    
  std::string voidMat;          ///< void material [void]
  std::string wallMat;          ///< wall material

 public:

  CrossGenerator();
  CrossGenerator(const CrossGenerator&);
  CrossGenerator& operator=(const CrossGenerator&);
  ~CrossGenerator();

  void setPlates(const double,const double,const double);
  void setPorts(const double,const double);
  void setFlange(const double,const double);
  /// set void material
  void setVoidMat(const std::string& M) { voidMat=M; }
  /// set pipe material
  void setMat(const std::string& M) { wallMat=M; }

  
  void generateCross(FuncDataBase&,const std::string&,
		     const double,const double,const double,
		     const double,const double) const;

  template<typename HCF>
  void generateCF(FuncDataBase&,const std::string&,
		  const double,const double,
		  const double,const double);
  template<typename HCF,typename VCF>
  void generateDoubleCF(FuncDataBase&,const std::string&,
			const double,const double,const double);

};

}

#endif
 
