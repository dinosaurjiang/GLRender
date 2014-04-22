//
//  GLParticleSystems.h
//  GLRender
//
//  Created by dinosaur on 13-12-11.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#ifndef __GLRender__GLParticleSystems__
#define __GLRender__GLParticleSystems__

#include <iostream>
#include "vec2.h"
#include "GLObject.h"
#include "GLTexture.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct Color4F
    {
        float r,g,b,a;
    }Color4F;
    
    //* @enum
    enum {
        /** The Particle emitter lives forever */
        kParticleDurationInfinity = -1,
        
        /** The starting size of the particle is equal to the ending size */
        kParticleStartSizeEqualToEndSize = -1,
        
        /** The starting radius of the particle is equal to the ending radius */
        kParticleStartRadiusEqualToEndRadius = -1,
    };
    
    
    enum {
        /** Gravity mode (A mode) */
        kParticleModeGravity,
        
        /** Radius mode (B mode) */
        kParticleModeRadius,
    };
    
    
    /** @typedef tPositionType
     possible types of particle positions
     */
    typedef enum {
        /** Living particles are attached to the world and are unaffected by emitter repositioning. */
        kPositionTypeFree,
        
        /** Living particles are attached to the world but will follow the emitter repositioning.
         Use case: Attach an emitter to an sprite, and you want that the emitter follows the sprite.
         */
        kPositionTypeRelative,
        
        /** Living particles are attached to the emitter and are translated along with it. */
        kPositionTypeGrouped,
    }tPositionType;
    
//    // backward compatible
//    enum {
//        kPositionTypeFree = kPositionTypeFree,
//        kPositionTypeGrouped = kPositionTypeGrouped,
//    };

    typedef struct Particle
    {
        kmVec2		pos;
        kmVec2		startPos;
        
        Color4F     color;
        Color4F     deltaColor;
        
        float		size;
        float		rotation;
        
        float		deltaSize;
        float		deltaRotation;
        
        double		timeToLive;
        
        //unsigned int atlasIndex;// for batch node, there not need anymore
        
        union {
            // Mode A: gravity, direction, radial accel, tangential accel
            struct {
                kmVec2		dir;
                float		radialAccel;
                float		tangentialAccel;
            } A;
            
            // Mode B: radius mode
            struct {
                float		angle;
                float		degreesPerSecond;
                float		radius;
                float		deltaRadius;
            } B;
        } mode;
        
    }Particle;
    
#ifdef __cplusplus
}
#endif


using namespace std;
class GLProgram;
class GLTexture;
class GLParticleSystem : public GLObjectBase
{
    
public:
    
    DECLARE_CLASS(GLParticleSystem)

    ~GLParticleSystem();
    GLParticleSystem() = default;
    /*
     * load particle system from json file data.
     */
    virtual void initGLParticleSystem(unsigned int particleCount);
    virtual void initGLParticleSystem(const char * json_data, unsigned long length);
    
    void resetSystem();
    void stopSystem();
    bool isFull();
    
    // those two method should be override by subclass
    virtual void updateQuadWithParticle(Particle* particle ,kmVec2 *pos);
    virtual void postStep();
    
    virtual void visit();
    virtual void draw();
    virtual void usePorgram();
    
    virtual void update(float dt);
    
    // call every time frame to update.
    virtual void updateParticles(float dt);
    virtual void updateWithNoTime();
    
    
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    virtual void setTexture(GLTexture* texture);
    GLTexture * texture();
    void setBlendAdditive(bool additive);
    bool blendAdditive();
    void setBlendFunc(GLBlender & blend);
    void setTotalParticles(unsigned long tp);
    unsigned long sysTotalParticles();
    
    void setTangentialAccel(float t);
    float tangentialAccel();
    
    void setTangentialAccelVar(float t);
    float tangentialAccelVar();
    
    void setRadialAccel(float t);
    float radialAccel();
    
    void setRadialAccelVar(float t);
    float radialAccelVar();
    
    void setGravity(kmVec2 g);
    kmVec2 gravity();
    
    void setSpeed(float speed);
    float speed();
    
    void setSpeedVar(float sv);
    float speedVar();
    
    void setStartRadius(float startRadius);
    float startRadius();
    
    void setStartRadiusVar(float startRadiusVar1);
    float startRadiusVar();
    
    void setEndRadius(float endRadius);
    float endRadius();
    
    void setEndRadiusVar(float endRadiusVar);
    float endRadiusVar();
    
    void setRotatePerSecond(float degrees);
    float rotatePerSecond();
    
    void setRotatePerSecondVar(float degrees);
    float rotatePerSecondVar();
    
    void updateBlendFunc();
    
protected:
    
	// is the particle system active ?
	bool _active;
	// Texture alpha behavior
	bool _opacityModifyRGB;
	// Whether or not the node will be auto-removed when there are not particles
	bool	_autoRemoveOnFinish;
	//YES if scaled or rotated
	bool _transformSystemDirty;
    
    
	// duration in seconds of the system. -1 is infinity
	float _duration;
	// time elapsed since the start of the system (in seconds)
	float _elapsed;
    
	// position is from "superclass" CocosNode
	kmVec2 _sourcePosition;
	// Position variance
	kmVec2 _posVar;
    
	// The angle (direction) of the particles measured in degrees
	float _angle;
	// Angle variance measured in degrees;
	float _angleVar;
    
	// Different modes
	long    _emitterMode;
	union {
		// Mode A:Gravity + Tangential Accel + Radial Accel
		struct {
			// gravity of the particles
			kmVec2 gravity;
            
			// The speed the particles will have.
			float speed;
			// The speed variance
			float speedVar;
            
			// Tangential acceleration
			float tangentialAccel;
			// Tangential acceleration variance
			float tangentialAccelVar;
            
			// Radial acceleration
			float radialAccel;
			// Radial acceleration variance
			float radialAccelVar;
        } A;
        
		// Mode B: circular movement (gravity, radial accel and tangential accel don't are not used in this mode)
		struct {
            
			// The starting radius of the particles
			float startRadius;
			// The starting radius variance of the particles
			float startRadiusVar;
			// The ending radius of the particles
			float endRadius;
			// The ending radius variance of the particles
			float endRadiusVar;
			// Number of degress to rotate a particle around the source pos per second
			float rotatePerSecond;
			// Variance in degrees for rotatePerSecond
			float rotatePerSecondVar;
            
            float _unused[2];
		} B;
	} _mode;
    
	// start ize of the particles
	float _startSize;
	// start Size variance
	float _startSizeVar;
	// End size of the particle
	float _endSize;
	// end size of variance
	float _endSizeVar;
    
	// How many seconds will the particle live
	float _life;
	// Life variance
	float _lifeVar;
    
	// Start color of the particles
	Color4F _startColor;
	// Start color variance
	Color4F _startColorVar;
	// End color of the particles
	Color4F _endColor;
	// End color variance
	Color4F _endColorVar;
    
	// start angle of the particles
	float _startSpin;
	// start angle variance
	float _startSpinVar;
	// End angle of the particle
	float _endSpin;
	// end angle ariance
	float _endSpinVar;
    
	// Array of particles
	Particle *_particleArray;
	// Maximum particles
	unsigned long _totalParticles;
	// Count of active particles
	unsigned long _particleCount;
    // Number of allocated particles
    unsigned long _allocatedParticles;
    
	// How many particles can be emitted per second
	float _emissionRate;
	float _emitCounter;
    
	// movment type: free or grouped
	tPositionType	_positionType;
    
	//  particle idx
	unsigned long _particleIdx;

	// Texture of the particles(have a default)
    GLTexture *_ptexture;
    
	// blend function
	GLBlender	_blendFunc;
    
    
    bool addParticle();
    GLProgram * ParticleSYSProgram();
    virtual void init(unsigned int particleCount);
private:
    void initParticle(Particle * p);
};


////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

class GLParticleSystemQuad  : public GLParticleSystem
{

public:
    ~GLParticleSystemQuad();
    GLParticleSystemQuad():GLParticleSystem(){};
    
    //load from boundles
    virtual void initGLParticleSystemWithBundleRes(const char * dataFile);
    virtual void initGLParticleSystemWithBundleRes(const string & dataFile);
    
    
    virtual void initGLParticleSystem(unsigned int particleCount);
    virtual void initGLParticleSystem(const char * json_data, unsigned long length);
    
    virtual void updateQuadWithParticle(Particle* particle ,kmVec2 *pos);
    virtual void postStep();
    
    virtual void visit();
    virtual void draw();
    virtual void usePorgram();
    
    void setTexture(GLTexture* texture);
protected:
    void createVAO();
    virtual void init(unsigned int particleCount);
    Quad *_particleList;
    GLushort  *_particleIdxCache;
    
	GLuint				_VAOname;
	GLuint				_buffersVBO[2]; //0: vertex  1: indices
};






#endif /* defined(__GLRender__GLParticleSystems__) */
