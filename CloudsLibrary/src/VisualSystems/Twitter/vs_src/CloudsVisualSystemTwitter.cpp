//
//  CloudsVisualSystemTwitter.cpp
//

#include "CloudsVisualSystemTwitter.h"
#include "CloudsRGBDVideoPlayer.h"

#include <Poco/URI.h>

bool dateSorter(Date const& lhs, Date const& rhs) {
    if (lhs.year != rhs.year)
        return lhs.year < rhs.year;
    if (lhs.month != rhs.month)
        return lhs.month < rhs.month;
    return lhs.day < rhs.day;
}

void CloudsVisualSystemTwitter::selfSetup()
{
    nodeModifier.r = 1.0;
    nodeModifier.g = 0.65;
    nodeModifier.b = 0.54;
    nodeModifier.a = 1.0;
    
    nodeActiveModifier.r = 1.0;
    nodeActiveModifier.g = 0.65;
    nodeActiveModifier.b = 0.54;
    nodeActiveModifier.a = 1.0;
    
    nodeMidpointModifier.r = 0.0;
    nodeMidpointModifier.g = 1.;
    nodeMidpointModifier.b = 1.;
    nodeMidpointModifier.a = 1.0;
    
    nodeActiveMidpointModifier.r = 0.5f;
    nodeActiveMidpointModifier.g = 0.65;
    nodeActiveMidpointModifier.b = 0.54;
    nodeActiveMidpointModifier.a = 1.0;
    
    tweetModifier.r = 1.0;
    tweetModifier.g = 0.65;
    tweetModifier.b = 0.54;
    tweetAlpha = 1.0;
    
    textColorModifier.r = 1;
    textColorModifier.g = 1;
    textColorModifier.b = 1;
    textColorModifier.a = 1;
    
    nodeColor.setHsb(nodeModifier.r, nodeModifier.g, nodeModifier.b,nodeModifier.a);
    nodeActiveColor.setHsb(nodeActiveModifier.r, nodeActiveModifier.g, nodeActiveModifier.b,nodeActiveModifier.a);
    nodeMidpointColor.setHsb(nodeMidpointColor.r, nodeMidpointColor.g, nodeMidpointColor.b,nodeMidpointColor.a);
    nodeActiveMidpointColor.setHsb(nodeActiveMidpointColor.r, nodeActiveMidpointColor.g, nodeActiveMidpointColor.b,nodeMidpointColor.a);
    textColor.setHsb(textColorModifier.r, textColorModifier.g, textColorModifier.b,textColorModifier.a);
    
    baseModifier.r = 0.62;
    baseModifier.g = 0.46;
    baseModifier.b = 0.90;
    baseAlpha = 0.01;
    
    baseColor  = ofFloatColor(0.0,0.0,1.0,0.1);
    tweetColor = ofFloatColor(1.0,0.0,0.0,1.0);
    
    refreshRate = 1000;
    edgeDecayRate = 0.8;
    meshExpansion = 100;
    pointSize =10;
    
    tweetModifier.r = 1.0;
    tweetModifier.g = 0.65;
    tweetModifier.b = 0.54;
    tweetAlpha = 1.0;
    
    baseModifier.r = 0.62;
    baseModifier.g = 0.46;
    baseModifier.b = 0.90;
    baseAlpha = 0.01;
    
    xScale = 1;
    yScale = 1;
    zScale = 10;
    rotateModel = false;
    initSystem(getVisualSystemDataPath() +"graphs/twitterOneUserMen.net");
    
    font.loadFont(getVisualSystemDataPath() + "fonts/MateriaPro_Light.otf");
    bRenderMesh = true;
    bRenderText = false;
    ofSetFrameRate(60);
}

void CloudsVisualSystemTwitter::selfBegin()
{
    
    ofEnableSmoothing();
    
    
}

void CloudsVisualSystemTwitter::selfSetupGui()
{
	clusterGui = new ofxUISuperCanvas("MESH PARAMS", gui);
    clusterGui->copyCanvasStyle(gui);
	clusterGui->copyCanvasProperties(gui);
    clusterGui->setName("Mesh");
    clusterGui->addButton("LOAD GRAPH", false);
	clusterGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    clusterGui->addToggle("RENDER MESH", &bRenderMesh);
    clusterGui->addIntSlider("REFRESH RATE", 1, 100, &refreshRate);
    clusterGui->addMinimalSlider("EDGE DECAY", 0.01, 0.9   , &edgeDecayRate);
    clusterGui->addSpacer();
    clusterGui->addMinimalSlider("TWEET HUE", 0.0, 1.0,&tweetModifier.r);
    clusterGui->addMinimalSlider("TWEET SAT", 0.0, 1.0, &tweetModifier.g);
    clusterGui->addMinimalSlider("TWEET BRI", 0.0, 1.0, &tweetModifier.b);
    clusterGui->addMinimalSlider("TWEET ALPHA", 0.0, 1.0, &tweetAlpha);
    clusterGui->addSpacer();
    clusterGui->addMinimalSlider("BASE HUE", 0.0, 1.0, &baseModifier.r);
    clusterGui->addMinimalSlider("BASE SAT", 0.0, 1.0, &baseModifier.g);
    clusterGui->addMinimalSlider("BASE BRI", 0.0, 1.0, &baseModifier.b);
    clusterGui->addMinimalSlider("BASE ALPHA", 0.0, 1.0, &baseAlpha);
    clusterGui->addSpacer();
    clusterGui->addMinimalSlider("NODE M HUE", 0.0, 1.0, &nodeMidpointModifier.r);
    clusterGui->addMinimalSlider("NODE M SAT", 0.0, 1.0, &nodeMidpointModifier.g);
    clusterGui->addMinimalSlider("NODE M BRI", 0.0, 1.0, &nodeMidpointModifier.b);
    clusterGui->addMinimalSlider("NODE M ALPHA", 0.0, 1.0, &nodeMidpointModifier.a);
    clusterGui->addSpacer();
    clusterGui->addMinimalSlider("NODE AM HUE", 0.0, 1.0, &nodeActiveMidpointModifier.r);
    clusterGui->addMinimalSlider("NODE AM SAT", 0.0, 1.0, &nodeActiveMidpointModifier.g);
    clusterGui->addMinimalSlider("NODE AM BRI", 0.0, 1.0, &nodeActiveMidpointModifier.b);
    clusterGui->addMinimalSlider("NODE AM ALPHA", 0.0, 1.0, &nodeActiveMidpointModifier.a);
    clusterGui->addSpacer();
    clusterGui->addMinimalSlider("NODE HUE", 0.0, 1.0, &nodeModifier.r);
    clusterGui->addMinimalSlider("NODE SAT", 0.0, 1.0, &nodeModifier.g);
    clusterGui->addMinimalSlider("NODE BRI", 0.0, 1.0, &nodeModifier.b);
    clusterGui->addMinimalSlider("NODE ALPHA", 0.0, 1.0, &nodeModifier.a);
    clusterGui->addSpacer();
    clusterGui->addMinimalSlider("NODEA HUE", 0.0, 1.0, &nodeActiveModifier.r);
    clusterGui->addMinimalSlider("NODEA SAT", 0.0, 1.0, &nodeActiveModifier.g);
    clusterGui->addMinimalSlider("NODEA BRI", 0.0, 1.0, &nodeActiveModifier.b);
    clusterGui->addMinimalSlider("NODEA ALPHA", 0.0, 1.0, &nodeActiveModifier.a);
    clusterGui->addMinimalSlider("X POS", 1, 100, &xScale);
    clusterGui->addMinimalSlider("Y POS", 1, 100, &yScale);
    clusterGui->addMinimalSlider("Z POS", 1, 100, &zScale);
    clusterGui->addButton("RELOAD MESH", false);
    clusterGui->addSpacer();
    clusterGui->addToggle("ROTATE", &rotateModel);
    
	ofAddListener(clusterGui->newGUIEvent, this, &CloudsVisualSystemTwitter::selfGuiEvent);
	guis.push_back(clusterGui);
	guimap[clusterGui->getName()] = clusterGui;
    
    textGui = new ofxUISuperCanvas("TEXT PARAMS", gui);
    textGui->copyCanvasStyle(gui);
	textGui->copyCanvasProperties(gui);
    textGui->setName("text");
    textGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    textGui->addToggle("RENDER TEXT", &bRenderText);
    textGui->addSpacer();
    textGui->addMinimalSlider("TEXT HUE", 0.0, 0.1, &textColorModifier.r);
    textGui->addMinimalSlider("TEXT SAT", 0.0, 0.1, &textColorModifier.g);
    textGui->addMinimalSlider("TEXT BRI", 0.0, 0.1, &textColorModifier.b);
    textGui->addMinimalSlider("TEXT ALPHA", 0.0, 0.1, &textColorModifier.a);
    textGui->addSpacer();
    
    vector<string> radioBillboard;
    radioBillboard.push_back("BILLBOARD SCREEN");
    radioBillboard.push_back("BILLBOARD NODES");
    radioBillboard.push_back("BILLBOARD ORIGIN");
    textGui->addRadio("BILLBOARD", radioBillboard);
    
    
    ofAddListener(textGui->newGUIEvent, this, &CloudsVisualSystemTwitter::selfGuiEvent);
	guis.push_back(textGui);
	guimap[textGui->getName()] = textGui;
}

void CloudsVisualSystemTwitter::loadJSONData(){
    
    ofFile f = ofFile();
    
    ofDirectory dir(getVisualSystemDataPath()+"tweets/");
    dir.listDir();
    if(dir.exists()){
        int size = dir.size();
        vector<ofFile>files= dir.getFiles();
        
        for(int i = 0; i< files.size(); i++){
            string filePath =getVisualSystemDataPath()+"tweets/" +files[i].getFileName();
            
            bool parsingSuccessful = result.openLocal(filePath);
            if (parsingSuccessful) {
                
                if(result.isMember("errors")) {
                    ofDrawBitmapString(result.getRawString(), 10, 14);
                }
                else if(result.isMember("Tweets")){
                    
                    Tweeter cur;
                    vector<Tweet> userTweets;
                    ofxJSONElement tweets = result["Tweets"];
                    
                    vector<string> names = ofSplitString(result["name"].asString(), ".")    ;
                    cur.name = "@" + names[0];
                    cur.ID = i;
                    
                    for(int j =0; j<tweets.size(); j ++){
                        Tweet t;
                        t.tweet = tweets[j]["Tweet"].asString();
                        
                        if(tweets[j]["Hashtag"].isValidIndex(0)){
                            ofxJSONElement hashTags = tweets[j]["Hashtag"];
                            
                            for(int k=0; k<hashTags.size(); k++){
                                t.hashtags.push_back(hashTags[k ].asString());
                            }
                        }
                        
                        if(tweets[j]["Users"].isValidIndex(0)){
                            ofxJSONElement users = tweets[j]["Users"];
                            
                            for(int k=0; k<users.size(); k++){
                                
                                if( !ofContains(cur.userLinks, users[k].asString())){
                                    t.mentionedUsers.push_back(users[k].asString());
                                    cur.userLinks.push_back(users[k].asString());
                                }
                            }
                        }
                        if(tweets[j].isMember("Date")){
                            bool alreadyExists = false;
                            ofxJSONElement date = tweets[j]["Date"];
                            t.tweetDate.day = date["Day"].asInt()   ;
                            t.tweetDate.month =date["Month"].asInt();
                            t.tweetDate.year =date["Year"].asInt();
                            
                            for(int i=0; i<dateIndex.size(); i++){
                                
                                if(dateIndex[i].year == t.tweetDate.year && dateIndex[i].month == t.tweetDate.month && dateIndex[i].day == t.tweetDate.day){
                                    alreadyExists = true;
                                    break;
                                }
                            }
                            if(! alreadyExists){
                                dateIndex.push_back(t.tweetDate);
                                dateIndexMap[getDateAsString(t.tweetDate)] = dateIndex.size()-1;
                            }
                        }
                        userTweets.push_back(t);
                    }
                    cur.tweets= userTweets;
                    tweeters.push_back(cur);
                } else {
                    cout  << "Failed to parse JSON" << endl;
                }
            }
        }
    }
    addUsersFromMentions();
}

void CloudsVisualSystemTwitter::clearData(){
    dateIndex.clear();
    dateIndexMap.clear();
    links.clear();
    lineIndexPairs.clear();
    numberOfMentions.clear();
    
}
void CloudsVisualSystemTwitter::parseClusterNetwork(string fileName){
    
    ofBuffer pajekFile = ofBufferFromFile(fileName);
	bool findingNodes = false;
    bool findingEdges = false;;
	while(!pajekFile.isLastLine()){
		string line = pajekFile.getNextLine();
		
		if(line == "" || line.at(0) == '%'){
			continue;
		}
		
		if (line.find("*Vertices") != string::npos ) {
			findingNodes = true;
			continue;
            
		}
		if (line.find("*Edgeslist") != string::npos ) {
			findingNodes = false;
            findingEdges = true;
            continue;
		}
		
		if(findingNodes){
			vector<string> components = ofSplitString(line, " ");
            
            int id = ofToInt(components[0]);
            
            Tweeter& tweeter = getTweeterByID(tweeters, id);
            
			int numcomp = components.size();
            max = ofVec3f(0,0,0);
            
			tweeter.position = ofVec3f(ofToFloat(components[2])*xScale,
                                       ofToFloat(components[3])*yScale,
                                       ofToFloat(components[4])*zScale);
		}
        
        if(findingEdges){
			vector<string> components = ofSplitString(line, " ");
            int id = ofToInt(components[0]);
            //428 4 8 9 11 15 17 18
            
            Tweeter& tweeter = getTweeterByID(tweeters, id);
            for(int i =1; i< components.size()-1; i++){
                if(tweeter.ID != ofToInt(components[i]) ){
                    tweeter.linksById.push_back(ofToInt(components[i]));
                }
                else{
                    cout<<"Error! "<<tweeter.name<<"  : "<<tweeter.ID<<" index "<< ofToInt(components[i])<<endl;
                }
            }
        }
        
	}
}
void CloudsVisualSystemTwitter::updateMeshFromTweets(int index){
    
    for(int i=0; i<tweeters.size(); i++){
        if(tweeters[i].hasTweetOnDate(dateIndex[index])){
            vector<Tweet> tweetsOnDate = tweeters[i].getTweetsByDate(dateIndex[index]);
            
            for(int k=0; k<tweetsOnDate.size();k ++){
                
                for(int l=0; l<tweetsOnDate[k].mentionedUsers.size(); l++){
                    int user = getUserIdByName(tweetsOnDate[k].mentionedUsers[l]);
                    if(user != -1){
                        
                        Tweeter& t  = getTweeterByID(tweeters,user);
                        if(lineIndexPairs.find(make_pair(tweeters[i].name, t.name)) != lineIndexPairs.end()){
                            
                            pair<int, int> currentIndeces = lineIndexPairs[make_pair(tweeters[i].name, t.name)];
                            edgeMesh.setColor(currentIndeces.first, tweetColor);
                            edgeMesh.setColor(currentIndeces.second, tweetColor);
                            int ind = MIN(currentIndeces.first,currentIndeces.second);
                            ind++;
                            edgeMesh.setColor(ind, nodeActiveMidpointColor);
                        }
                        else if(lineIndexPairs.find(make_pair(t.name,tweeters[i].name)) != lineIndexPairs.end()){
                            
                            pair<int, int> currentIndeces = lineIndexPairs[make_pair(tweeters[i].name, t.name)];
                            edgeMesh.setColor(currentIndeces.first, tweetColor);
                            edgeMesh.setColor(currentIndeces.second, tweetColor);
                            int ind = MIN(currentIndeces.first,currentIndeces.second);
                            ind++;
                            edgeMesh.setColor(ind, nodeActiveMidpointColor);
                        }
                        else{
                        }
                    }
                }
            }
            nodeMesh.setColor(tweeters[i].nodeVertexIndex, nodeActiveColor);
        }
    }
    
    
    
}
void CloudsVisualSystemTwitter::updateMesh(){
    
    for(int i= 0; i<nodeMesh.getColors().size(); i++){
        
        ofFloatColor c = nodeMesh.getColor(i);
        if(c != nodeColor){
            float h = ofLerp(c.getHue(), nodeColor.getHue(), edgeDecayRate);
            float s = ofLerp(c.getSaturation(), nodeColor.getSaturation(), edgeDecayRate);
            float b = ofLerp(c.getBrightness() , nodeColor.getBrightness(),edgeDecayRate);
            float a =ofLerp(c.a , nodeColor.a,edgeDecayRate);
            c.setHsb(h, s, b);
            c.a = a;
            nodeMesh.setColor(i, c);
        }
        else{
            nodeMesh.setColor(i, c);
        }
    }
    
    for(int i= 0; i<edgeMesh.getColors().size(); i++){
        ofFloatColor c = edgeMesh.getColor(i);
        
        if(edgeMesh.getNormal(i).y == 1){
            if(c != nodeMidpointColor){
                float h = ofLerp(c.getHue(), nodeMidpointColor.getHue(), edgeDecayRate);
                float s = ofLerp(c.getSaturation(), nodeMidpointColor.getSaturation(), edgeDecayRate);
                float b = ofLerp(c.getBrightness() , nodeMidpointColor.getBrightness(),edgeDecayRate);
                float a =ofLerp(c.a , nodeMidpointColor.a,edgeDecayRate);
                
                c.setHsb(h, s, b);
                c.a = a;
                
                if(c == ofFloatColor::black){
                    edgeMesh.setColor(i, nodeMidpointColor);
                }
                else{
                    edgeMesh.setColor(i,c);
                }
            }
            else{
                edgeMesh.setColor(i,nodeMidpointColor);
            }
        }
        else{
            if(c != baseColor){
                
                float h = ofLerp(c.getHue(), baseColor.getHue(), edgeDecayRate);
                float s = ofLerp(c.getSaturation(), baseColor.getSaturation(), edgeDecayRate);
                float b = ofLerp(c.getBrightness() , baseColor.getBrightness(),edgeDecayRate);
                float a =ofLerp(c.a , baseColor.a,edgeDecayRate);
                
                c.setHsb(h, s, b);
                c.a = a;
                
                if(c == ofFloatColor::black){
                    edgeMesh.setColor(i, baseColor);
                }
                else{
                    edgeMesh.setColor(i,c);
                }
            }
            else{
                edgeMesh.setColor(i, baseColor);
            }
        }
        
    }
}

void CloudsVisualSystemTwitter::reloadMeshColor(){
    for(int i=0; i<edgeMesh.getVertices().size();i++){
        edgeMesh.setColor(i, baseColor);
    }
}

void CloudsVisualSystemTwitter::loadMesh(){
    
    edgeMesh.clear();
    nodeMesh.clear();
    int  currentIndex =0;
    
    for(int j=0; j<tweeters.size(); j++){
        
        for (int k=0; k<tweeters[j].linksById.size(); k++) {
            Tweeter& t  = getTweeterByID(tweeters, tweeters[j].linksById[k]);
            if(lineIndexPairs.find(make_pair(tweeters[j].name, t.name)) == lineIndexPairs.end() &&
               lineIndexPairs.find(make_pair(t.name,tweeters[j].name)) == lineIndexPairs.end() ){
                edgeMesh.addVertex(tweeters[j].position);
                edgeMesh.addNormal(ofVec3f(0,0,0));
                edgeMesh.addColor(baseColor);
                tweeters[j].edgeVertexIndex = currentIndex;
                currentIndex++;
                
                //adding midpoint
                float x = ofLerp(tweeters[j].position.x, t.position.x, 0.5);
                float y = ofLerp(tweeters[j].position.y, t.position.y, 0.5);
                float z = ofLerp(tweeters[j].position.z, t.position.z, 0.5);
                edgeMesh.addVertex(ofVec3f(x,y,z));
                edgeMesh.addNormal(ofVec3f(0,1,0));
                edgeMesh.addColor(nodeMidpointColor);
                currentIndex++;
                
                edgeMesh.addVertex(t.position);
                edgeMesh.addNormal(ofVec3f(0,0,0));
                edgeMesh.addColor(baseColor);
                t.edgeVertexIndex = currentIndex;
                
                links.insert(make_pair(tweeters[j].ID, tweeters[j].linksById[k]));
                lineIndexPairs[make_pair(tweeters[j].name, t.name) ] = make_pair(currentIndex-2, currentIndex);
                currentIndex++;
                
            }
            else{
                //                cout<<"alread a link between "<< tweeters[j].name << " and "<<t.name<<endl;
            }
        }
    }
    edgeMesh.setMode(OF_PRIMITIVE_LINES);
    
    cout<<"No of vertices in edges "<< edgeMesh.getVertices().size()<<endl;
    currentIndex = 0;
    for(int j=0; j<tweeters.size(); j++){
        
        nodeMesh.addVertex(tweeters[j].position);
        nodeMesh.addNormal(ofVec3f(0,0,0));
        nodeMesh.addColor(ofFloatColor(nodeColor));
        tweeters[j].nodeVertexIndex = currentIndex;
        currentIndex++;
    }
    cout<<"No of vertices in node "<<  nodeMesh.getVertices().size()<<endl;
    
    nodeMesh.setMode(OF_PRIMITIVE_POINTS);
}

void CloudsVisualSystemTwitter::addUsersFromMentions(){
    
    vector<string> names;
    
    for (int i= 0; i < tweeters.size(); i++) {
        names.push_back(tweeters[i].name);
    }
    
    for (int i= 0; i < tweeters.size(); i++) {
        for(int j=0; j<tweeters[i].userLinks.size(); j++){
            
            if(! ofContains(names, tweeters[i].userLinks[j])){
                numberOfMentions[tweeters[i].userLinks[j]] ++;
            }
        }
    }
    
    map<string,int>::iterator it;
    for(it = numberOfMentions.begin() ; it != numberOfMentions.end() ; it++){
        //Filter mentioned users by times mentioned
        if(it->second > 1){
            Tweeter t = Tweeter(it->first, tweeters.size());
            tweeters.push_back(t);
        }
    }
}


void CloudsVisualSystemTwitter::createPajekNetwork(){
    stringstream ss;
    cout<<"Creating paejk file"<<endl;
    ss<<"*Vertices "<<tweeters.size()<<endl;
    
    for (int i= 0; i < tweeters.size(); i++) {
        ss<<tweeters[i].ID<<"  \""<<tweeters[i].name<<"\""<<endl;
    }
    
    ss<<"*EdgesList "<<tweeters.size()<<endl;
    for(int j =0; j<tweeters.size(); j++){
        
        string edges;
        if(tweeters[j].userLinks.size() > 0){
            for(int k=0; k< tweeters[j].userLinks.size(); k++){
                
                if(getUserIdByName(tweeters[j].userLinks[k]) != -1){
                    edges += ofToString(getUserIdByName(tweeters[j].userLinks[k])) +  " ";
                }
            }
            ss<<tweeters[j].ID<<" "<<edges<<endl;
        }
    }
    ofBuffer b = ofBuffer(ss);
    ofBufferToFile(getVisualSystemDataPath() + "/twitter.net",b);
}

int CloudsVisualSystemTwitter:: getUserIdByName(string name){
    
    for (int i=0; i<tweeters.size(); i++) {
        if(tweeters[i].name == name){
            return tweeters[i].ID;
        }
    }
    return -1;
}

vector<Tweeter> CloudsVisualSystemTwitter::getTweetersForDate(int index){
    vector<Tweeter> tOnDate;
    
    for(int k=0; k<tweeters.size(); k++){
        if(tweeters[k].hasTweetOnDate(dateIndex[index])){
            tOnDate.push_back(tweeters[k]);
        }
    }
    
    return tOnDate;
}

void CloudsVisualSystemTwitter::drawTweetsForDate(int index){
    
    for(int k=0; k<tweeters.size(); k++){
        vector<Tweet> tweets =  tweeters[k].getTweetsByDate(dateIndex[index]);
        
        if(tweets.size() > 0 ){
            
            for(int j = 0; j<tweets.size(); j++){
                ss<<tweets[j].tweet<<" "<<getDateAsString(dateIndex[index])<<endl;
            }
        }
    }
    
}
string CloudsVisualSystemTwitter::getDateAsString(Date d){
    string dateString;
    
    dateString += ofToString(d.day) + " , ";
    dateString += ofToString(d.month) + " , ";
    dateString += ofToString(d.year);
    return dateString;
}


Tweeter& CloudsVisualSystemTwitter::getTweeterByID(vector<Tweeter>& tweeters, int _id ){
    
    for(int i=0; i< tweeters.size(); i++){
        if(tweeters[i].ID == _id){
            return tweeters[i];
        }
    }
}

void CloudsVisualSystemTwitter::CompareDates(Date d1,Date d2){
    
}

//--------------------------------------------------------------
void CloudsVisualSystemTwitter::selfGuiEvent(ofxUIEventArgs &e)
{
    
    if (e.widget->getName() == "BILLBOARD SCREEN") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            billboardType = 0;
        }
    }
    else if (e.widget->getName() == "BILLBOARD NODES") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            billboardType = 1;
        }
    }
    else if (e.widget->getName() == "BILLBOARD ORIGIN") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            billboardType = 2;
        }
    }
    else if(e.getName() == "LOAD GRAPH"){
        ofxUIButton* t  = (ofxUIButton*) e.widget;
        if (t->getValue()) {
            ofFileDialogResult result = ofSystemLoadDialog("Load Images From Folder", false, getVisualSystemDataPath() +"graphs/");
            
            if(result.bSuccess && result.fileName.length())
            {
                loadGraphFromPath(result.filePath);
            }
        }
    }
    else if (e.getName() == "RELOAD MESH")
    {
        initSystem(currentMeshFilePath);
        
    }
    baseColor.setHsb(baseModifier.r, baseModifier.g, baseModifier.b);
    baseColor.a = baseAlpha;
    tweetColor.setHsb(tweetModifier.r, tweetModifier.g, tweetModifier.b);
    tweetColor.a =tweetAlpha;
    nodeColor.setHsb(nodeModifier.r, nodeModifier.g, nodeModifier.b,nodeModifier.a);
    nodeActiveColor.setHsb(nodeActiveModifier.r, nodeActiveModifier.g, nodeActiveModifier.b,nodeActiveModifier.a);
    nodeMidpointColor.setHsb(nodeMidpointModifier.r, nodeMidpointModifier.g, nodeMidpointModifier.g,nodeActiveMidpointModifier.a);
    nodeActiveMidpointColor.setHsb(nodeActiveMidpointModifier.r, nodeActiveMidpointModifier.g, nodeActiveMidpointModifier.b,nodeMidpointModifier.a);
    textColor.setHsb(textColorModifier.r, textColorModifier.g, textColorModifier.b,textColorModifier.a);
    reloadMeshColor();
}

void CloudsVisualSystemTwitter::initSystem(string filePath){
    currentMeshFilePath = filePath;
    clearData();
    loadJSONData();
    parseClusterNetwork(filePath);
    loadMesh();
    std::sort(dateIndex.begin(), dateIndex.end(), &dateSorter);
    currentDateIndex = dateIndex.size() -1;
    updateMeshFromTweets(currentDateIndex);
    reloadMeshColor();
}

void CloudsVisualSystemTwitter::loadGraphFromPath(string filePath){
    cout<<filePath<<endl;
    
    if(ofFile::doesFileExist(filePath)){
        initSystem(filePath);
    }
    else{
        cout<<filePath<<" does not exist "<<endl;
    }
    
}
//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemTwitter::selfSetupSystemGui(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemTwitter::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemTwitter::selfSetupRenderGui(){
    
}

void CloudsVisualSystemTwitter::guiRenderEvent(ofxUIEventArgs &e){
	
}


//--------------------------------------------------------------
// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemTwitter::selfPresetLoaded(string presetPath)
{
	
}


//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemTwitter::selfSceneTransformation(){
    
    if(rotateModel){
        ofRotateZ(ofGetFrameNum()%360);
    }
    
}

//--------------------------------------------------------------
//normal update call
void CloudsVisualSystemTwitter::selfUpdate()
{
    if(  ofGetFrameNum() %refreshRate <1){
        currentDateIndex--;
        if (currentDateIndex <= 0) {
            currentDateIndex = dateIndex.size() - 1;
        }
        updateMeshFromTweets(currentDateIndex);
    }
    updateMesh();
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemTwitter::selfDraw()
{
    ofPushStyle();
    ofPushMatrix();
    ofEnableBlendMode(OF_BLENDMODE_ADD  );
    ofSetBackgroundColor(0,0,0);
    glDisable(GL_DEPTH_TEST);
    ofScale(10, 10);
    
    
    if(bRenderMesh){
        glPointSize(1);
        nodeMesh.draw();
        edgeMesh.draw();
        
    }
    
    if(bRenderText){
        drawText(ofVec3f(10,10,10));
//        cout<<"Drawing text"<<endl;
//        ofDrawBitmapString(" HERE ", 0,0,0);
    }
    ofPopMatrix();
    ofPopStyle();
    
}

// draw any debug stuff here
void CloudsVisualSystemTwitter::selfDrawDebug()
{
    //    ofPushStyle();
    //    ofSetColor(ofColor::white);
    //    string cur = getDateAsString( dateIndex[currentDateIndex]);
    //    ofDrawBitmapString(cur , 0,0);
    //    ofCircle(0, 0, 200);
    //    ofPopStyle();
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemTwitter::selfDrawBackground()
{
    
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemTwitter::selfEnd()
{
    
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemTwitter::selfExit()
{
    
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemTwitter::selfKeyPressed(ofKeyEventArgs & args){
    if(args.key =='k'){
        std::sort(dateIndex.begin(), dateIndex.end(), &dateSorter);
        
        for(int i =0; i<dateIndex.size(); i++){
            cout<<dateIndex[i].day<<" : "<<dateIndex[i].month<<" : "<<dateIndex[i].year<<endl;
        }
    }
    if(args.key == 'j'){
        cout<<baseColor.getHue()<<" , "<<baseColor.getSaturation()<<" , "<<baseColor.getBrightness()<<endl;
    }
    
}
void CloudsVisualSystemTwitter::drawText(ofVec3f pos){
    ofPushMatrix();
    {
        
//        getCameraRef().setScale(1,-1,1);
//        ofVec3f pos =ofVec3f(0,0,0);
        
        if (billboardType == 0) {  // SCREEN
            ofxBillboardBeginSphericalCheat(pos);
        }
        else {
            // ORIGIN
            ofxBillboardBeginSphericalObvious(getCameraPosition(), pos);
        }
        ofPushStyle();
//        ofSetColor(textColor);
        ofSetColor(255, 0, 0);
        ofScale(1,-1,1);
//        getCameraRef().setScale(1,-1,-1);
        //ofDrawBitmapString(" HERE ", pos);
        
        //for (int i=0; i<tweeters.size(); i++) {
        //                float  x= tweeters[i].position.x;
        //                float y = tweeters[i].position.y;
//        float x = -0.5 * font.getLineLength();
        font.drawString("Here",0,0);

        ofPopStyle();

        ofxBillboardEnd();
        getCameraRef().setScale(1,1,1);
    }
    ofPopMatrix();
}

void CloudsVisualSystemTwitter::selfKeyReleased(ofKeyEventArgs & args){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemTwitter::selfMouseDragged(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemTwitter::selfMouseMoved(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemTwitter::selfMousePressed(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemTwitter::selfMouseReleased(ofMouseEventArgs& data){
	
}
