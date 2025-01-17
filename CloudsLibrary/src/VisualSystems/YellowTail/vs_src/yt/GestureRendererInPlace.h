/*
 *  GestureRendererInPlace.h
 *  yellowtail
 *
 *  Created by Max Hawkins on 12/15/10.
 *  Copyright 2010 Carnegie Mellon University. All rights reserved.
 *
 */
#ifndef GESTURE_RENDERER_INPLACE
#define GESTURE_RENDERER_INPLACE

#include "GestureRenderer.h"
#include "Gesture.h"
#include "YtPolygon.h"

    
class GestureRendererInPlace : public GestureRenderer {
public:
	static void renderGesture(Gesture *gesture, int maxWidth, int maxHeight);
private:
	static void renderFill(Gesture *gesture, int maxWidth, int maxHeight);
	static void renderOutline(Gesture *gesture, int maxWidth, int maxHeight);
};

#endif