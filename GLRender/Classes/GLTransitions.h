//
//  GLTransitions.h
//  GLRender
//
//  Created by dinosaur on 13-12-6.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#ifndef GLRender_GLTransitions_h
#define GLRender_GLTransitions_h

#include <iostream>

enum TransiltionTimingFunction
{
    kTransiltionTimingFunctionLinear,
    kTransiltionTimingFunctionRandomize,
    
    kTransiltionTimingFunctionEaseIn,
    kTransiltionTimingFunctionEaseOut,
    kTransiltionTimingFunctionEaseInOut,
    kTransiltionTimingFunctionEaseOutIn,
    
    kTransiltionTimingFunctionEaseInBack,
    kTransiltionTimingFunctionEaseOutBack,
    kTransiltionTimingFunctionEaseInOutBack,
    kTransiltionTimingFunctionEaseOutInBack,
    
    kTransiltionTimingFunctionEaseInElastic,
    kTransiltionTimingFunctionEaseOutElastic,
    kTransiltionTimingFunctionEaseInOutElastic,
    kTransiltionTimingFunctionEaseOutInElastic,
    
    kTransiltionTimingFunctionEaseInBounce,
    kTransiltionTimingFunctionEaseOutBounce,
    kTransiltionTimingFunctionEaseInOutBounce,
    kTransiltionTimingFunctionEaseOutInBounce,
    
    kNumberOfTransiltionTimingFunction
};

typedef enum TransiltionTimingFunction TransiltionTimingFunction;


/*
 *  those functions comes from sparrowframe-- a open source 2d game engine.
 */
class GLTransitions
{
public:
    static inline float  linear(float ratio);
    static inline float  randomize(float ratio);
    
    static inline float  easeIn(float ratio);
    static inline float  easeOut(float ratio);
    static inline float  easeInOut(float ratio);
    static inline float  easeOutIn(float ratio);
    
    static inline float  easeInBack(float ratio);
    static inline float  easeOutBack(float ratio);
    static inline float  easeInOutBack(float ratio);
    static inline float  easeOutInBack(float ratio);
    
    static inline float  easeInElastic(float ratio);
    static inline float  easeOutElastic(float ratio);
    static inline float  easeInOutElastic(float ratio);
    static inline float  easeOutInElastic(float ratio);
    
    static inline float  easeInBounce(float ratio);
    static inline float  easeOutBounce(float ratio);
    static inline float  easeInOutBounce(float ratio);
    static inline float  easeOutInBounce(float ratio);
    
private:
    GLTransitions();
    ~GLTransitions();
};


float timing_function_value_with_name(TransiltionTimingFunction funcType, float ratio);



#endif
