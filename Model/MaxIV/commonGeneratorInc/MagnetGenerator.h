/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/MagnetGenerator.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#ifndef setVariable_MagnetGenerator_h
#define setVariable_MagnetGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class MagnetGenerator
  \version 1.0
  \author Stuart Ansell
  \date September 2020
  \brief MagnetGenerator for variables
*/

class MagnetGenerator
{
 private:

  std::string preName;  ///< Pre-name (L2SPF/TDC)

  Geometry::Vec3D xyzStep;         ///< One-time x,y,z step
  
  double length;          ///< length of magnetic unit 
  double width;           ///< width of magnetic unit
  double height;          ///< height of magnetic unit

  std::array<double,4> KFactor;  ///< Magnet units

  std::set<std::string> activeCells;     /// active cells

 public:

  MagnetGenerator();
  MagnetGenerator(const MagnetGenerator&);
  MagnetGenerator& operator=(const MagnetGenerator&);
  virtual ~MagnetGenerator();

  /// Set the prename (L2SPF etc)
  void setPreName(const std::string& N) { preName=N;}
  void setOffset(const double,const double,const double);
  void setOffset(const Geometry::Vec3D&);
  void setSize(const double,const double,const double);
  void setField(const double,const double,const double,const double);

  void generate(FuncDataBase&,const std::string&,
		const std::string&,const std::string&,
		const double);

  void generateCorMag(FuncDataBase&,const size_t,
		      const std::string&,const double,
		      const double = 0.0);

  void generateDipole(FuncDataBase&,const size_t,
		      const std::string&,
		      const double,const double);
  void generateQuad(FuncDataBase&,const size_t,
		    const std::string&,
		    const double,const double);
  void generateSexupole(FuncDataBase&,const size_t,
			const std::string&,
			const double,const double);

  void generate(FuncDataBase&,const size_t,
		const std::string&,const double);

};

}

#endif
