//
//  BgSubtractApp.cpp
//  BgSubtract
//
//  Created by Nien Lam on 2/7/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIO.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class BgSubtractApp : public AppBasic 
{
  public:
	void setup();
	void keyDown( KeyEvent event );
	void update();
	void draw();
	void prepareSettings( Settings *settings );
  private:
	// Size of the input images.
	static const int IMG_WIDTH  = 320;
	static const int IMG_HEIGHT = 240;
	static const int IMG_SPACER = 10;
		
	// Remember Draw state.
	enum DrawState { DRAW_STATE_1, DRAW_STATE_2, DRAW_STATE_3 };
	DrawState mDrawState;
	
	// 3 sets of 2 channels for background subtraction.
	Channel32f mImg1a;
	Channel32f mImg1b;
	Channel32f mImg2a;
	Channel32f mImg2b;
	Channel32f mImg3a;
	Channel32f mImg3b;

	// Threshold value for background subtraction.
	float mThreshold;
	
	// Background subtraction with threshold.
	void subtractBg( Channel32f const& channel1, Channel32f const& channel2, 
					 Channel32f& channelResult, float threshold );
};


void BgSubtractApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( IMG_WIDTH * 3 + IMG_SPACER * 2, IMG_HEIGHT );
	settings->setFrameRate( 60.0f );
}


void BgSubtractApp::setup()
{
	// Get image files.
	mImg1a = Channel32f( loadImage( loadResource( "img-1a.jpg" ) ) );
	mImg1b = Channel32f( loadImage( loadResource( "img-1b.jpg" ) ) );
	mImg2a = Channel32f( loadImage( loadResource( "img-2a.jpg" ) ) );
	mImg2b = Channel32f( loadImage( loadResource( "img-2b.jpg" ) ) );
	mImg3a = Channel32f( loadImage( loadResource( "img-3a.jpg" ) ) );
	mImg3b = Channel32f( loadImage( loadResource( "img-3b.jpg" ) ) );
	
	// Initialize draw state.
	mDrawState = DRAW_STATE_1;
	
	// Initialize threshold value.
	mThreshold = .2;
}


void BgSubtractApp::update()
{
}


void BgSubtractApp::draw()
{
	gl::clear( Color( 0, 0, 0 ), true );
	
	// Channels for outputing to the screen.
	Channel32f output1, output2;
	Channel32f output3 = Channel32f( IMG_WIDTH, IMG_HEIGHT );

	// Switch draw state.
	switch( mDrawState ) 
	{
		case DRAW_STATE_1:
			output1 = mImg1a;
			output2 = mImg1b;
			break;
			
		case DRAW_STATE_2:
			output1 = mImg2a;
			output2 = mImg2b;
			break;
			
		case DRAW_STATE_3:
			output1 = mImg3a;
			output2 = mImg3b;
			break;
			
		default:
			console() << __PRETTY_FUNCTION__ << ":" << __LINE__ << ":" << "Unsupported Draw State" << endl;
			exit( 1 );
	}
	
	// Apply background subtraction
	subtractBg( output1, output2, output3, mThreshold);
	
	// Draw to screen.
	gl::draw( output1, Vec2i(0, 0) );
	gl::draw( output2, Vec2i( IMG_WIDTH + IMG_SPACER, 0 ) );
	gl::draw( output3, Vec2i( ( IMG_WIDTH + IMG_SPACER ) * 2 , 0 ) );
}


void BgSubtractApp::keyDown( KeyEvent event ) 
{
	// Change draw state based on keyboard input.
    if ( event.getChar() == '1' )
	{
		mDrawState = DRAW_STATE_1;
	}
    else if ( event.getChar() == '2' )
	{	
		mDrawState = DRAW_STATE_2;
	}
	else if ( event.getChar() == '3' )
	{
		mDrawState = DRAW_STATE_3;
	}

	// Change threshold with key press.       
    if ( event.getCode() == KeyEvent::KEY_UP  )
	{
		mThreshold = fminf( 1.0, mThreshold + .01 ); 
	}
    else if ( event.getCode() == KeyEvent::KEY_DOWN  )
	{	
		mThreshold = fmaxf( 0.0, mThreshold - .01 );
	}
}	


void BgSubtractApp::subtractBg( Channel32f const& channel1, Channel32f const& channel2, 
								Channel32f& channelResult, float threshold )
{	
	Channel32f::ConstIter iterCh1 = channel1.getIter();
	Channel32f::ConstIter iterCh2 = channel2.getIter();
	Channel32f::Iter      iterCh3 = channelResult.getIter();

	while ( iterCh1.line() && iterCh2.line() && iterCh3.line() ) 
	{
		while ( iterCh1.pixel() && iterCh2.pixel() && iterCh3.pixel()  ) 
		{
			if ( abs( iterCh1.v() - iterCh2.v() ) < threshold )
				iterCh3.v() = 255;
			else	
				iterCh3.v() = 0;
		}
	}
}


CINDER_APP_BASIC( BgSubtractApp, RendererGl )
