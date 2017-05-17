/****************************************************************************
**
** This file is part of the LibreCAD project, a 2D CAD program
**
** Copyright (C) 2010 R. van Twisk (librecad@rvt.dds.nl)
** Copyright (C) 2001-2003 RibbonSoft. All rights reserved.
**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file gpl-2.0.txt included in the
** packaging of this file.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
** This copyright notice MUST APPEAR in all copies of the script!
**
**********************************************************************/


#ifndef RS_DIMORDINATE_H
#define RS_DIMORDINATE_H

#include "rs_dimension.h"

/**
 * Holds the data that defines an ordinate dimension entity.
 */
struct RS_DimOrdinateData {
	/**
	 * Default constructor
	 */
    RS_DimOrdinateData();
	/**
	 * Constructor with initialisation.
	 *
		* @para extensionPoint1 Definition point. Startpoint of the
	 *         first extension line.
		* @para extensionPoint2 Definition point. Startpoint of the
	 *         second extension line.
	 */
    RS_DimOrdinateData(const RS_Vector& extensionPoint1,
					  const RS_Vector& extensionPoint2);

	/** Definition point. Startpoint of the first extension line. */
	RS_Vector extensionPoint1;
	/** Definition point. Startpoint of the second extension line. */
	RS_Vector extensionPoint2;
};

std::ostream& operator << (std::ostream& os, const RS_DimOrdinateData& dd);

/**
 * Class for ordinate dimension entities.
 *
 * @author Andrew Mustun
 */
class RS_DimOrdinate : public RS_Dimension {
public:
    RS_DimOrdinate(RS_EntityContainer* parent,
                  const RS_DimensionData& d,
                  const RS_DimOrdinateData& ed);

	RS_Entity* clone() const override;

    /**	@return RS2::EntityDimOrdinate */
	RS2::EntityType rtti() const override{
        return RS2::EntityDimOrdinate;
    }

    /**
     * @return Copy of data that defines the ordinate dimension.
     * @see getData()
     */
    RS_DimOrdinateData const& getEData() const;

	RS_VectorSolutions getRefPoints() const override;

	QString getMeasuredLabel() override;

	void updateDim(bool autoText=false) override;

	RS_Vector const& getExtensionPoint1() const;

	RS_Vector const& getExtensionPoint2() const;

    /**
     * Recalculate the original Dimension Point to remove Dim oblique angle.
     * @author Rallaz
     */
	void updateDimPoint();

	void move(const RS_Vector& offset) override;
	void rotate(const RS_Vector& center, const double& angle) override;
	void rotate(const RS_Vector& center, const RS_Vector& angleVector) override;
	void scale(const RS_Vector& center, const RS_Vector& factor) override;
	void mirror(const RS_Vector& axisPoint1, const RS_Vector& axisPoint2) override;
	bool hasEndpointsWithinWindow(const RS_Vector& v1, const RS_Vector& v2) override;
	void stretch(const RS_Vector& firstCorner,
                         const RS_Vector& secondCorner,
						 const RS_Vector& offset) override;
	void moveRef(const RS_Vector& ref, const RS_Vector& offset) override;

    friend std::ostream& operator << (std::ostream& os,
                                      const RS_DimOrdinate& d);

protected:
    /** Extended data. */
    RS_DimOrdinateData edata;
};

#endif
