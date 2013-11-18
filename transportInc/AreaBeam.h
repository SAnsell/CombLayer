/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   transportInc/AreaBeam.h
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
#ifndef Transport_AreaBeam_h
#define Transport_AreaBeam_h

namespace Transport
{

/*!  
  \class AreaBeam
  \brief   Top class for a source
  \version 1.0
  \author S. Ansell
  \date December 2009

  The beam is generate on a point at X,Y,Z
  - X :: Width across sample
  - Y :: Distance along beam (0 == sample centre)
  - Z :: Height (0 == Middle of beam)
*/

class AreaBeam : public Beam
{
 private:   

  double wavelength;        ///< Wavelength [A]
  double Width;             ///< Width [cm]
  double Height;            ///< Height [cm]
  double startY;            ///< Initial position [cm]

 public:
  
  AreaBeam();
  AreaBeam(const AreaBeam&);
  AreaBeam& operator=(const AreaBeam&);
  /// Clone constructor
  virtual Beam* clone() const { return new AreaBeam(*this); }
  virtual ~AreaBeam();

  /// Effective typeid
  virtual std::string className() const { return "AreaBeam"; }
  /// Visitor Acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }

  /// Set Wavelength
  virtual void setWavelength(const double W) { wavelength=W; }

  void setWidth(const double W) { Width=W; }      ///< Set width
  void setHeight(const double H) { Width=H; }     ///< Set height
  void setStart(const double Y) { startY=Y; }     ///< Set initial position 
  void setBias(const double) {};

  double height() const { return Height; }        ///< Get height
  double width() const { return Width; }          ///< Get width
  double wave() const { return wavelength; }      ///< Get wavelength
  double getStart() const { return startY; }      ///< get Initial position


  virtual MonteCarlo::neutron generateNeutron() const;

  // Output stuff
  void write(std::ostream&) const;

};

}  // NAMESPACE Transport

#endif
