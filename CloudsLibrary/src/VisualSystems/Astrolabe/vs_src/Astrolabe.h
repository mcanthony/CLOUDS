//
//  Astrolabe.h
//  Astrolabe
//
//  Created by laserstorm on 12/20/13.
//
//

#pragma once
#include "ofMain.h"
#include "ofxTween.h"

class TweenInfo{
public:
	TweenInfo()
	{
		scale = 1.;
	};
	~TweenInfo(){};
	
	void set( float _step, float _duration, float _delay, float _scale = 1 )
	{
		step = _step;
		duration = _duration;
		delay = _delay;
		reverse = false;
		scale = _scale;
	}
	
	void operator = (TweenInfo ti)
	{
		duration = ti.duration;
		delay = ti.delay;
		step = ti.step;
		reverse = ti.reverse;
		scale = ti.scale;
	}
	
	float duration;
	float delay;
	float step;
	bool reverse;
	float scale;
};

class Astrolabe : public ofNode{
public:
	Astrolabe();
	~Astrolabe();
	
	void clear();
	
	void setupMesh( float _lowRadian, float _hiRadian, float _innerRadius, float _outerRadius, float _thickness);
	
	void updateRotations();
	
	void addRotationTween( string axis, float startVal, float step, float duration, float delay, float increment, float _scale=1 );
	void setRotationTween( string axis, float startVal, float step, float duration, float delay, float increment, float _scale=1 );
	
	void draw();
	
	void setVbo( ofVbo& _vbo, int _numVertices );
	
	void buildTheMesh();
	
	ofVec3f normalFromThreePoints(ofVec3f p0, ofVec3f p1, ofVec3f p2);
	
	void setTweenIncrement(string axis, int increment);
	
	void setTweenDelay(string axis, int delay);
	
	void setTweenScale(string axis, float scale);
	
	void setTweenReverse( string axis, bool reverse );
	
//	void getTweenReverse( string axis );
	
	void setEase( ofxEasing& e );
	void setEase( ofxEasing* e );
	
//protected:
	ofxEasing* ease;
	ofxEasingLinear easelinear;
	
	
	ofVec3f eularAngels;
	map<string, ofxTween> rot;
	map<string, TweenInfo> tweenInfo;
	
	ofColor color;
	float r0, r1;//radians
	float innerRadius, outerRadius, thickness;
	vector <ofVec3f> profile;
	
	ofVbo* adoptedVbo;
	ofVbo vbo;
	int numVertices;
	vector<ofVec3f> vertices;
	vector<ofVec3f> normals;
	
	//shader deformation variables
	float thicknessOffset;
};

