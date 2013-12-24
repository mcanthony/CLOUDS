//
//  CloudsSDLayer.h
//  CloudsSDDesigner
//
//  Created by James George on 12/14/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxSvg.h"
#include "ofxTween.h"

typedef struct {
	string gid;
	ofRectangle rect;
} TextBounds;

class CloudsSDLayer
{
public:
	
	CloudsSDLayer();
	
	void parse(string svgFilePath);
	
	vector<TextBounds> textBoxes;
	ofxSVG svg;
	
	void start();
	void update();
	void draw();
	
	bool animating;
	float delayTime;
	float startTime;
	float duration;
	
	float maxUpdateInterval;
	float lastUpdateTime;
	
	ofVec2f startPercent;
	
	ofVec2f percentComplete;
	float xlag;
	ofRectangle drawRect;
	ofVec2f startPoint;
	ofVec2f endPoint;
	
};
