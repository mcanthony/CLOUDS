//
//  CloudsInput.h
//  CLOUDS
//
//  Created by James George on 12/8/13.
//
//

#pragma once

#include "ofMain.h"

enum CloudsCursorMode
{
    CURSOR_MODE_NONE = 0,
    CURSOR_MODE_INACTIVE,
    CURSOR_MODE_CAMERA,
    CURSOR_MODE_DRAW
};

class CloudsInputEvents;
class CloudsInteractionEventArgs;
class CloudsInput
{
  public:
	CloudsInput();
	
	virtual void enable() = 0;
	virtual void disable() = 0;
	
	void interactionMoved(ofVec3f pos, bool primary = false, bool dragged = false, int actionType = 0, int playerId = 0, float focus = 1.0f, bool canceled = false);
	void interactionStarted(ofVec3f pos, bool primary = false, bool dragged = false, int actionType = 0, int playerId = 0, float focus = 1.0f, bool canceled = false);
	void interactionDragged(ofVec3f pos, bool primary = false, bool dragged = false, int actionType = 0, int playerId = 0, float focus = 1.0f, bool canceled = false);
	void interactionEnded(ofVec3f pos, bool primary = false, bool dragged = false, int actionType = 0, int playerId = 0, float focus = 1.0f, bool canceled = false);

	bool isDragging();
	float getPositionX();
	float getPositionY();
	float getPositionZ();
	ofVec3f getPosition();
    ofVec3f getPreviousPosition();
	
    map<int, CloudsInteractionEventArgs>& getInputPoints();
	
	CloudsInputEvents& getEvents();
	void setBleedPixels(int bleed);
    ofVec2f getBleedPixels();
	
    virtual void drawCursorDefault(CloudsCursorMode mode, ofVec3f& pos, bool bDragged, float focus, float fadeOut) = 0;
    void selfDrawCursorDefault(CloudsCursorMode mode, ofVec3f& pos, bool bDragged, float focus, float fadeOut, float size);
    
    bool bUserBegan;
    
  protected:

    ofVec3f currentPosition;
	ofVec3f lastPosition;
    ofVec3f bleedOffset;
	
    bool dragging;
    int bleed;
    
    map<int, CloudsInteractionEventArgs> inputPoints;

	bool enabled;
	CloudsInputEvents* events;
	
};

void SetCloudsInput(ofPtr<CloudsInput> input);
ofPtr<CloudsInput> GetCloudsInput();

//global accessors, replacement for ofGetMouseX(), etc
bool GetCloudsInputPressed();
float GetCloudsInputX();
float GetCloudsInputY();
float GetCloudsInputZ();
ofVec3f GetCloudsInputPosition();

float GetCloudsPreviousInputX();
float GetCloudsPreviousInputY();
float GetCloudsPreviousInputZ();
ofVec3f GetCloudsPreviousInputPosition();

void SetBleedPixels(int bleed);

//TODIZZLE:
//float GetCloudsPreviousInputX(int playerId = 0);
//float GetCloudsPreviousInputY(int playerId = 0);
//float GetCloudsPreviousInputZ(int playerId = 0);
//ofVec3f GetCloudsPreviousInputPosition(int playerId = 0);

map<int, CloudsInteractionEventArgs>& GetCloudsInputPoints();

