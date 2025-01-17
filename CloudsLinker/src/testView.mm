#import "testView.h"
#include "CloudsClip.h"
#include "CloudsGlobal.h"

@implementation testView

@synthesize clipEndFrame;
@synthesize preview;

- (void)setup
{
	ofSeedRandom();
	
    ofBackground(220);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
	ofSetFrameRate(30);
	
	movieSuccessfullyLoaded = false;
    
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX);
    CFRelease(resourcesURL);
    chdir(path);
    
    long size;
    char *buf;
    char *ptr;
    
    size = pathconf(".", _PC_PATH_MAX);
    if ((buf = (char *)malloc((size_t)size)) != NULL){
        ptr = getcwd(buf, (size_t)size);
    }
    cout << ptr << endl;
    
    
	parser.loadFromFiles();
    clipEndFrame = 0;
    clipLoaded = NO;

	
	font.loadFont("materiapro_light.ttf", 12);
	
	cout << ofToString(parser.percentOfClipsLinked(), 4) << " percent clips linked!" << endl;
	
    linkerA.testViewParent = self;
    linkerB.testViewParent = self;
    linkerA.parser = &parser;
    linkerB.parser = &parser;
	[linkerA setup];
    [linkerB setup];
    
	vector<CloudsClip*> projectExamples = parser.getClipsWithKeyword("#example");
	for(int i = 0; i < projectExamples.size(); i++){
		cout << projectExamples[i]->getSpeakerFirstName() << " " << projectExamples[i]->getSpeakerLastName() << ": " << projectExamples[i]->name << endl;
	}
	
    
//	if(!ofFile::doesFileExist(GetCloudsDataPath() + "CloudsMovieDirectory.txt")){
//		ofSystemAlertDialog("Could not find movie file path. \
//							Create a file called CloudsMovieDirectory.txt \
//							that contains one line, the path to your movies folder");
//	}
	
	//parser.setCombinedVideoDirectory(ofBufferFromFile(GetCloudsDataPath() + "CloudsMovieDirectory.txt").getText());
    //parser.loadFromFiles();
    
	CloudsExporter::saveGephiCSV(parser);
	CloudsExporter::savePajekNetwork(parser);
}


- (void) updateViews
{
	[linkerA updateTables];
	[linkerB updateTables];
}

- (IBAction)stopPreview:(id)sender
{
    preview.stop();
}

- (float) clipPercentComplete
{
    
	if(movieSuccessfullyLoaded && preview.isLoaded() && preview.isPlaying()){
		return (1.0*preview.getCurrentFrame() - currentPlayingClip->startFrame) / (currentPlayingClip->endFrame - currentPlayingClip->startFrame);
	}
	return 0;
}

- (IBAction) togglePlay:(id)sender
{
	if(preview.isLoaded()){
		if(preview.isPlaying()){
			playingRGBD = false;
			preview.stop();
		}
		else{
			playingRGBD = true;
			preview.play();
		}
	}
}


- (void)update
{
	
	//most of the time we want to wait for a clip to finish or be interrupted by user interaction
	if(playingRGBD){
		preview.update();
		if(preview.isLoaded()) {
//			if(!preview.isPlaying()){
//				preview.play();
//			}
//			if(preview.getCurrentFrame() >= clipEndFrame){
//				playingRGBD = false;
//				preview.stop();
//			}
		}
	}
}

- (void)draw
{
	ofBackground(0);
    if(preview.isLoaded()){
		ofRectangle screenRect(0,0, ofGetWidth(), ofGetHeight());
		ofRectangle videoRect(0,0,1920,1080);
		videoRect.scaleTo(screenRect);
		
        preview.draw(videoRect);
		
		font.drawString(currentPlayingClip->getLinkName() + "\n" + ofJoinString(currentPlayingClip->getKeywords(), ", "), 10, ofGetHeight() - font.getLineHeight()*2 - 10);
    }	
}

- (void) exit
{
	NSLog(@"exit!!");
}

- (void)selectClip:(CloudsClip*)clip inAlternateTable:(id)sender
{
	if(sender == linkerA){
		[linkerB selectClip:clip];
	}
	else{
		[linkerA selectClip:clip];
	}
}

- (void)keyPressed:(int)key
{
	
	if(key == ' '){
		[self togglePlay:self];
	}
	
	cout << "KEY " << key << endl;
}

-(IBAction)suppressClips:(id)sender
{
    if([linkerB isClipSelected]&&[linkerA isClipSelected]){
        [self suppressLink:[linkerA selectedClip]
					toClip:[linkerB selectedClip]];
        [linkerA updateTables];
        [linkerB updateTables];
		
		[self saveLinks:self];
    }
}

-(IBAction)linkFromLeftToRight:(id)sender
{
    if([linkerB isClipSelected]&&[linkerA isClipSelected]){
        [self linkClip:[linkerA selectedClip]
				toClip:[linkerB selectedClip]];
        [linkerA updateTables];
        [linkerB updateTables];
		
		[self saveLinks:self];
    }
}

-(IBAction)linkFromRightToLeft:(id)sender
{
    if([linkerA isClipSelected]&&[linkerB isClipSelected]){
        [self linkClip:[linkerB selectedClip]
				toClip:[linkerA selectedClip]];
        [linkerA updateTables];
        [linkerB updateTables];
		[self saveLinks:self];		
    }
}

- (void) linkClip:(CloudsClip*) source toClip:(CloudsClip*) target
{
	if(source->getLinkName() != target->getLinkName()){
		
		NSLog(@"creating link for %s + %s", source->getLinkName().c_str(), target->getLinkName().c_str());
		
		CloudsLink l;
		l.sourceName = source->getLinkName();
		l.targetName = target->getLinkName();
		l.startFrame = -1;
		l.endFrame = -1;
		
		parser.unsuppressConnection(l);
		
		parser.addLink(l);
		
		//save
		[self saveLinks:self];
	}
	else{
		NSLog(@"failed clip for %s + %s", source->getLinkName().c_str(), target->getLinkName().c_str());
	}
}

- (void) suppressLink:(CloudsClip*)source toClip:(CloudsClip*) target
{
	if(source != target){
		
		NSLog(@"suppressing link between %s + %s", source->getLinkName().c_str(), target->getLinkName().c_str());
		
		CloudsLink l;
		l.sourceName = source->getLinkName();
		l.targetName = target->getLinkName();
		l.startFrame = -1;
		l.endFrame = -1;
        
		parser.removeLink(source->getLinkName(),target->getLinkName());
		parser.suppressConnection(l);
        
		[self saveLinks:self];
	}
}

- (IBAction) saveLinks:(id)sender
{
    parser.saveLinks();
    
//	if(ofDirectory("../../../CloudsData/links/").exists()){
//		parser.saveLinks("../../../CloudsData/links/clouds_link_db.xml");
//        cout<<"saving!"<<endl;
//	}
//	else{
//		parser.saveLinks("clouds_link_db.xml");
//	}
}

- (void) playClip:(CloudsClip*) clip
{

	if(currentPlayingClip != NULL && currentPlayingClip->getLinkName() == clip->getLinkName()){
		if(preview.isPlaying()){
			preview.stop();
		}
		else{
			if(preview.getCurrentFrame() >= clipEndFrame){
				preview.setFrame( clip->startFrame );
			}
			preview.play();
		}
		return;
	}
		
	preview.stop();
	
    if(clip->hasMediaAsset){
        
        movieSuccessfullyLoaded = preview.loadMovie(clip->combinedVideoPath);
        if(!movieSuccessfullyLoaded){
            ofLogError() << "Clip " << clip->combinedVideoPath << " failed to load.";
            return;
        }
        clipEndFrame = preview.getTotalNumFrames();
        playingRGBD = true;
    }
    else{
        string clipFilePath = relinkFilePath( clip->sourceVideoFilePath );
        movieSuccessfullyLoaded = preview.loadMovie(clipFilePath) ;
        if(!movieSuccessfullyLoaded){
            ofLogError() << "Clip " << clipFilePath << " failed to load.";
            return;
        }
        
        preview.setFrame(clip->startFrame);
        playingRGBD = false;
        clipEndFrame = clip->endFrame;
    }
    
	preview.play();

	currentPlayingClip = clip;
}

- (IBAction) autolinkByNumber:(id)sender
{
	parser.autolinkSequentialClips();
	[self saveLinks:sender];
}

- (void)keyReleased:(int)key
{
	
}

- (void)mouseMoved:(NSPoint)p
{
	
}

- (void)mouseDragged:(NSPoint)p button:(int)button
{
	
}

- (void)mousePressed:(NSPoint)p button:(int)button
{
	
}

- (void)mouseReleased:(NSPoint)p button:(int)button
{
	
}

- (void)windowResized:(NSSize)size
{

}

@end