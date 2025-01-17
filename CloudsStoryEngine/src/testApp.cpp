#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
	//ofToggleFullscreen();
    ofEnableSmoothing();
	
	currentAct = NULL;
	rebuildAct = false;
	
	parser.loadFromFiles();
	

	visualSystems.loadPresets();
	
	storyEngine.parser = &parser;
	storyEngine.visualSystems = &visualSystems;
	
	storyEngine.setup();
	storyEngine.printDecisions = false;
	storyEngine.toggleGuis();
	
	parser.printDichotomyRatios();
	
	//websockets.setup();
#ifndef CLOUDS_NO_OSC
	oscSender.setup();
#endif
	ofAddListener(storyEngine.getEvents().actCreated, this, &testApp::actCreated);

		//stringstream str;
		//for(int i = 0; i < parser.getAllClips().size(); i++){
		//	if(parser.getAllClips()[i]->hasQuestion()){
		//		for(int q = 0; q < parser.getAllClips()[i]->getQuestions().size(); q++){
		//			str << parser.getAllClips()[i]->getLinkName() << "	" << parser.getAllClips()[i]->getQuestions()[q] << endl;
		//		}
		//	}
		//}
		//ofBufferToFile("ALL_QUESTIONS.txt", ofBuffer(str));
	
}

//--------------------------------------------------------------
void testApp::actCreated(CloudsActEventArgs& args){
	
	if(currentAct != NULL){
		currentAct->unregisterEvents(this);
		#ifndef CLOUDS_NO_OSC
		currentAct->unregisterEvents(&oscSender);
		#endif
		delete currentAct;
	}

	currentAct = args.act;
	currentAct->registerEvents(this);
	#ifndef CLOUDS_NO_OSC
	currentAct->registerEvents(&oscSender);
	#endif
	currentAct->play();
    currentAct->getTimeline().enableEvents();
}

//--------------------------------------------------------------
void testApp::actBegan(CloudsActEventArgs& args){
//	cout << "act began" << endl;
}

//--------------------------------------------------------------
void testApp::actEnded(CloudsActEventArgs& args){
	
}

//--------------------------------------------------------------
void testApp::clipBegan(CloudsClipEventArgs& args){
	
}

//--------------------------------------------------------------
void testApp::visualSystemBegan(CloudsVisualSystemEventArgs& args){
	
}

//--------------------------------------------------------------
void testApp::visualSystemEnded(CloudsVisualSystemEventArgs& args){
	
}

//--------------------------------------------------------------
void testApp::topicChanged(CloudsTopicEventArgs& newTopic){
	
}

void testApp::questionProposed(CloudsQuestionEventArgs& args){
    
}

void testApp::questionSelected(CloudsQuestionEventArgs& args){
    
}

void testApp::preRollRequested(CloudsPreRollEventArgs& clip){
    
}
//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    if(currentAct != NULL){
		currentAct->drawDebug();
	}
}

//--------------------------------------------------------------
void testApp::exit(){
    storyEngine.saveGuiSettings();
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    ofxUIButton* b = (ofxUIButton*) e.widget;
    if(name == "BUILD ACT" &&  b->getValue() ){
        rebuildAct = true;
    }

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    if(key == 'h'){
        storyEngine.toggleGuis();
    }
    else if(key =='f'){
        ofToggleFullscreen();
    }
	if(key == 't'){
		storyEngine.positionGuis();
	}
	if(key == 'S'){
		storyEngine.saveGuiSettings();
	}

#ifndef CLOUDS_NO_OSC
	if(key == 'E'){
		vector<int> projectExampleIndecs;
		for(int i = 0; i < parser.getAllClips().size(); i++){
			if(parser.getAllClips()[i].hasProjectExample){
				projectExampleIndecs.push_back(i);
			}
		}
		
		if(projectExampleIndecs.size() > 0){
			int exampleIndex = projectExampleIndecs[ ofRandom(projectExampleIndecs.size()) ];
			oscSender.sendClip( parser.getAllClips()[exampleIndex]);
			cout << "SENT CLIP " << parser.getAllClips()[exampleIndex].getLinkName() << " WITH EXAMPLE " << parser.getAllClips()[exampleIndex].projectExampleTitle << endl;
		}
	}
	
	if(key == 'C'){
		oscSender.sendClip( parser.getAllClips()[ ofRandom(parser.getAllClips().size()) ] );
	}
#endif

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}