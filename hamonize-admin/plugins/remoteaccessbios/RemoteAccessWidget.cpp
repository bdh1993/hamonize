/*
 *  RemoteAccessWidget.cpp - widget containing a VNC-view and controls for it
 *
 *  Copyright (c) 2006-2019 Tobias Junghans <tobydox@veyon.io>
 *
 *  This file is part of Veyon - https://veyon.io
 *
 *  This is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 *  USA.
 */

#include <QBitmap>
#include <QLayout>
#include <QMenu>
#include <QPainter>
#include <QPaintEvent>

#include "rfb/keysym.h"

#include "RemoteAccessWidget.h"
#include "VncView.h"
#include "VeyonConnection.h"
#include "Computer.h"
#include "ComputerControlInterface.h"
#include "PlatformCoreFunctions.h"
#include "ToolButton.h"
#include "Screenshot.h"

// toolbar for remote-control-widget
RemoteAccessWidgetToolBar::RemoteAccessWidgetToolBar( RemoteAccessWidget* parent, bool viewOnly ) :
	QWidget( parent ),
	m_parent( parent ),
	m_showHideTimeLine( ShowHideAnimationDuration, this ),
	m_iconStateTimeLine( 0, this ),
	m_connecting( false ),
	m_viewOnlyButton( new ToolButton( QPixmap( QStringLiteral(":/remoteaccess/kmag.png") ), tr( "View only" ), tr( "Remote control" ) ) ),
	m_sendShortcutButton( new ToolButton( QPixmap( QStringLiteral(":/remoteaccess/preferences-desktop-keyboard.png") ), tr( "Send shortcut" ) ) ),
	m_screenshotButton( new ToolButton( QPixmap( QStringLiteral(":/remoteaccess/camera-photo.png") ), tr( "Screenshot" ) ) ),
	m_fullScreenButton( new ToolButton( QPixmap( QStringLiteral(":/remoteaccess/view-fullscreen.png") ), tr( "Fullscreen" ), tr( "Window" ) ) ),
	m_exitButton( new ToolButton( QPixmap( QStringLiteral(":/remoteaccess/application-exit.png") ), tr( "Exit" ) ) )
{
	QPalette pal = palette();
	pal.setBrush( QPalette::Window, QPixmap( QStringLiteral(":/core/toolbar-background.png") ) );
	setPalette( pal );

	setAttribute( Qt::WA_NoSystemBackground, true );
	move( 0, 0 );
	show();
	startConnection();

	m_viewOnlyButton->setCheckable( true );
	m_fullScreenButton->setCheckable( true );
	m_viewOnlyButton->setChecked( viewOnly );
	// hihoon change start for RemoteBIOSControl Mode
/*
	if (remoteMode == VncView::ViewOnlyMode)
		m_viewOnlyButton->setChecked( true );
	else
		m_viewOnlyButton->setChecked( false );
*/
	// hihoon change end for RemoteBIOSControl Mode
	m_fullScreenButton->setChecked( false );

	connect( m_viewOnlyButton, &ToolButton::toggled, this, &RemoteAccessWidgetToolBar::updateControls );
	connect( m_viewOnlyButton, &QAbstractButton::toggled, parent, &RemoteAccessWidget::toggleViewOnly );
	connect( m_fullScreenButton, &QAbstractButton::toggled, parent, &RemoteAccessWidget::toggleFullScreen );
	connect( m_screenshotButton, &QAbstractButton::clicked, parent, &RemoteAccessWidget::takeScreenshot );
	connect( m_exitButton, &QAbstractButton::clicked, parent, &QWidget::close );

	auto vncView = parent->vncView();

	auto shortcutMenu = new QMenu();
#if QT_VERSION < 0x050600
#warning Building legacy compat code for unsupported version of Qt
	connect( shortcutMenu->addAction( tr( "Ctrl+Alt+Del" ) ), &QAction::triggered,
			 vncView, [=]() { vncView->sendShortcut( VncView::ShortcutCtrlAltDel ); } );
	connect( shortcutMenu->addAction( tr( "Ctrl+Esc" ) ), &QAction::triggered,
			 vncView, [=]() { vncView->sendShortcut( VncView::ShortcutCtrlEscape ); } );
	connect( shortcutMenu->addAction( tr( "Alt+Tab" ) ), &QAction::triggered,
			 vncView, [=]() { vncView->sendShortcut( VncView::ShortcutAltTab ); } );
	connect( shortcutMenu->addAction( tr( "Alt+F4" ) ), &QAction::triggered,
			 vncView, [=]() { vncView->sendShortcut( VncView::ShortcutAltF4 ); } );
	connect( shortcutMenu->addAction( tr( "Win+Tab" ) ), &QAction::triggered,
			 vncView, [=]() { vncView->sendShortcut( VncView::ShortcutWinTab ); } );
	connect( shortcutMenu->addAction( tr( "Win" ) ), &QAction::triggered,
			 vncView, [=]() { vncView->sendShortcut( VncView::ShortcutWin ); } );
	connect( shortcutMenu->addAction( tr( "Menu" ) ), &QAction::triggered,
			 vncView, [=]() { vncView->sendShortcut( VncView::ShortcutMenu ); } );
	connect( shortcutMenu->addAction( tr( "Alt+Ctrl+F1" ) ), &QAction::triggered,
			 vncView, [=]() { vncView->sendShortcut( VncView::ShortcutAltCtrlF1 ); } );
#else
	shortcutMenu->addAction( tr( "Ctrl+Alt+Del" ), vncView, [=]() { vncView->sendShortcut( VncView::ShortcutCtrlAltDel ); }  );
	shortcutMenu->addAction( tr( "Ctrl+Esc" ), vncView, [=]() { vncView->sendShortcut( VncView::ShortcutCtrlEscape ); }  );
	shortcutMenu->addAction( tr( "Alt+Tab" ), vncView, [=]() { vncView->sendShortcut( VncView::ShortcutAltTab ); }  );
	shortcutMenu->addAction( tr( "Alt+F4" ), vncView, [=]() { vncView->sendShortcut( VncView::ShortcutAltF4 ); }  );
    shortcutMenu->addAction( tr( "Ctrl+Alt+F1" ), vncView, [=]() { vncView->sendShortcut( VncView::ShortcutCtrlAltF1 ); }  );
    shortcutMenu->addAction( tr( "Ctrl+Alt+F2" ), vncView, [=]() { vncView->sendShortcut( VncView::ShortcutCtrlAltF2 ); }  );
    shortcutMenu->addAction( tr( "Ctrl+Alt+F3" ), vncView, [=]() { vncView->sendShortcut( VncView::ShortcutCtrlAltF3 ); }  );
    shortcutMenu->addAction( tr( "Ctrl+Alt+F4" ), vncView, [=]() { vncView->sendShortcut( VncView::ShortcutCtrlAltF4 ); }  );
    shortcutMenu->addAction( tr( "Ctrl+Alt+F5" ), vncView, [=]() { vncView->sendShortcut( VncView::ShortcutCtrlAltF5 ); }  );
    shortcutMenu->addAction( tr( "Ctrl+Alt+F6" ), vncView, [=]() { vncView->sendShortcut( VncView::ShortcutCtrlAltF6 ); }  );
    shortcutMenu->addAction( tr( "Ctrl+Alt+F7" ), vncView, [=]() { vncView->sendShortcut( VncView::ShortcutCtrlAltF7 ); }  );
    shortcutMenu->addAction( tr( "Ctrl+Alt+F8" ), vncView, [=]() { vncView->sendShortcut( VncView::ShortcutCtrlAltF8 ); }  );
    shortcutMenu->addAction( tr( "Win+Tab" ), vncView, [=]() { vncView->sendShortcut( VncView::ShortcutWinTab ); }  );
	shortcutMenu->addAction( tr( "Win" ), vncView, [=]() { vncView->sendShortcut( VncView::ShortcutWin ); }  );
	shortcutMenu->addAction( tr( "Menu" ), vncView, [=]() { vncView->sendShortcut( VncView::ShortcutMenu ); }  );
	shortcutMenu->addAction( tr( "Alt+Ctrl+F1" ), vncView, [=]() { vncView->sendShortcut( VncView::ShortcutAltCtrlF1 ); }  );
#endif

	m_sendShortcutButton->setMenu( shortcutMenu );
	m_sendShortcutButton->setPopupMode( QToolButton::InstantPopup );
	m_sendShortcutButton->setObjectName( QStringLiteral("shortcuts") );

	auto layout = new QHBoxLayout( this );
	layout->setMargin( 1 );
	layout->setSpacing( 1 );
	layout->addStretch( 0 );
	layout->addWidget( m_sendShortcutButton );
	layout->addWidget( m_viewOnlyButton );
	layout->addWidget( m_screenshotButton );
	layout->addWidget( m_fullScreenButton );
	layout->addWidget( m_exitButton );
	layout->addSpacing( 5 );
	connect( vncView, &VncView::startConnection, this, &RemoteAccessWidgetToolBar::startConnection );
	connect( vncView, &VncView::connectionEstablished, this, &RemoteAccessWidgetToolBar::connectionEstablished );

	setFixedHeight( m_exitButton->height() );

	connect( &m_showHideTimeLine, &QTimeLine::valueChanged, this, &RemoteAccessWidgetToolBar::updatePosition );

	m_iconStateTimeLine.setFrameRange( 0, 100 );
	m_iconStateTimeLine.setDuration( 1500 );
	m_iconStateTimeLine.setUpdateInterval( 60 );
	m_iconStateTimeLine.setCurveShape( QTimeLine::SineCurve );
	connect( &m_iconStateTimeLine, &QTimeLine::valueChanged, this, &RemoteAccessWidgetToolBar::updateConnectionAnimation );
	connect( &m_iconStateTimeLine, &QTimeLine::finished, &m_iconStateTimeLine, &QTimeLine::start );
}


void RemoteAccessWidgetToolBar::appear()
{
	m_showHideTimeLine.setDirection( QTimeLine::Backward );
	if( m_showHideTimeLine.state() != QTimeLine::Running )
	{
		m_showHideTimeLine.resume();
	}
}


void RemoteAccessWidgetToolBar::disappear()
{
	if( !m_connecting && !rect().contains( mapFromGlobal( QCursor::pos() ) ) )
	{
		QTimer::singleShot( DisappearDelay, this, [this]() {
			if( m_showHideTimeLine.state() != QTimeLine::Running )
			{
				m_showHideTimeLine.setDirection( QTimeLine::Forward );
				m_showHideTimeLine.resume();
			}
		} );
	}
}



void RemoteAccessWidgetToolBar::updateControls( bool viewOnly )
{
	m_sendShortcutButton->setVisible( viewOnly == false );
}



void RemoteAccessWidgetToolBar::leaveEvent( QEvent *event )
{
	disappear();
	QWidget::leaveEvent( event );
}



void RemoteAccessWidgetToolBar::paintEvent( QPaintEvent *paintEv )
{
	QPainter p( this );
	QFont f = p.font();

	p.setOpacity( 0.8-0.8*m_showHideTimeLine.currentValue() );
	p.fillRect( paintEv->rect(), palette().brush( QPalette::Window ) );
	p.setOpacity( 1 );

	f.setPointSize( 12 );
	f.setBold( true );
	p.setFont( f );

	//p.setPen( Qt::white );
	//p.drawText( 64, 22, m_parent->windowTitle() );

	p.setPen( QColor( 192, 192, 192 ) );
	f.setPointSize( 10 );
	p.setFont( f );

	if( m_connecting )
	{
		QString dots;
		for( int i = 0; i < ( m_iconStateTimeLine.currentTime() / 120 ) % 6; ++i )
		{
			dots += QLatin1Char('.');
		}
		p.drawText( 32, height() / 2 + fontMetrics().height(), tr( "Connecting %1" ).arg( dots ) );
	}
	else
	{
		p.drawText( 32, height() / 2 + fontMetrics().height(), tr( "Connected." ) );
	}
}



void RemoteAccessWidgetToolBar::updateConnectionAnimation()
{
	repaint();
}



void RemoteAccessWidgetToolBar::updatePosition()
{
	const auto newY = static_cast<int>( m_showHideTimeLine.currentValue() * height() );

	if( newY != -y() )
	{
		move( x(), qMax( -height(), -newY ) );
	}
}



void RemoteAccessWidgetToolBar::startConnection()
{
	m_connecting = true;
	m_iconStateTimeLine.start();
	appear();
	update();
}




void RemoteAccessWidgetToolBar::connectionEstablished()
{
	m_connecting = false;
	m_iconStateTimeLine.stop();
	disappear();

	// within the next 1000ms the username should be known and therefore we update
	QTimer::singleShot( 1000, this, QOverload<>::of( &RemoteAccessWidgetToolBar::update ) );
}




// hihoon RemoteAccessWidget::RemoteAccessWidget( const ComputerControlInterface::Pointer& computerControlInterface, bool viewOnly ) :
RemoteAccessWidget::RemoteAccessWidget( const ComputerControlInterface::Pointer& computerControlInterface, VncView::Mode remoteMode) :
	QWidget( nullptr ),
	m_computerControlInterface( computerControlInterface ),
	// hihoon original m_vncView( new VncView( computerControlInterface->computer().hostAddress(), -1, this, VncView::RemoteControlMode ) ),
	m_vncView( new VncView( computerControlInterface->computer().hostAddress(), -1, this, remoteMode ) ),
	// hihoon original m_connection( new VeyonConnection( m_vncView->vncConnection() ) ),
	m_connection( new VeyonConnection( m_vncView->vncConnection() ) ) 
	// hihoon original m_toolBar( new RemoteAccessWidgetToolBar( this, viewOnly ) )
{

	// hihoon add
	if (remoteMode == VncView::ViewOnlyMode)
		m_toolBar = new RemoteAccessWidgetToolBar( this, true );
	else
		m_toolBar = new RemoteAccessWidgetToolBar( this, false );

	setWindowTitle( tr( "%1 - %2 Remote Access" ).arg( computerControlInterface->computer().name(),
													   VeyonCore::applicationName() ) );
	setWindowIcon( QPixmap( QStringLiteral(":/remoteaccess/kmag.png") ) );
	setAttribute( Qt::WA_DeleteOnClose, true );

	m_vncView->move( 0, 0 );
	connect( m_vncView, &VncView::mouseAtBorder, m_toolBar, &RemoteAccessWidgetToolBar::appear );
	connect( m_vncView, &VncView::keyEvent, this, &RemoteAccessWidget::checkKeyEvent );
	connect( m_vncView, &VncView::sizeHintChanged, this, &RemoteAccessWidget::updateSize );

	showMaximized();
	VeyonCore::platform().coreFunctions().raiseWindow( this );

	showNormal();

	move( 0, 0 );

	// hihoon add
	if (remoteMode == VncView::ViewOnlyMode)
		toggleViewOnly( true );
	else
		toggleViewOnly( false );
}



RemoteAccessWidget::~RemoteAccessWidget()
{
	delete m_connection;
	delete m_vncView;
}



void RemoteAccessWidget::enterEvent( QEvent* event )
{
	m_toolBar->disappear();

	QWidget::enterEvent( event );
}



void RemoteAccessWidget::leaveEvent( QEvent* event )
{
	QTimer::singleShot( AppearDelay, this, [this]() {
		if( underMouse() == false )
		{
			m_toolBar->appear();
		}
	} );

	QWidget::leaveEvent( event );
}



void RemoteAccessWidget::resizeEvent( QResizeEvent* event )
{
	m_vncView->resize( size() );
	m_toolBar->setFixedSize( width(), m_toolBar->height() );

	QWidget::resizeEvent( event );
}



void RemoteAccessWidget::checkKeyEvent( unsigned int key, bool pressed )
{
	if( pressed && key == XK_Escape && !m_connection->isConnected() )
	{
		close();
	}
}



void RemoteAccessWidget::updateSize()
{
	if( !( windowState() & Qt::WindowFullScreen ) &&
			m_vncView->sizeHint().isEmpty() == false )
	{
		resize( m_vncView->sizeHint() );
	}
}



void RemoteAccessWidget::toggleFullScreen( bool _on )
{
	if( _on )
	{
		setWindowState( windowState() | Qt::WindowFullScreen );
	}
	else
	{
		setWindowState( windowState() & ~Qt::WindowFullScreen );
	}
}



void RemoteAccessWidget::toggleViewOnly( bool viewOnly )
{
	m_vncView->setViewOnly( viewOnly );
	m_toolBar->updateControls( viewOnly );
	m_toolBar->update();
}



void RemoteAccessWidget::takeScreenshot()
{
	Screenshot().take( m_computerControlInterface );
}
