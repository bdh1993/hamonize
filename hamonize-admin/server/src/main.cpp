/*
 * main.cpp - main file for Veyon Server
 *
 * Copyright (c) 2006-2019 Tobias Junghans <tobydox@veyon.io>
 *
 * This file is part of Veyon - https://veyon.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include <QCoreApplication>
#include <QApplication>

#include "ComputerControlServer.h"
#include "VeyonConfiguration.h"


int main( int argc, char **argv )
{
//	QCoreApplication app( argc, argv );
    QApplication app( argc, argv );

	VeyonCore core( &app, QStringLiteral("Server") );

	ComputerControlServer server( &core );
	if( server.start() == false )
	{
		vCritical() << "Failed to start server";
		return -1;
	}

	return core.exec();
}
