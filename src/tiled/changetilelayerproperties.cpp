/*
 * changetilelayerproperties.cpp
 * Copyright 2010, Jeff Bland <jksb@member.fsf.org>
 * Copyright 2010, Thorbjørn Lindeijer <thorbjorn@lindeijer.nl>
 * Copyright 2011, Gregory Nickonov <gregory@nickonov.ru>
 *
 * This file is part of Tiled.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "changetilelayerproperties.h"

#include "mapdocument.h"
#include "tilelayer.h"

#include <QCoreApplication>
#include <QDebug>

using namespace Tiled;
using namespace Tiled::Internal;

ChangeTileLayerProperties::ChangeTileLayerProperties(
        MapDocument *mapDocument,
        TileLayer *tileLayer,
        const QColor &color)
    : QUndoCommand(
          QCoreApplication::translate(
              "Undo Commands", "Change Tile Layer Properties"))
    , mMapDocument(mapDocument)
    , mTileLayer(tileLayer)
    , mUndoColor(tileLayer->color())
    , mRedoColor(color)
{
}

void ChangeTileLayerProperties::redo()
{
    qDebug() << "Redoing tile layer properties change!";
    mTileLayer->setColor(mRedoColor);

    mMapDocument->emitTileLayerChanged(mTileLayer);
}

void ChangeTileLayerProperties::undo()
{
    mTileLayer->setColor(mUndoColor);

    mMapDocument->emitTileLayerChanged(mTileLayer);
}

