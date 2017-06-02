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

#ifndef RS_ACTIONDIMORDINATE_H
#define RS_ACTIONDIMORDINATE_H

#include "rs_actiondimension.h"

struct RS_DimOrdinateData;

/**
 * This action class can handle user events to draw
 * ordinate dimensions.
 *
 * @author Ben Thompson
 */
class RS_ActionDimOrdinate : public RS_ActionDimension {
        Q_OBJECT
public:
    /**
     * Action States.
     */
    enum Status {
        SetOriginPoint,    /**< Set the origin point.  */
        SetOriginDefPoint, /**< Set the definite point for the text placement */
        SetExtPoint,       /**< Set the point to be measured */
        SetDefPoint,       /**< Set the definite point for the text placement */
        SetText            /**< Set the text label in command line */
    };

public:
    RS_ActionDimOrdinate(RS_EntityContainer& container,
                        RS_GraphicView& graphicView);
    ~RS_ActionDimOrdinate() override;

	void reset() override;
    void init(int status=0) override;
    void trigger() override;
    void preparePreview();

	void mouseMoveEvent(QMouseEvent* e) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
    void keyPressEvent(QKeyEvent*) override;

    void coordinateEvent(RS_CoordinateEvent* e) override;
	void commandEvent(RS_CommandEvent* e) override;
    QStringList getAvailableCommands() override;

	void hideOptions() override;
	void showOptions() override;

	void updateMouseButtonHints() override;
    void updateMouseCursor() override;

protected:
		/**
     * Ordinate dimension data.
	 */
        std::unique_ptr<RS_DimOrdinateData> edata;

		/** Last status before entering text. */
		Status lastStatus;

        /**
         * Points set so far.
         */
        struct Points;
        std::unique_ptr<Points> pPoints;
}
;

#endif
