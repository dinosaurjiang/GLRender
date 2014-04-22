//
//  GLTransitions.cpp
//  GLRender
//
//  Created by dinosaur on 13-12-6.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#include "GLTransitions.h"
#include <stdlib.h>
#include <limits.h>
#include <cmath>
#include "GLRenderDef.h"
#include "Utilities.h"

float GLTransitions::linear(float ratio)
{
    return ratio;
}

float GLTransitions::randomize(float ratio)
{
    return randomFloat();
}

float GLTransitions::easeIn(float ratio)
{
    return ratio * ratio * ratio;
}

float GLTransitions::easeOut(float ratio)
{
    float invRatio = ratio - 1.0f;
    return invRatio * invRatio * invRatio + 1.0f;
}

float GLTransitions::easeInOut(float ratio)
{
    if (ratio < 0.5f) return 0.5f * GLTransitions::easeIn(ratio*2.0f);
    else              return 0.5f * GLTransitions::easeOut((ratio-0.5f)*2.0f) + 0.5f;
}

float GLTransitions::easeOutIn(float ratio)
{
    if (ratio < 0.5f) return 0.5f * GLTransitions::easeOut(ratio*2.0f);
    else              return 0.5f * GLTransitions::easeIn((ratio-0.5f)*2.0f) + 0.5f;
}

float GLTransitions::easeInBack(float ratio)
{
    float s = 1.70158f;
    return powf(ratio, 2.0f) * ((s + 1.0f)*ratio - s);
}

float GLTransitions::easeOutBack(float ratio)
{
    float invRatio = ratio - 1.0f;
    float s = 1.70158f;
    return powf(invRatio, 2.0f) * ((s + 1.0f)*invRatio + s) + 1.0f;
}

float GLTransitions::easeInOutBack(float ratio)
{
    if (ratio < 0.5f) return 0.5f * GLTransitions::easeInBack(ratio*2.0f);
    else              return 0.5f * GLTransitions::easeOutBack((ratio-0.5f)*2.0f) + 0.5f;
}

float GLTransitions::easeOutInBack(float ratio)
{
    if (ratio < 0.5f) return 0.5f * GLTransitions::easeOutBack(ratio*2.0f);
    else              return 0.5f * GLTransitions::easeInBack((ratio-0.5f)*2.0f) + 0.5f;
}

float GLTransitions::easeInElastic(float ratio)
{
    if (ratio == 0.0f || ratio == 1.0f) return ratio;
    else
    {
        float p = 0.3f;
        float s = p / 4.0f;
        float invRatio = ratio - 1.0f;
        return -1.0f * powf(2.0f, 10.0f*invRatio) * sinf((invRatio-s)*TWO_PI/p);
    }
}

float GLTransitions::easeOutElastic(float ratio)
{
    if (ratio == 0.0f || ratio == 1.0f) return ratio;
    else
    {
        float p = 0.3f;
        float s = p / 4.0f;
        return powf(2.0f, -10.0f*ratio) * sinf((ratio-s)*TWO_PI/p) + 1.0f;
    }
}

float GLTransitions::easeInOutElastic(float ratio)
{
    if (ratio < 0.5f) return 0.5f * GLTransitions::easeInElastic(ratio*2.0f);
    else              return 0.5f * GLTransitions::easeOutElastic((ratio-0.5f)*2.0f) + 0.5f;
}

float GLTransitions::easeOutInElastic(float ratio)
{
    if (ratio < 0.5f) return 0.5f * GLTransitions::easeOutElastic(ratio*2.0f);
    else              return 0.5f * GLTransitions::easeInElastic((ratio-0.5f)*2.0f) + 0.5f;
}

float GLTransitions::easeInBounce(float ratio)
{
    return 1.0f - GLTransitions::easeOutBounce(1.0f - ratio);
}

float GLTransitions::easeOutBounce(float ratio)
{
    float s = 7.5625f;
    float p = 2.75f;
    float l;
    if (ratio < (1.0f/p))
    {
        l = s * powf(ratio, 2.0f);
    }
    else
    {
        if (ratio < (2.0f/p))
        {
            ratio -= 1.5f/p;
            l = s * powf(ratio, 2.0f) + 0.75f;
        }
        else
        {
            if (ratio < 2.5f/p)
            {
                ratio -= 2.25f/p;
                l = s * powf(ratio, 2.0f) + 0.9375f;
            }
            else
            {
                ratio -= 2.625f/p;
                l = s * powf(ratio, 2.0f) + 0.984375f;
            }
        }
    }
    return l;
}

float GLTransitions::easeInOutBounce(float ratio)
{
    if (ratio < 0.5f) return 0.5f * GLTransitions::easeInBounce(ratio*2.0f);
    else              return 0.5f * GLTransitions::easeOutBounce((ratio-0.5f)*2.0f) + 0.5f;
}

float GLTransitions::easeOutInBounce(float ratio)
{
    if (ratio < 0.5f) return 0.5f * GLTransitions::easeOutBounce(ratio*2.0f);
    else              return 0.5f * GLTransitions::easeInBounce((ratio-0.5f)*2.0f) + 0.5f;
}




float timing_function_value_with_name(TransiltionTimingFunction funcType, float ratio)
{
    float t = 0;
    switch (funcType) {
        case kTransiltionTimingFunctionLinear:
            return GLTransitions::linear(ratio);
        case kTransiltionTimingFunctionRandomize:
            return GLTransitions::randomize(ratio);
        case kTransiltionTimingFunctionEaseIn:
            return GLTransitions::easeIn(ratio);
        case kTransiltionTimingFunctionEaseOut:
            return GLTransitions::easeOut(ratio);
        case kTransiltionTimingFunctionEaseInOut:
            return GLTransitions::easeInOut(ratio);
        case kTransiltionTimingFunctionEaseOutIn:
            return GLTransitions::easeOutIn(ratio);
            
        case kTransiltionTimingFunctionEaseInBack:
            return GLTransitions::easeInBack(ratio);
        case kTransiltionTimingFunctionEaseOutBack:
            return GLTransitions::easeOutBack(ratio);
        case kTransiltionTimingFunctionEaseInOutBack:
            return GLTransitions::easeInOutBack(ratio);
        case kTransiltionTimingFunctionEaseOutInBack:
            return GLTransitions::easeOutInBack(ratio);
            
        case kTransiltionTimingFunctionEaseInElastic:
            return GLTransitions::easeInElastic(ratio);
        case kTransiltionTimingFunctionEaseOutElastic:
            return GLTransitions::easeOutElastic(ratio);
        case kTransiltionTimingFunctionEaseInOutElastic:
            return GLTransitions::easeInOutElastic(ratio);
        case kTransiltionTimingFunctionEaseOutInElastic:
            return GLTransitions::easeOutInElastic(ratio);
            
        case kTransiltionTimingFunctionEaseInBounce:
            return GLTransitions::easeInBounce(ratio);
        case kTransiltionTimingFunctionEaseOutBounce:
            return GLTransitions::easeOutBounce(ratio);
        case kTransiltionTimingFunctionEaseInOutBounce:
            return GLTransitions::easeInOutBounce(ratio);
        case kTransiltionTimingFunctionEaseOutInBounce:
            return GLTransitions::easeOutInBounce(ratio);
        default:
            break;
    }
    return t;
}


