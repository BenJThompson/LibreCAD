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
#include<cmath>
#include<vector>
#include <QAction>
#include <QMouseEvent>
#include "rs_actiondimordinate.h"
#include "rs_dimordinate.h"

#include "rs_dialogfactory.h"
#include "rs_line.h"
#include "rs_graphicview.h"
#include "rs_commandevent.h"
#include "rs_constructionline.h"
#include "rs_coordinateevent.h"
#include "rs_preview.h"
#include "rs_debug.h"


struct RS_ActionDimOrdinate::Points {
std::vector<RS_Vector> points;
};


RS_ActionDimOrdinate::RS_ActionDimOrdinate(RS_EntityContainer& container,
        RS_GraphicView& graphicView)
        :RS_ActionDimension("Draw ordinate dimensions",
                    container, graphicView)
    , pPoints(new Points{})
    {
    actionType=RS2::ActionDimOrdinate;
	reset();
}


RS_ActionDimOrdinate::~RS_ActionDimOrdinate()  = default;


void RS_ActionDimOrdinate::reset() {
    RS_ActionDimension::reset();

    edata.reset(new RS_DimOrdinateData(RS_Vector(false),
							  RS_Vector(false))
				);
    lastStatus = SetOriginPoint;
	RS_DIALOGFACTORY->requestOptions(this, true, true);
    pPoints->points.clear();
}


void RS_ActionDimOrdinate::init(int status) {
     RS_PreviewActionInterface::init(status);

    reset();
}


void RS_ActionDimOrdinate::trigger() {
    RS_ActionDimension::trigger();

    preparePreview();
    graphicView->moveRelativeZero(data->definitionPoint);

		//data->text = getText();
    RS_DimOrdinate* dim =
        new RS_DimOrdinate(container, *data, *edata);
    dim->setLayerToActive();
    dim->setPenToActive();
    dim->update();
    container->addEntity(dim);

    // upd. undo list:
    if (document) {
        document->startUndoCycle();
        document->addUndoable(dim);
        document->endUndoCycle();
    }

    RS_Vector rz = graphicView->getRelativeZero();
        graphicView->redraw(RS2::RedrawDrawing);
    graphicView->moveRelativeZero(rz);

    RS_DEBUG->print("RS_ActionDimOrdinate::trigger():"
                    " dim added: %d", dim->getId());
}


//this is drawing
void RS_ActionDimOrdinate::preparePreview() {
	RS_Vector dirV = RS_Vector::polar(100.,
				  edata->extensionPoint1.angleTo(
					  edata->extensionPoint2)
				  +M_PI_2);
	RS_ConstructionLine cl(nullptr,
                           RS_ConstructionLineData(
							   edata->extensionPoint2,
							   edata->extensionPoint2+dirV));

    data->definitionPoint =
        cl.getNearestPointOnEntity(data->definitionPoint);
}


//these are events
void RS_ActionDimOrdinate::mouseMoveEvent(QMouseEvent* e) {
    RS_DEBUG->print("RS_ActionDimOrdinate::mouseMoveEvent begin");

    RS_Vector mouse = snapPoint(e);

    switch (getStatus()) {
    case SetOriginPoint:
        break;

    case SetExtPoint:
		if (edata->extensionPoint1.valid) {
            deletePreview();
            preview->addEntity(
				new RS_Line{preview.get(),edata->extensionPoint1, mouse}
            );
            drawPreview();
        }
        break;

    case SetDefPoint:
		if (edata->extensionPoint1.valid && edata->extensionPoint2.valid) {
            deletePreview();
			data->definitionPoint = mouse;

            preparePreview();

						//data->text = getText();
            RS_DimOrdinate* dim = new RS_DimOrdinate(preview.get(), *data, *edata);
            preview->addEntity(dim);
            dim->update();
            drawPreview();
        }
        break;

        default:
                break;
    }

    RS_DEBUG->print("RS_ActionDimOrdinate::mouseMoveEvent end");
}



void RS_ActionDimOrdinate::mouseReleaseEvent(QMouseEvent* e) {
    if (e->button()==Qt::LeftButton) {
        RS_CoordinateEvent ce(snapPoint(e));
        coordinateEvent(&ce);
    } else if (e->button()==Qt::RightButton) {
        deletePreview();
        init(getStatus()-1);
    }
}


//look here
void RS_ActionDimOrdinate::keyPressEvent(QKeyEvent* e) {
    if (getStatus()==SetOriginPoint && e->key()==Qt::Key_Enter) {
        trigger();
        reset();
        setStatus(SetDefPoint);
    }
}

//and here
void RS_ActionDimOrdinate::coordinateEvent(RS_CoordinateEvent* e) {
    if (!e) return;

    RS_Vector pos = e->getCoordinate();

    switch (getStatus()) {
    case SetOriginPoint:
        edata->extensionPoint1 = pos;
        graphicView->moveRelativeZero(pos);
        setStatus(SetExtPoint);
        break;

    case SetExtPoint:
		edata->extensionPoint2 = pos;
        graphicView->moveRelativeZero(pos);
        setStatus(SetDefPoint);
        break;

    case SetDefPoint:
        data->definitionPoint = pos;
        trigger();
        reset();
        setStatus(SetExtPoint);
        break;

    default:
        break;
    }
}



void RS_ActionDimOrdinate::commandEvent(RS_CommandEvent* e) {
    QString c = e->getCommand().toLower();

    if (checkCommand("help", c)) {
		RS_DIALOGFACTORY->commandMessage(msgAvailableCommands()
										 + getAvailableCommands().join(", "));
        return;
    }

    switch (getStatus()) {
    case SetText: {
            setText(c);
			RS_DIALOGFACTORY->requestOptions(this, true, true);
            setStatus(lastStatus);
            graphicView->enableCoordinateInput();
        }
        break;

    default:
        if (checkCommand("text", c)) {
            lastStatus = (Status)getStatus();
            graphicView->disableCoordinateInput();
            setStatus(SetText);
        }
        break;
    }
}



QStringList RS_ActionDimOrdinate::getAvailableCommands() {
    QStringList cmd;

    switch (getStatus()) {
    case SetOriginPoint:
    case SetExtPoint:
    case SetDefPoint:
        cmd += command("text");
        break;

    default:
        break;
    }

    return cmd;
}


void RS_ActionDimOrdinate::hideOptions() {
    RS_DIALOGFACTORY->requestOptions(this, false);

    RS_ActionDimension::hideOptions();
}


void RS_ActionDimOrdinate::showOptions() {
    RS_ActionDimension::showOptions();

    RS_DIALOGFACTORY->requestOptions(this, true);
}


void RS_ActionDimOrdinate::updateMouseButtonHints() {
    switch (getStatus()) {
    case SetOriginPoint:
		RS_DIALOGFACTORY->updateMouseWidget(
                    tr("Set the origin point"),
					tr("Cancel"));
		break;
    case SetExtPoint:
		RS_DIALOGFACTORY->updateMouseWidget(
                    tr("Set the point"),
					tr("Back"));
		break;
    case SetDefPoint:
        RS_DIALOGFACTORY->updateMouseWidget(
                    tr("Place the dimention text"),
                    tr("Back"));
        break;
    case SetText:
		RS_DIALOGFACTORY->updateMouseWidget(tr("Enter dimension text:"), "");
		break;
	default:
		RS_DIALOGFACTORY->updateMouseWidget();
		break;
	}
}


void RS_ActionDimOrdinate::updateMouseCursor() {
    graphicView->setMouseCursor(RS2::CadCursor);
}

// EOF
