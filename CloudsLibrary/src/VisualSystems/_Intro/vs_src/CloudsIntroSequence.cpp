//
//  IntroSequence.cpp
//  ComputerTicker
//
//  Created by James George on 5/26/13.
//
//

#include "CloudsIntroSequence.h"
#include "CloudsGlobal.h"
#include "CloudsEvents.h"
#include "CloudsPortalEvents.h"
#ifdef KINECT_INPUT
#include "CloudsInputKinectOSC.h"
#endif

//JG REMOVING THIS
CloudsVisualSystemEvents CloudsIntroSequence::events;

string CloudsIntroSequence::getSystemName(){
	return "_Intro";
}

void CloudsIntroSequence::selfSetDefaults(){

	selectedQuestion = NULL;
	showingQuestions = false;
	paused = false;
	currentFontSize = -1;
	currentFontExtrusion = -1;
	startedOnclick = false;
	caughtQuestion = NULL;
	titleNoisePosition = 0;
	titleNoiseSpeed = 0;
	hoveringTitle = false;
	currentTitleOpacity = 0;
	bQuestionDebug = false;
	
	kinectHelperAlpha = 0.0;
	nodeAlphaAttenuate = 1.0; //rift calibration nodes
	
	introNodeOne.hover = introNodeOne.finished = false;
	introNodeTwo.hover = introNodeTwo.finished = false;
	introNodeThree.hover = introNodeThree.finished = false;

	introNodeOne.multiplier  = 1;
	introNodeTwo.multiplier = -1;
	introNodeThree.multiplier = 0;
	
	hintCursorEndPoint = ofVec2f(320,240);

	promptTime = 0.0;
	promptShown = false;

	// Set question defaults.
	questionScale = 0.1f;
	helperFontSize = 14; //TODO make dynamic per preset
//    questionFontSize = 14;
//    questionLineLength = 300.0f;
//    questionLineSpacing = 1.0f;
	questionMinZDistance = 50;
	perlinOffset = 0;
	
    warpCamera.setNearClip(.01);
	
	timeSinceLastPrompt = 0;
	
	questionChannels.resize(4);
	channelPauseTime.resize(4);
	
#ifdef OCULUS_RIFT
    bCursorInCenter = false;
    startTimeCursorInCenter = 0;
#endif

}

void CloudsIntroSequence::selfSetup(){
	
	ofDisableArbTex();
	sprite.loadImage(getVisualSystemDataPath() + "images/dot.png");
	ofEnableArbTex();
	
	currentFontExtrusion = -1;
	currentFontSize = -1;
	
	introNodes.push_back( &introNodeOne );
	introNodes.push_back( &introNodeTwo );
	introNodes.push_back( &introNodeThree );

	reloadShaders();
}

void CloudsIntroSequence::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "arrange questions" && ((ofxUIButton*)e.widget)->getValue()){
		positionStartQuestions();
	}
	else if(e.widget->getName() == "generate tunnel" && ((ofxUIButton*)e.widget)->getValue()){
		generateTunnel();
	}
}

void CloudsIntroSequence::selfSetupCameraGui(){
	camGui->addSlider("camera fwd force", 0, 2, &cameraForwardSpeed);
	camGui->addSlider("camera wobble range", 0, 10, &camWobbleRange);
	camGui->addSlider("camera wobble speed", 0, 1., &camWobbleSpeed);
	camGui->addToggle("hold camera", &paused);
}

void CloudsIntroSequence::guiSystemEvent(ofxUIEventArgs &e){
}

void CloudsIntroSequence::selfSetupRenderGui(){
	
	//	rdrGui->addSlider("Min Point Size", 0, 7, &pointSize.min);
	rdrGui->addSlider("Max Point Size", 0, 7, &pointSize.max);
	rdrGui->addSlider("Min Distance", 0, 500, &distanceRange.min);
	rdrGui->addSlider("Max Distance", 0, 500, &distanceRange.max);
	
	//rdrGui->addSlider("Chroma Max Distort", 0, 10, &maxChromaDistort);
	
	rdrGui->addSlider("Perlin Amplitude", 0, 10, &perlinAmplitude);
	rdrGui->addSlider("Perlin Density", 0, 50, &perlinDensity);
	rdrGui->addSlider("Perlin Speed", 0, .1, &perlinSpeed);
	
	//	rdrGui->addSlider("Font Size", 1, 10, &fontSize);
	//	rdrGui->addSlider("Font Extrusion", 0, 10, &fontExtrusion);
	
	rdrGui->addSlider("Wireframe Alpha", 0, 1.0, &wireframeAlpha);
}


void CloudsIntroSequence::selfSetupGuis(){
	
	questionGui = new ofxUISuperCanvas("QUESTIONS", gui);
	questionGui->copyCanvasStyle(gui);
	questionGui->copyCanvasProperties(gui);
	questionGui->setName("Questions");
	questionGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	questionGui->addToggle("Debug Tug", &bQuestionDebug);
	questionGui->addSlider("Scale", 0, 1, &questionScale);
	questionGui->addSlider("Wrap Distance", 100, 4000, &questionWrapDistance);
	questionGui->addSlider("Question Min Z", 10, 200, &questionMinZDistance);
	questionGui->addSlider("Inner Radius", 2, 20, &questionTunnelInnerRadius);
	questionGui->addRangeSlider("Tug Distance", 10, 300, &questionTugDistance.min, &questionTugDistance.max);
	questionGui->addRangeSlider("Attenuate Distance", 10, 300,&questionAttenuateDistance.min,&questionAttenuateDistance.max);
	questionGui->addSlider("Question Pause Dur", 1.0, 15.0, &questionPauseDuration);
	
	questionGui->addButton("arrange questions", false);
	//	questionGui->addToggle("Custom Toggle", &customToggle);
	ofAddListener(questionGui->newGUIEvent, this, &CloudsIntroSequence::selfGuiEvent);
	
	guis.push_back(questionGui);
	guimap[questionGui->getName()] = questionGui;
	
	
	tunnelGui = new ofxUISuperCanvas("TUNNEL", gui);
	tunnelGui->copyCanvasStyle(gui);
	tunnelGui->copyCanvasProperties(gui);
	tunnelGui->setName("Tunnel");
	tunnelGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	tunnelGui->addSlider("Tunnel Width",  10, 50, &tunnelMax.x);
	tunnelGui->addSlider("Tunnel Height", 10, 50, &tunnelMax.y);
	tunnelGui->addSlider("Tunnel Depth",  100, 1000, &tunnelMax.z);
	
	tunnelGui->addSlider("Tunnel Tint H",  0, 1.0, &tint.r);
	tunnelGui->addSlider("Tunnel Tint S",  0, 1.0, &tint.g);
	tunnelGui->addSlider("Tunnel Tint B",  0, 1.0, &tint.b);
	
	tunnelGui->addSlider("Loose Rez X", 5, 20, &looseTunnelResolutionX);
	tunnelGui->addSlider("Loose Rez Z", 5, 50, &looseTunnelResolutionZ);
    
	tunnelGui->addButton("generate tunnel", false);
	
	ofAddListener(tunnelGui->newGUIEvent, this, &CloudsIntroSequence::selfGuiEvent);
	
	guis.push_back(tunnelGui);
	guimap[tunnelGui->getName()] = tunnelGui;
	
	typeGui = new ofxUISuperCanvas("TYPE", gui);
	typeGui->copyCanvasStyle(gui);
	typeGui->copyCanvasProperties(gui);
	typeGui->setName("Type");
	typeGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	typeGui->addIntSlider("Title Size", 2, 15, &titleFontSize);
	typeGui->addIntSlider("Title Extrude", 1, 5, &titleFontExtrude);
	typeGui->addSlider("Title Tracking", 0, 50, &titleTypeTracking);
	typeGui->addSlider("Title Offset", 0, 100, &titleTypeOffset);
	typeGui->addSlider("Title Opacity", .0, 1., &titleTypeOpacity);
	
	typeGui->addRangeSlider("Title Glow Range", 0., 1.0, &titleMinGlow, &titleMaxGlow);
	typeGui->addSlider("Title Noise Speed", 0, .1, &titleNoiseSpeed);
	typeGui->addSlider("Title Noise Dense", .001, 1., &titleNoiseDensity);
	typeGui->addSlider("Title Hover X", 0, 1.0, &titleRectWidth);
	typeGui->addSlider("Title Hover Y", 0, 1.0, &titleRectHeight);
	
	guis.push_back(typeGui);
	guimap[typeGui->getName()] = typeGui;
	
	
	introGui = new ofxUISuperCanvas("INTRO", gui);
	introGui->copyCanvasStyle(gui);
	introGui->copyCanvasProperties(gui);
	introGui->setName("Intro");
	introGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	introGui->addSlider("Intro Node Size", 2, 7, &introNodeSize);
	introGui->addSlider("Intro Node Hold Time", .5, 3, &introNodeHoldTime); //seconds
	introGui->addSlider("Intro Node X",    0, 400, &introNodeOffset.x); //mirrored along the axis
	introGui->addSlider("Intro Node Y",    0, 200, &introNodeOffset.y); //mirrored along the axis
	introGui->addSlider("Intro Node Z", -100, 100, &introNodeOffset.z); //mirrored along the axis
	
	guis.push_back(introGui);
	guimap[introGui->getName()] = introGui;
	
	
	helperTextGui = new ofxUISuperCanvas("HELPERTEXT", gui);
	helperTextGui->copyCanvasStyle(gui);
	helperTextGui->copyCanvasProperties(gui);
	helperTextGui->setName("Helpertext");
	helperTextGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	helperTextGui->addIntSlider("FONT SIZE", 4, 100, &helperFontSize);
	helperTextGui->addSlider("FONT SCALE", 0, 1.0, &helperFontScale);
	helperTextGui->addSlider("TRACKING", 0, 30, &helperFontTracking);
	helperTextGui->addSlider("Y POSITION", -200, 200, &helperFontY);
	
	guis.push_back(helperTextGui);
	guimap[helperTextGui->getName()] = helperTextGui;

}

void CloudsIntroSequence::selfPresetLoaded(string presetPath){
	generateTunnel();
	warpCamera.setPosition(0, 0, 0);
	warpCamera.lookAt(ofVec3f(0,0,tunnelMax.z));
	positionStartQuestions();
}

void CloudsIntroSequence::reloadShaders(){
	tunnelShader.load(getVisualSystemDataPath() + "shaders/IntroTunnel");
	typeShader.load(getVisualSystemDataPath() + "shaders/IntroType");
	CloudsPortal::reloadShader();
}

void CloudsIntroSequence::selfUpdate(){
	
	if(!startedOnclick && timeline->getIsPlaying()){
		timeline->stop();
	}
	
	updateCamera();
	updateWaiting();
	updateQuestions();
	updateTitle();
}

void CloudsIntroSequence::updateCamera(){
	
	ofVec2f wobble = ofVec2f(ofSignedNoise(100 + ofGetElapsedTimef()*camWobbleSpeed),
							 ofSignedNoise(200 + ofGetElapsedTimef()*camWobbleSpeed)) * camWobbleRange;
	if(!paused){
		warpCamera.dolly(-cameraForwardSpeed);
		warpCamera.setPosition(wobble.x, wobble.y, warpCamera.getPosition().z);
	}
	else{
		warpCamera.setPosition(wobble.x, wobble.y, 0);
	}
	warpCamera.lookAt( ofVec3f(0, 0, warpCamera.getPosition().z + 50) );
	
}

void CloudsIntroSequence::updateWaiting(){
	
    // Trigger start manually
    if (startedOnclick) {
		nodeAlphaAttenuate = MAX(0,nodeAlphaAttenuate-0.02);
		return;
	}
	
	#ifdef OCULUS_RIFT
	for(int i = 0; i < introNodes.size(); i++){
		updateIntroNodePosition(*introNodes[i]);
	}
	
	for(int i = 0; i < introNodes.size(); i++){
		updateIntroNodeInteraction(*introNodes[i]);
		//bail if we haven't completed this part of the sequence
		if(!introNodes[i]->finished){
			break;
		}
	}
	
	if(introNodeThree.finished){
		startedOnclick = true;
		timeline->play();
	}
	
	#elif defined(KINECT_INPUT)
	k4w::ViewerState viewerState = ((CloudsInputKinectOSC*)GetCloudsInput().get())->viewerState;
	if(startQuestions.size() > 0 && viewerState != k4w::ViewerState_None){
		if(!promptShown && ofGetElapsedTimef() - timeSinceLastPrompt > 8){
			if(viewerState == k4w::ViewerState_OutOfRange){
				CloudsPortalEventArgs args("MOVE CLOSER TO THE DISPLAY");
				ofNotifyEvent(events.portalHoverBegan, args);
			}
			else{
				CloudsPortalEventArgs args("EXTEND YOUR HAND TO BEGIN");
				ofNotifyEvent(events.portalHoverBegan, args);
			}
			timeSinceLastPrompt = ofGetElapsedTimef();
			promptShown = true;
			kinectHelperTargetAlpha = 1.0;
		}
		else if(promptShown && ofGetElapsedTimef() - timeSinceLastPrompt > 4){
			CloudsPortalEventArgs args("");
			ofNotifyEvent(events.portalHoverEnded, args);
			timeSinceLastPrompt = ofGetElapsedTimef();
			promptShown = false;
			kinectHelperTargetAlpha = 0.0;
		}
	}
	 
	#else
	if(startQuestions.size() > 0){
		if(!promptShown && ofGetElapsedTimef() - timeSinceLastPrompt > 10){
			CloudsPortalEventArgs args("CLICK TO BEGIN");
			ofNotifyEvent(events.portalHoverBegan, args);
			timeSinceLastPrompt = ofGetElapsedTimef();
			promptShown = true;

		}
		else if(promptShown && ofGetElapsedTimef() - timeSinceLastPrompt > 4){
			CloudsPortalEventArgs args("");
			ofNotifyEvent(events.portalHoverEnded, args);
			timeSinceLastPrompt = ofGetElapsedTimef();
			promptShown = false;
		}
	}
	#endif
}


void CloudsIntroSequence::updateIntroNodePosition(IntroNode& node){
#ifdef OCULUS_RIFT
	node.worldPosition = ofVec3f( introNodeOffset.x, -introNodeOffset.y, introNodeOffset.z + titleTypeOffset);
	node.worldPosition.x *= node.multiplier;
	node.worldPosition.y *= node.multiplier;
	if(node.multiplier == 0){
		node.worldPosition.y -= 10; //plus 20 just to move everything down a little bit
	}
	else{
		node.worldPosition.z *= .5; //bring the side nodes closer in Z
	}
	
	ofRectangle viewport = getOculusRift().getOculusViewport();
	ofVec3f screenPos = getOculusRift().worldToScreen(node.worldPosition, true);
	//if we look too far behind it'll flip!
	if(screenPos.z < 0){
		node.screenPosition = getOculusRift().worldToScreen(introNodeThree.worldPosition, true);
	}
	else{
		node.screenPosition = ofVec2f(screenPos.x,screenPos.y);
	}
	node.cursorDirection = node.screenPosition - viewport.getCenter();
	node.cursorDistance  = node.cursorDirection.length();
	node.cursorDirection.normalize();

#endif
}

void CloudsIntroSequence::updateIntroNodeInteraction(IntroNode& node){
	
	#ifdef OCULUS_RIFT
	if(node.finished){
		return;
	}
	
	if(!node.hover && node.cursorDistance < questionTugDistance.min){
		node.hover = true;
		node.hoverStartTime = ofGetElapsedTimef();
	}
	else if(node.hover){
		if(node.cursorDistance > questionTugDistance.max){
			node.hover = false;
			node.percentComplete = 0.0;
		}
		else if(node.percentComplete >= 1.0){
			node.finished = true;
			nodeActivatedTime = ofGetElapsedTimef(); //used for alpha on helper text
		}
		else{
			node.percentComplete = ofMap(ofGetElapsedTimef(), node.hoverStartTime, node.hoverStartTime+introNodeHoldTime, 0.0, 1.0,true);
		}
	}
	#endif
}

void CloudsIntroSequence::updateTitle(){
	
	if(currentFontSize != titleFontSize ||
	   currentFontExtrusion != titleFontExtrude)
	{
		currentFontSize = titleFontExtrude;
		currentFontExtrusion = titleFontExtrude;
		extrudedTitleText.loadFont(GetCloudsDataPath() + "font/materiapro_light.ttf", titleFontSize, currentFontExtrusion);
	}
	
	titleRect = ofRectangle(0,0,titleRectWidth*ofGetWidth(),titleRectHeight*ofGetHeight());
	titleRect.alignTo( ofPoint(getCanvasWidth()/2, getCanvasHeight()/2) );
	hoveringTitle = titleRect.inside(GetCloudsInputX(), GetCloudsInputY());
	
	//	cout << "title rect is " << titleRect.getTopLeft() << " " << titleRect.getBottomLeft() << endl;
	//	cout << "hovering? " << (hoveringTitle ? "YES" : "NO" ) << endl;
	
	titleNoisePosition += titleNoiseSpeed;
	float hoverTitleOpacity;
	if(hoveringTitle || (startedOnclick && timeline->getIsPlaying()) ){
		hoverTitleOpacity = .9;
	}
	else{
		hoverTitleOpacity = titleTypeOpacity;
	}
	
	currentTitleOpacity += (hoverTitleOpacity-currentTitleOpacity)*.05;
}

void CloudsIntroSequence::updateQuestions(){
	

	for(int i = 0; i < startQuestions.size(); i++){
		CloudsPortal& curQuestion = startQuestions[i];
		curQuestion.scale = questionScale;
		curQuestion.update();
		
		if(curQuestion.hoverPosition.z < warpCamera.getPosition().z){
			curQuestion.hoverPosition.z += questionWrapDistance;
		}
		
		//hold the questions
		if(questionChannels[ curQuestion.tunnelQuadrantIndex ]){
			float slowDownFactor = powf(ofMap(ofGetElapsedTimef(),
											  channelPauseTime[curQuestion.tunnelQuadrantIndex] + questionPauseDuration,
											  channelPauseTime[curQuestion.tunnelQuadrantIndex] + questionPauseDuration+2, 1.0, 0.0, true), 2.0);
			if(&curQuestion == caughtQuestion){
				slowDownFactor = 1.0;
			}
			curQuestion.hoverPosition.z += cameraForwardSpeed * slowDownFactor;
			if(slowDownFactor == 0.0){
				questionChannels[ curQuestion.tunnelQuadrantIndex ] = false;
			}
			
		}
		else{
			float distanceToStoppingPoint = (curQuestion.hoverPosition.z - warpCamera.getPosition().z) - questionMinZDistance*.25;
			float slowDownFactor = powf(ofMap(distanceToStoppingPoint, 0.0, 300, 1.0, 0.0, true), 2.0);
			curQuestion.hoverPosition.z += slowDownFactor * cameraForwardSpeed;
			if(slowDownFactor > .9){
				questionChannels[curQuestion.tunnelQuadrantIndex] = true;
				channelPauseTime[curQuestion.tunnelQuadrantIndex] = ofGetElapsedTimef();
			}
		}

//		startQuestions[i].hoverPosition.z += ofMap(distanceToQuestion,
//												   questionTugDistance.max, questionTugDistance.min,
//												   0, cameraForwardSpeed);
		
		
		if(curQuestion.hoverPosition.z - warpCamera.getPosition().z < questionMinZDistance){
#ifdef OCULUS_RIFT
            ofVec3f screenPos = getOculusRift().worldToScreen(curQuestion.hoverPosition, true);
			ofRectangle viewport = getOculusRift().getOculusViewport();
            float distanceToQuestion = ofDist(screenPos.x, screenPos.y,viewport.getCenter().x, viewport.getCenter().y);
#else
            ofVec2f mouseNode = cursor;
			float distanceToQuestion = startQuestions[i].screenPosition.distance(mouseNode);
#endif
			if(caughtQuestion == NULL){
				if( distanceToQuestion < questionTugDistance.max ){
					if(distanceToQuestion < questionTugDistance.min){
						caughtQuestion = &curQuestion;
						if (caughtQuestion->startHovering()) {
                            CloudsPortalEventArgs args(getQuestionText());
                            ofNotifyEvent(events.portalHoverBegan, args);
                        }
					}
				}
			}
			
			//we have a caught question make sure it's still close
			else if(caughtQuestion == &curQuestion){
//				curQuestion.hoverPosition.z += cameraForwardSpeed;
				if( caughtQuestion->isSelected() && !bQuestionDebug){
					selectedQuestion = caughtQuestion;
				}
				else if(distanceToQuestion > questionTugDistance.max){
					caughtQuestion->stopHovering();
					caughtQuestion = NULL;
                    CloudsPortalEventArgs args(getQuestionText());
                    ofNotifyEvent(events.portalHoverEnded, args);
				}
			}
		}
	}
}


void CloudsIntroSequence::setStartQuestions(vector<CloudsClip>& possibleStartQuestions){

	selectedQuestion = NULL;
	startQuestions.clear();
	
	for(int i = 0; i < possibleStartQuestions.size(); i++){
		
		CloudsPortal q;
		q.cam = &warpCamera;
		q.bLookAtCamera = true;
//		q.font = &questionFont;
		q.clip = possibleStartQuestions[i];
		q.topic = q.clip.getAllTopicsWithQuestion()[0];
		q.question = q.clip.getQuestionForTopic(q.topic);

		q.setup();
		
		startQuestions.push_back(q);
	}
	timeSinceLastPrompt = ofGetElapsedTimef();
	positionStartQuestions();
}

void CloudsIntroSequence::generateTunnel(){

	// loose tunnel, with lines
	tunnelMeshLoose.clear();
	
	float looseTunnelStepZ = tunnelMax.z / looseTunnelResolutionZ;
	float looseTunnelStepY = tunnelMax.y / looseTunnelResolutionX;
	float looseTunnelStepX = tunnelMax.x / looseTunnelResolutionX;
	
	float tightTunnelStepZ = looseTunnelStepZ/2.0;
	float tightTunnelStepY = looseTunnelStepY/2.0;
	float tightTunnelStepX = looseTunnelStepX/2.0;

	for(float z = 0; z < tunnelMax.z; z += looseTunnelStepZ){
		//add invisible connector point
		tunnelMeshLoose.addColor(ofFloatColor(0,0));
		tunnelMeshLoose.addVertex(ofVec3f(-tunnelMax.x,-tunnelMax.y, z));
		
		//draw the top
		for(float x = -tunnelMax.x; x <= tunnelMax.x; x += tightTunnelStepX){
			tunnelMeshLoose.addColor(ofFloatColor::white);
			tunnelMeshLoose.addVertex(ofVec3f(x,-tunnelMax.y,z));
		}
		//draw right side
		for(float y = -tunnelMax.y; y <= tunnelMax.y; y += tightTunnelStepY){
			tunnelMeshLoose.addColor(ofFloatColor::white);
			tunnelMeshLoose.addVertex(ofVec3f(tunnelMax.x,y,z));
		}
		//draw bottom
		for(float x = tunnelMax.x; x >= -tunnelMax.x; x -= tightTunnelStepX){
			tunnelMeshLoose.addColor(ofFloatColor::white);
			tunnelMeshLoose.addVertex(ofVec3f(x,tunnelMax.y,z));
		}
		//draw the left side
		for(float y = tunnelMax.y; y >= -tunnelMax.y; y -= tightTunnelStepY){
			tunnelMeshLoose.addColor(ofFloatColor::white);
			tunnelMeshLoose.addVertex(ofVec3f(-tunnelMax.x,y,z));
		}
		tunnelMeshLoose.addColor(ofFloatColor(0,0));
		tunnelMeshLoose.addVertex(ofVec3f(-tunnelMax.x,-tunnelMax.y, z));
	}
	tunnelMeshLoose.setMode(OF_PRIMITIVE_LINE_STRIP);
	
	
	// tight tunnel, dots only
	tunnelMeshTight.clear();
	for(float z = 0; z < tunnelMax.z; z += tightTunnelStepZ){
		//draw the top
		for(float x = -tunnelMax.x; x <= tunnelMax.x; x += tightTunnelStepX){
			tunnelMeshTight.addVertex(ofVec3f(x,-tunnelMax.y,z));
		}
		//draw right side
		for(float y = -tunnelMax.y; y <= tunnelMax.y; y += tightTunnelStepY){
			tunnelMeshTight.addVertex(ofVec3f(tunnelMax.x,y,z));
		}
		//draw bottom
		for(float x = tunnelMax.x; x >= -tunnelMax.x; x -= tightTunnelStepX){
			tunnelMeshTight.addVertex(ofVec3f(x,tunnelMax.y,z));
		}
		//draw the left side
		for(float y = tunnelMax.y; y >= -tunnelMax.y; y -= tightTunnelStepY){
			tunnelMeshTight.addVertex(ofVec3f(-tunnelMax.x,y,z));
		}
	}	
}

void CloudsIntroSequence::positionStartQuestions(){

	//set the start questions along a random tunnel
//	for(int i = 0; i < startQuestions.size(); i++){
//		startQuestions[i].hoverPosition = ofVec3f(0, ofRandom(questionTunnelInnerRadius, tunnelMax.y), 0);
//		startQuestions[i].hoverPosition.rotate(ofRandom(360), ofVec3f(0,0,1));
//		startQuestions[i].hoverPosition.z = 400 + tunnelMax.z*.5 + ofRandom(questionWrapDistance);
//	}

	//new way with sets of 4
	for(int i = 0; i < startQuestions.size(); i++){
		startQuestions[i].tunnelQuadrantIndex = i%4;
		startQuestions[i].hoverPosition = ofVec3f(0, questionTunnelInnerRadius, 0);
		startQuestions[i].hoverPosition.rotate(i%4 * .25 * 360, ofVec3f(0,0,1));
		startQuestions[i].hoverPosition.z = 400 + tunnelMax.z*.5 + i * (1.0*questionWrapDistance / startQuestions.size() );
	}
}

bool CloudsIntroSequence::istStartQuestionHovering(){
	caughtQuestion != NULL;
}

string CloudsIntroSequence::getQuestionText(){
	if(selectedQuestion != NULL){
		return selectedQuestion->question;
	}
	else if(caughtQuestion != NULL){
		return caughtQuestion->question;
	}
	return "";
}

bool CloudsIntroSequence::isStartQuestionSelected(){
	return selectedQuestion != NULL;
}

void CloudsIntroSequence::autoSelectQuestion(){
	selectedQuestion = &startQuestions[ ofRandom(startQuestions.size()) ];
}

CloudsPortal* CloudsIntroSequence::getSelectedQuestion(){
	return selectedQuestion;
}

void CloudsIntroSequence::selfDrawBackground(){
	if(bQuestionDebug){
		ofPushStyle();
		
		for(int i = 0; i < startQuestions.size(); i++){
			float alpha = .1;
			if(startQuestions[i].hoverPosition.z - warpCamera.getPosition().z < questionMinZDistance){
				alpha = .8;
			}
			
			if(startQuestions[i].hovering){
				ofFill();
			}
			else{
				ofNoFill();
			}
			
			ofSetColor(startQuestions[i].hovering ? ofColor::green : ofColor::yellow, alpha*255);
			ofCircle(startQuestions[i].screenPosition, questionTugDistance.min);
			ofNoFill();
			ofSetColor(ofColor::white, alpha*255);
			ofCircle(startQuestions[i].screenPosition, questionTugDistance.max);
			
		}
		ofPopStyle();
	}
}

void CloudsIntroSequence::selfDrawDebug(){

}

void CloudsIntroSequence::timelineBangEvent(ofxTLBangEventArgs& args){
	
	CloudsVisualSystem::timelineBangEvent(args);
	if(args.flag == "LOOP_END"){
		ofxTLFlags* flags = (ofxTLFlags*)timeline->getTrack("Intro-Outro");
		ofxTLFlag* flag = flags->getFlagWithKey("LOOP_START");
		if(flag != NULL){
			timeline->stop();
			timeline->setCurrentTimeMillis(flag->time);
			timeline->play();
		}
		
		showingQuestions = true;
	}
	
	if(args.flag == "PROMPT"){
		CloudsPortalEventArgs args("SELECT A QUESTION TO BEGIN");
		ofNotifyEvent(events.portalHoverBegan, args);
	}
}

void CloudsIntroSequence::selfDraw(){
	
#if defined(OCULUS_RIFT) && defined(CLOUDS_APP)
    if (hud != NULL) {
        if(selectedQuestion != NULL){
            hud->draw3D(getOculusRift().baseCamera, ofVec2f(0, -selectedQuestion->screenPosition.y/2));
        }
        else if(caughtQuestion != NULL){
            hud->draw3D(getOculusRift().baseCamera, ofVec2f(0, -caughtQuestion->screenPosition.y/2));
        }
        else{
            hud->draw3D(getOculusRift().baseCamera);
        }
    }
#endif
    
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
	glEnable(GL_POINT_SMOOTH);
	glDisable(GL_DEPTH_TEST);
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	drawCloudsType();
	drawIntroNodes();
	drawHelperType();
	drawTunnel();
	drawPortals();
}

void CloudsIntroSequence::drawTunnel(){
	ofPushStyle();
	
	tunnelShader.begin();
	tunnelShader.setUniform1f("minPointSize", pointSize.min);
	tunnelShader.setUniform1f("maxPointSize", pointSize.max);
	tunnelShader.setUniform1f("minDistance", distanceRange.min);
	
	if(bUseOculusRift){
		tunnelShader.setUniform1f("maxDistance", distanceRange.max + currentTitleOpacity * 120.);
	}
	else{
		tunnelShader.setUniform1f("maxDistance", distanceRange.max + currentTitleOpacity * 200.);
	}
	
	tunnelShader.setUniform1f("cameraZ", warpCamera.getPosition().z);
	tunnelShader.setUniform1f("tunnelDepth", tunnelMax.z);
	tunnelShader.setUniform1f("noiseAmplitude", perlinAmplitude);
	tunnelShader.setUniform1f("noiseDensity", perlinDensity);
	perlinOffset += perlinSpeed;
	tunnelShader.setUniform1f("noisePosition", perlinOffset);
	tunnelShader.setUniform1f("colorAttenuate", 1.0);
	ofFloatColor tinted = ofFloatColor::fromHsb(tint.r, tint.g, tint.b);
	tunnelShader.setUniform4f("tint",tinted.r,tinted.g,tinted.b, 1.0);
	
	ofSetColor(255);
	ofEnablePointSprites();
	tunnelShader.setUniformTexture("sprite", sprite, 0);
	tunnelShader.setUniform1f("useSprite", 1);
	tunnelMeshTight.drawVertices();
	ofDisablePointSprites();
	
	ofSetLineWidth(1);
	tunnelShader.setUniform1f("colorAttenuate", wireframeAlpha);
	tunnelShader.setUniform1f("useSprite", 0);
	tunnelMeshLoose.draw();
	
	tunnelShader.end();
	
	ofPopStyle();
}

void CloudsIntroSequence::drawPortals(){
	ofPushStyle();
    ofEnableAlphaBlending();
	
	CloudsPortal::shader.begin();
	
	CloudsPortal::shader.setUniform1i("doAttenuate", 1);
	CloudsPortal::shader.setUniform1f("minDistance", questionAttenuateDistance.min);
	CloudsPortal::shader.setUniform1f("maxDistance", questionAttenuateDistance.max);
	
    ofSetColor(255);
	for(int i = 0; i < startQuestions.size(); i++){
		startQuestions[i].draw();
	}
	
	CloudsPortal::shader.end();
	
	ofPopStyle();
}

void CloudsIntroSequence::drawCloudsType(){
	ofPushMatrix();
	ofPushStyle();
	
	ofRotate(180, 0, 0, 1);
	
	ofEnableAlphaBlending();
	ofSetColor(255, currentTitleOpacity*255);
	ofTranslate(0, 0, titleTypeOffset );
	extrudedTitleText.setTracking( titleTypeTracking );
	float width  = extrudedTitleText.stringWidth("CLOUDS");
	float height = extrudedTitleText.stringHeight("CLOUDS");
	
	typeShader.begin();
	typeShader.setUniform1f("noisePosition",titleNoisePosition);
	typeShader.setUniform1f("noiseDensity",titleNoiseDensity);
	typeShader.setUniform1f("glowMin", titleMinGlow);
	typeShader.setUniform1f("glowMax", titleMaxGlow);

	extrudedTitleText.drawString("CLOUDS", -width/2, height/2);
	
	typeShader.end();

	ofPopStyle();
	ofPopMatrix();
}

void CloudsIntroSequence::drawHelperType(){
	ofPushMatrix();
	ofPushStyle();

	if(currentHelperFontSize != helperFontSize){
		helperFont.loadFont(GetCloudsDataPath() + "font/Blender-THIN.ttf", helperFontSize);
		currentHelperFontSize = helperFontSize;
	}
	
#ifdef OCULUS_RIFT
	if(!startedOnclick){
		
		ofRotate(180, 0, 0, 1);//flip around
		ofMatrix4x4 compensate;
		compensate.rotate(180, 0, 0, 1);
		
		string helpHoverText;
		ofVec3f basePosition(0,0,0);
		float helperTextOpacity = 0.0;
		if(introNodeThree.hover || introNodeTwo.finished){
			helpHoverText = "LOOK CENTER";
			basePosition = introNodeTwo.worldPosition;
			helperTextOpacity = powf(ofMap(ofGetElapsedTimef(), nodeActivatedTime+.1, nodeActivatedTime+1,0.0,.8,true), 2.) * (1.0-introNodeThree.percentComplete);
		}
		else if(introNodeTwo.hover || introNodeOne.finished){
			helpHoverText = "LOOK RIGHT";
			basePosition = introNodeOne.worldPosition;
			helperTextOpacity = powf(ofMap(ofGetElapsedTimef(), nodeActivatedTime+.1, nodeActivatedTime+1,0.0,.8,true), 2.);
		}
		else {
			helpHoverText = "LOOK LEFT";
			basePosition = introNodeThree.worldPosition;
			helperTextOpacity = (currentTitleOpacity - titleTypeOpacity)*(1.0-introNodeOne.percentComplete);
		}
		
		if(helpHoverText != ""){
			helperFont.setTracking(helperFontTracking);
			float hoverTextWidth  = helperFont.stringWidth(helpHoverText);
			float hoverTextHeight = helperFont.stringHeight(helpHoverText);
			
			getOculusRift().multBillboardMatrix( compensate.preMult( basePosition ) );
			ofScale(helperFontScale,helperFontScale,helperFontScale);

			ofSetColor(255,255*helperTextOpacity);
			helperFont.drawString(helpHoverText, -hoverTextWidth/2, helperFontY-hoverTextHeight/2);
		}
	}
	
#endif
	
	ofPopStyle();
	ofPopMatrix();
	
	
}

void CloudsIntroSequence::drawIntroNodes(){

#ifdef OCULUS_RIFT
	ofPushStyle();
	
	for(int i = 0; i < introNodes.size(); i++){
		ofPushMatrix();

		if(introNodes[i]->finished){
			ofFloatColor tinted = ofFloatColor::fromHsb(tint.r, tint.g, tint.b);
			ofSetColor(tinted, 200*nodeAlphaAttenuate);
		}
		else if(introNodes[i]->hover){
			ofSetColor(255,100,100,200);
		}
		else{
			ofSetColor(255,200);
		}
		
		getOculusRift().multBillboardMatrix(introNodes[i]->worldPosition);
		
		//ofTranslate( introNodes[i]->worldPosition );
		
		ofNoFill();
		ofCircle(0,0,0, introNodeSize);
		ofFill();
		ofCircle(0,0,0, introNodeSize*introNodes[i]->percentComplete);
		ofPopMatrix();
		
		if(!introNodes[i]->finished){
			break;
		}

	}
	
	ofPopStyle();
#endif
	
}

void CloudsIntroSequence::selfDrawOverlay(){

	//old oculus arrow draing code
//	
//	bool drawDirection = false;
//	float cursorScalar;
//	int maxCursorDistance = 400;
//	ofVec2f cursorDirection;
//	ofVec2f direction;
//	
//	helpHoverText = "";
//	if(introNodeThree.hover){
//		///no text
//	}
//	else if(introNodeTwo.finished){
//		drawDirection = true;
//		cursorScalar = ofMap(introNodeThree.cursorDistance, 0, maxCursorDistance, 0, 1.0,true);
//		cursorDirection = introNodeThree.cursorDirection;
//		helpHoverText = "";
//	}
//	else if(introNodeTwo.hover || introNodeOne.finished){
//		drawDirection = true;
//		cursorScalar = ofMap(introNodeTwo.cursorDistance, 0, maxCursorDistance, 0, 1.0,true);
//		cursorDirection = introNodeTwo.cursorDirection;
//		helpHoverText = "LOOK RIGHT";
//	}
//	else {
//		drawDirection = true;
//		cursorScalar = ofMap(introNodeOne.cursorDistance, 0, maxCursorDistance, 0, 1.0,true);
//		cursorDirection = introNodeOne.cursorDirection;
//		helpHoverText = "LOOK LEFT";
//	}
//	
//	if(false && drawDirection){
//		ofPushStyle();
//		ofSetColor(255, 200);
//		ofVec2f start(320,240);
//		hintCursorEndPoint += (start + cursorDirection*(1.0-powf(1.0-cursorScalar,3.0f))*40 - hintCursorEndPoint) * .1;
//		ofLine(start,hintCursorEndPoint);
//		ofPushMatrix();
//		ofTranslate(hintCursorEndPoint);
//		ofRotate( -(hintCursorEndPoint - start).normalized().angle(ofVec2f(0,1)) );
//		ofTriangle(ofVec2f(0,0), ofVec2f(-3,-6), ofVec2f(3,-6));
//		ofPopMatrix();
//		ofPopStyle();
//	}
	
//	if(helpHoverText != ""){
//		float hoverTextWidth  = questionFont.stringWidth(hoverText);
//		float hoverTextHeight = questionFont.stringHeight(hoverText);
//		questionFont.drawString(hoverText, 320 - hoverTextWidth/2, 240 + hoverTextHeight*1.5);
//	}

}

void CloudsIntroSequence::selfPostDraw(){
	
	CloudsVisualSystem::selfPostDraw();
#ifdef KINECT_INPUT
	//TODO: Move this to the general system drawing
	if(kinectHelperAlpha > 0.0){
		kinectHelperAlpha += (kinectHelperTargetAlpha-kinectHelperAlpha)*.05;
		ofPushStyle();
		ofSetColor(255,kinectHelperAlpha);
		((CloudsInputKinectOSC*) GetCloudsInput().get() )->debug(0,0, 200, 100);
	}
#endif

}

void CloudsIntroSequence::selfBegin(){
	timeline->stop();
	startedOnclick = false;
	selectedQuestion = NULL;
	for(int i = 0; i < startQuestions.size(); i++){
		startQuestions[i].stopHovering();
	}
}

void CloudsIntroSequence::selfEnd(){
	
}

void CloudsIntroSequence::selfExit(){
	
}

void CloudsIntroSequence::selfKeyPressed(ofKeyEventArgs & args){

	if(args.key == 'q'){
		if(selectedQuestion != NULL){
			selectedQuestion->stopHovering();
		}
		selectedQuestion = NULL;
	}
	if(args.key == 'R'){
		reloadShaders();
	}

}

void CloudsIntroSequence::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsIntroSequence::selfMouseDragged(ofMouseEventArgs& data){
    cursor.set(data.x, data.y, cursor.z);
}

void CloudsIntroSequence::selfMouseMoved(ofMouseEventArgs& data)
{
    cursor.set(data.x, data.y, cursor.z);
}

void CloudsIntroSequence::selfMousePressed(ofMouseEventArgs& data){
#if !defined(OCULUS_RIFT)
	if(!startedOnclick && startQuestions.size() > 0){
		startedOnclick  = true;
		timeline->play();
		
		CloudsPortalEventArgs args("");
		ofNotifyEvent(events.portalHoverEnded, args);
		timeSinceLastPrompt = ofGetElapsedTimef();
		promptShown = false;
	}
#endif
}

void CloudsIntroSequence::selfMouseReleased(ofMouseEventArgs& data){
	
}


void CloudsIntroSequence::guiRenderEvent(ofxUIEventArgs &e){
	
}
