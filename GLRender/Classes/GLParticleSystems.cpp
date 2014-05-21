//
//  GLParticleSystems.cpp
//  GLRender
//
//  Created by dinosaur on 13-12-11.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#include "GLParticleSystems.h"
#include "string.h"
#include "Utilities.h"
#include "GLRenderDef.h"
#include "GLTexture.h"
#include "GLProgram.h"
#include "GLSupport.h"
#include "reader.h"
//#include "DefaultShader.h"



static void GLBindVAO(GLuint vaoId)
{
#if OPEN_GL_CACHE
    static GLuint _cacheVAO = 0;
	if( _cacheVAO != vaoId )
	{
		_cacheVAO = vaoId;
		glBindVertexArray(vaoId);
	}
#else
	glBindVertexArrayAPPLE(vaoId);
#endif
}


#if TARGET_IOS
#define MAX_PARTICLES_COUTS 300
#else
#define MAX_PARTICLES_COUTS 1000
#endif

//TODO: need to call supper destructor????
// GLObjectBase::~GLObjectBase();
// I think not
GLParticleSystem::~GLParticleSystem()
{
    if (_particleArray)
    {
        delete [] _particleArray;
    }
    LOG("GLParticleSystem::~GLParticleSystem");
}


void GLParticleSystem::initGLParticleSystem(unsigned int numberOfParticles)
{
    this->init(numberOfParticles);
}


/*
 * load particle system from json file data.
 */
void GLParticleSystem::initGLParticleSystem(const char * json_data, unsigned long length)
{
    //read the json data
    string * json_doc_string = new string(json_data);
    Json::Reader * json_reader = new Json::Reader();
    Json::Value rootValue;
    json_reader->parse(*json_doc_string, rootValue);
    
    unsigned int myMaxParticles = rootValue["maxParticles"].asUInt();
    this->init(myMaxParticles);
    

    // angle
    // duration
    _angle = rootValue["angle"].asDouble();
    _angleVar = rootValue["angleVariance"].asDouble();
    
    _duration = rootValue["duration"].asDouble();
    
    // blend function
    _blendFunc.src = rootValue["blendFuncSource"].asInt();
    _blendFunc.dst = rootValue["blendFuncDestination"].asInt();
    
    // color
    float r,g,b,a;
    
    r = rootValue["startColorRed"].asDouble();
    g = rootValue["startColorGreen"].asDouble();
    b = rootValue["startColorBlue"].asDouble();
    a = rootValue["startColorAlpha"].asDouble();
    _startColor = (Color4F) {r,g,b,a};
    
    r = rootValue["startColorVarianceRed"].asDouble();
    g = rootValue["startColorVarianceGreen"].asDouble();
    b = rootValue["startColorVarianceBlue"].asDouble();
    a = rootValue["startColorVarianceAlpha"].asDouble();
    _startColorVar = (Color4F) {r,g,b,a};
    
    r = rootValue["finishColorRed"].asDouble();
    g = rootValue["finishColorGreen"].asDouble();
    b = rootValue["finishColorBlue"].asDouble();
    a = rootValue["finishColorAlpha"].asDouble();
    _endColor = (Color4F) {r,g,b,a};
    
    r = rootValue["finishColorVarianceRed"].asDouble();
    g = rootValue["finishColorVarianceGreen"].asDouble();
    b = rootValue["finishColorVarianceBlue"].asDouble();
    a = rootValue["finishColorVarianceAlpha"].asDouble();
    _endColorVar = (Color4F) {r,g,b,a};
    
    // particle size
    _startSize = rootValue["startParticleSize"].asDouble();
    _startSizeVar = rootValue["startParticleSizeVariance"].asDouble();
    _endSize = rootValue["finishParticleSize"].asDouble();
    _endSizeVar = rootValue["finishParticleSizeVariance"].asDouble();
    
    // position
    float x = rootValue["sourcePositionx"].asDouble();
    float y = rootValue["sourcePositiony"].asDouble();
    _position = {x,y,0};
    _posVar.x = rootValue["sourcePositionVariancex"].asDouble();
    _posVar.y = rootValue["sourcePositionVariancey"].asDouble();
    
    // Spinning
    _startSpin = rootValue["rotationStart"].asDouble();
    _startSpinVar = rootValue["rotationStartVariance"].asDouble();
    _endSpin = rootValue["rotationEnd"].asDouble();
    _endSpinVar = rootValue["rotationEndVariance"].asDouble();
    
    _emitterMode  = rootValue["emitterType"].asInt();
    
    // Mode A: Gravity + tangential accel + radial accel
    if( _emitterMode  == kParticleModeGravity ) {
        // gravity
        _mode.A.gravity.x = rootValue["gravityx"].asDouble();
        _mode.A.gravity.y = rootValue["gravityy"].asDouble();
        
        //
        // speed
        _mode.A.speed = rootValue["speed"].asDouble();
        _mode.A.speedVar = rootValue["speedVariance"].asDouble();
        
        // radial acceleration
        Json::Value v_zero(0);
        Json::Value dvalue(0);
        dvalue = rootValue.get("radialAcceleration", v_zero);
        _mode.A.radialAccel = dvalue.asDouble();
        
        dvalue = rootValue.get("radialAccelVariance",v_zero);
        _mode.A.radialAccelVar = dvalue.asDouble();
        
        // tangential acceleration
        dvalue = rootValue.get("tangentialAcceleration",v_zero);
        _mode.A.tangentialAccel = dvalue.asDouble();
        
        dvalue = rootValue.get("tangentialAccelVariance",v_zero);
        _mode.A.tangentialAccelVar = dvalue.asDouble();
    }
    // or Mode B: radius movement
    else if( _emitterMode  == kParticleModeRadius ) {
        float maxRadius = rootValue["maxRadius"].asDouble();
        float maxRadiusVar = rootValue["maxRadiusVariance"].asDouble();
        float minRadius = rootValue["minRadius"].asDouble();
        
        _mode.B.startRadius = maxRadius;
        _mode.B.startRadiusVar = maxRadiusVar;
        _mode.B.endRadius = minRadius;
        _mode.B.endRadiusVar = 0;
        _mode.B.rotatePerSecond = rootValue["rotatePerSecond"].asDouble();
        _mode.B.rotatePerSecondVar = rootValue["rotatePerSecondVariance"].asDouble();
        
    } else {
        GLAssert( false, "Invalid emitterType in config file");
    }
    
    //life span
    _life = rootValue["particleLifespan"].asDouble();
    _lifeVar = rootValue["particleLifespanVariance"].asDouble();
    
    // emission Rate
    _emissionRate = _totalParticles / _life;
    
    //don't get the internal texture if a batchNode is used
//    if (!_batchNode)
    {
        // Set a compatible default for the alpha transfer
        _opacityModifyRGB = false;
        
        // texture
        // Try to get the texture from the cache
        string textureName = rootValue["textureFileName"].asString();
        GLTexture * tex = GLTextureManager::manager()->getTextureWithName(textureName);

        if(tex == nullptr)
        {
            tex = GLTexture::createTextureWithImageName(textureName);
        }
        
        if( tex )
        {
            this->setTexture(tex);
        }
        else
        {
            /////////////////////////////////////////////////////////////////////
            Exception("BAD ARGC", "Why running here, this is not need.");
            // this is cocos2d-way:
            // cocos2d load texture data(base64 encode)
            // 1,decode,
            // 2 flat zip data
            // 3, create texture.
            /////////////////////////////////////////////////////////////////////
            // but I donot want to implments this methods.
            // because decode waste of CPU and time.
            // In my indived file formats.
            // image was store as data.
            // and when it loaded. image will be first load as gltexture.
            // just find it from texture manager is OK.
            /////////////////////////////////////////////////////////////////////
        }
    }
	
    
    //delete the resource
    delete json_doc_string;
    delete json_reader;
}

void GLParticleSystem::init(unsigned int numberOfParticles)
{
    _totalParticles = numberOfParticles;
    _particleCount = 0;
    //particles = (Particle *)calloc( _totalParticles, sizeof(Particle) );
    _particleArray = new Particle[_totalParticles];
    bzero(_particleArray, sizeof(Particle) * _totalParticles);
    
    if( ! _particleArray ) {
        LOG("Particle system: not enough memory");
        Exception("memory alloc faild", "There are no enough memory to use.");
    }
    
    _allocatedParticles = numberOfParticles;
    
    // default, active
    _active = true;
    
    //    // default blend function
    //    blend = (ccBlendFunc) { CC_BLEND_SRC, CC_BLEND_DST };
    
    // default movement type;
    _positionType = kPositionTypeFree;
    
    // by default be in mode A:
    _emitterMode = kParticleModeGravity;
    
    _autoRemoveOnFinish = false;
    
    // Optimization: compile updateParticle method
    //updateParticleCallBack = &GLParticleSystem::updateQuadWithParticle;
    
    //for batchNode
    _transformSystemDirty = false;
}


void GLParticleSystem::initParticle(Particle *par)
{
	//CGPoint currentPosition = _position;
	// timeToLive
	// no negative _life. prevent division by 0
	par->timeToLive = _life + _lifeVar * randomFloat_Minus1_1();
	par->timeToLive = MAX(0, par->timeToLive);
    
	// position
	par->pos.x = _sourcePosition.x + _posVar.x * randomFloat_Minus1_1();
	par->pos.y = _sourcePosition.y + _posVar.y * randomFloat_Minus1_1();
    
	// Color
	Color4F start;
	start.r = clampf( _startColor.r + _startColorVar.r * randomFloat_Minus1_1(), 0, 1);
	start.g = clampf( _startColor.g + _startColorVar.g * randomFloat_Minus1_1(), 0, 1);
	start.b = clampf( _startColor.b + _startColorVar.b * randomFloat_Minus1_1(), 0, 1);
	start.a = clampf( _startColor.a + _startColorVar.a * randomFloat_Minus1_1(), 0, 1);
    
	Color4F end;
	end.r = clampf( _endColor.r + _endColorVar.r * randomFloat_Minus1_1(), 0, 1);
	end.g = clampf( _endColor.g + _endColorVar.g * randomFloat_Minus1_1(), 0, 1);
	end.b = clampf( _endColor.b + _endColorVar.b * randomFloat_Minus1_1(), 0, 1);
	end.a = clampf( _endColor.a + _endColorVar.a * randomFloat_Minus1_1(), 0, 1);
    
	par->color = start;
	par->deltaColor.r = (end.r - start.r) / par->timeToLive;
	par->deltaColor.g = (end.g - start.g) / par->timeToLive;
	par->deltaColor.b = (end.b - start.b) / par->timeToLive;
	par->deltaColor.a = (end.a - start.a) / par->timeToLive;
    
	// size
	float startS = _startSize + _startSizeVar * randomFloat_Minus1_1();
	startS = MAX(.0f, startS); // No negative value
    
	par->size = startS;
	if( _endSize == kParticleStartSizeEqualToEndSize )
    {
		par->deltaSize = .0f;
	}
    else
    {
		float endS = _endSize + _endSizeVar * randomFloat_Minus1_1();
		endS = MAX(.0f, endS);	// No negative values
		par->deltaSize = (endS - startS) / par->timeToLive;
	}
    
	// rotation
	float startA = _startSpin + _startSpinVar * randomFloat_Minus1_1();
	float endA = _endSpin + _endSpinVar * randomFloat_Minus1_1();
	par->rotation = startA;
	par->deltaRotation = (endA - startA) / par->timeToLive;
    
    
	if( _positionType == kPositionTypeRelative )
    {
        par->startPos.x = _position.x;
        par->startPos.y = _position.y;
    }
    
	// direction
	float a = angle_to_radius( _angle + _angleVar * randomFloat_Minus1_1() );
    
	// Mode Gravity: A
	if( _emitterMode == kParticleModeGravity ) {
        
		kmVec2 v = {cosf( a ), sinf( a )};
		float s = _mode.A.speed + _mode.A.speedVar * randomFloat_Minus1_1();
        
		// direction
		par->mode.A.dir = {v.x*s, v.y*s};
        
		// radial accel
		par->mode.A.radialAccel = _mode.A.radialAccel + _mode.A.radialAccelVar * randomFloat_Minus1_1();
        
		// tangential accel
		par->mode.A.tangentialAccel =_mode.A.tangentialAccel + _mode.A.tangentialAccelVar*randomFloat_Minus1_1();
	}
	// Mode Radius: B
	else
    {
		// Set the default diameter of the particle from the source position
		float startRadius = _mode.B.startRadius + _mode.B.startRadiusVar * randomFloat_Minus1_1();
		float endRadius = _mode.B.endRadius + _mode.B.endRadiusVar * randomFloat_Minus1_1();
        
		par->mode.B.radius = startRadius;
        
		if( _mode.B.endRadius == kParticleStartRadiusEqualToEndRadius )
			par->mode.B.deltaRadius = 0;
		else
			par->mode.B.deltaRadius = (endRadius - startRadius) / par->timeToLive;
        
		par->mode.B.angle = a;
		par->mode.B.degreesPerSecond = angle_to_radius(_mode.B.rotatePerSecond + _mode.B.rotatePerSecondVar * randomFloat_Minus1_1());
	}
}


bool GLParticleSystem::addParticle()
{
    if (this->isFull())
    {
        return false;
    }
    
    Particle * p = &_particleArray[_particleCount];
    this->initParticle(p);
    _particleCount++;
    
    return true;
}


void GLParticleSystem::resetSystem()
{
	_active = true;
	_elapsed = 0;
	for(_particleIdx = 0; _particleIdx < _particleCount; ++_particleIdx) {
		Particle *p = &_particleArray[_particleIdx];
		p->timeToLive = 0;
	}
}

void GLParticleSystem::stopSystem()
{
    _active = false;
    _elapsed = _duration;
    _emitCounter = 0;
}

bool GLParticleSystem::isFull()
{
	return (_particleCount == _totalParticles);
}


#pragma mark ParticleSystem - MainLoop

//TODO: does this method auto call draw.
void GLParticleSystem::update(float dt)
{
    this->updateParticles(dt);
}

void GLParticleSystem::updateParticles(float dt)
{
    
	if( _active && _emissionRate )
    {
		float rate = 1.0f /  _emissionRate;
		
		//issue #1201, prevent bursts of particles, due to too high emitCounter
		if (_particleCount < _totalParticles)
			_emitCounter += dt;
		
		while( _particleCount < _totalParticles && _emitCounter > rate )
        {
			this->addParticle();
			_emitCounter -= rate;
		}
        
		_elapsed += dt;
        
		if(_duration != -1 && _duration < _elapsed)
            this->stopSystem();
	}
    
	_particleIdx = 0;
    
	kmVec2 currentPosition = {.0f};
    if( _positionType == kPositionTypeRelative )   
    {
        currentPosition.x = _position.x;
        currentPosition.y = _position.y;
    }
    
    
	if (!_hidden)
	{
		while( _particleIdx < _particleCount )
		{
			Particle *p = &_particleArray[_particleIdx];
            
			// _life
			p->timeToLive -= dt;
            
			if( p->timeToLive > 0 ) {
                
				// Mode A: gravity, direction, tangential accel & radial accel
				if( _emitterMode == kParticleModeGravity ) {
					kmVec2 tmp, radial={0.0f}, tangential;
                    
					// radial acceleration
					if(p->pos.x || p->pos.y)
                        kmVec2Normalize(&radial, &p->pos);
                    
					tangential = radial;
					kmVec2Mult(&radial,&radial, p->mode.A.radialAccel);
                    
					// tangential acceleration
					float newy = tangential.x;
					tangential.x = -tangential.y;
					tangential.y = newy;
					kmVec2Mult(&tangential,&tangential, p->mode.A.tangentialAccel);
                    
					// (gravity + radial + tangential) * dt
                    tmp.x = radial.x + tangential.x + _mode.A.gravity.x;
                    tmp.y = radial.y + tangential.y + _mode.A.gravity.y;
					//tmp = ccpAdd( ccpAdd( radial, tangential), _mode.A.gravity);
                    kmVec2Mult(&tmp, &tmp, dt);
					//tmp = ccpMult( tmp, dt);
                    kmVec2Add(&p->mode.A.dir, &p->mode.A.dir, &tmp);
					//p->mode.A.dir = ccpAdd( p->mode.A.dir, tmp);
                    
                    kmVec2Mult(&tmp, &p->mode.A.dir, dt);
					//tmp = ccpMult(p->mode.A.dir, dt);
					kmVec2Add(&p->pos, &p->pos, &tmp);
                    //p->pos = ccpAdd( p->pos, tmp );
				}
				// Mode B: radius movement
				else
                {
					// Update the angle and radius of the particle.
					p->mode.B.angle += p->mode.B.degreesPerSecond * dt;
					p->mode.B.radius += p->mode.B.deltaRadius * dt;
                    
					p->pos.x = - cosf(p->mode.B.angle) * p->mode.B.radius;
					p->pos.y = - sinf(p->mode.B.angle) * p->mode.B.radius;
				}
                
				// color
				p->color.r += (p->deltaColor.r * dt);
				p->color.g += (p->deltaColor.g * dt);
				p->color.b += (p->deltaColor.b * dt);
				p->color.a += (p->deltaColor.a * dt);
                
				// size
				p->size += (p->deltaSize * dt);
				p->size = MAX( 0, p->size );
                
				// angle
				p->rotation += (p->deltaRotation * dt);
                
				//
				// update values in quad
				//
                
				kmVec2	newPos;
                
				if( _positionType == kPositionTypeFree || _positionType == kPositionTypeRelative )
				{
                    kmVec2 diff; kmVec2Subtract(&diff, &currentPosition, &p->startPos);
					//kmVec2 diff = ccpSub( currentPosition, p->startPos );
					kmVec2Subtract(&newPos, &p->pos, &diff);
                    //newPos = ccpSub(p->pos, diff);
				} else
					newPos = p->pos;
                
				// translate newPos to correct position, since matrix transform isn't performed in batchnode
				// don't update the particle with the new position information, it will interfere with the radius and tangential calculations
                //if (batchNode)
                if(0)// no batch node
				{
					newPos.x+=_position.x;
					newPos.y+=_position.y;
				}
                
                
                this->updateQuadWithParticle(p, &newPos);
				//_updateParticleImp(self, _updateParticleSel, p, newPos);
                
				// update particle counter
				_particleIdx++;
                
			} else {
				// _life < 0
                
                // there do not use batch node, so do not use atlasIndex.
				//unsigned long currentIndex = p->atlasIndex;
                
                
                // 把最后一个离子，move 到当前位置。
				if( _particleIdx != _particleCount-1 )
					_particleArray[_particleIdx] = _particleArray[_particleCount-1];
                
				//if (batchNode)
                /*if(0)
				{
					//disable the switched particle
					[batchNode disableParticle(atlasIndex+currentIndex)];
					//switch indexes
					particleArray[_particleCount-1].atlasIndex = currentIndex;
				}*/
                
				_particleCount--;
                
				if( _particleCount == 0 && _autoRemoveOnFinish )
                {
                    _parent->removeChildByTag(this->_tag, true);
					return;
				}
			}
		}//while
		_transformSystemDirty = false;
	}
    
    this->postStep();
}

void GLParticleSystem::updateWithNoTime()
{
    this->updateParticles(0.0f);
}


// those two method should be override by subclass
// why ???
void GLParticleSystem::updateQuadWithParticle(Particle* particle ,kmVec2 *pos)
{
    Exception("abostructe function", "overrite me");
}


void GLParticleSystem::postStep()
{
    Exception("abostructe function", "overrite me");
}

void GLParticleSystem::visit(){}
void GLParticleSystem::draw(){}
void GLParticleSystem::usePorgram(){}
#pragma mark -
#pragma mark preperty modify



void GLParticleSystem::setTexture(GLTexture* texture)
{
    _ptexture = texture;
    this->updateBlendFunc();
}

GLTexture * GLParticleSystem::texture()
{
	return _ptexture;
}

#pragma mark ParticleSystem - Additive Blending
void GLParticleSystem::setBlendAdditive(bool additive)
{
	if( additive )
    {
        
		_blendFunc.src = GL_SRC_ALPHA;
		_blendFunc.dst = GL_ONE;
        
	} else {
        
        // ptexture && /*! [_texture hasPremultipliedAlpha] */
		if( 0 ) {// all the texture has premultip alpha
			_blendFunc.src = GL_SRC_ALPHA;
			_blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
		} else {
            _blendFunc.makeDefault();
		}
	}
}

bool GLParticleSystem::blendAdditive()
{
	return( _blendFunc.src == GL_SRC_ALPHA && _blendFunc.dst == GL_ONE);
}

void GLParticleSystem::setBlendFunc(GLBlender & blend)
{
	if( _blendFunc.src != _blendFunc.src || _blendFunc.dst != _blendFunc.dst ) {
		blend = blend;
        this->updateBlendFunc();
	}
}
#pragma mark ParticleSystem - Total Particles Property

void GLParticleSystem::setTotalParticles(unsigned long tp)
{
    GLAssert( tp <= _allocatedParticles, "Particle: resizing particle array only supported for quads");
    _totalParticles = tp;
}

unsigned long GLParticleSystem::sysTotalParticles()
{
    return _totalParticles;
}

#pragma mark ParticleSystem - Properties of Gravity Mode
void GLParticleSystem::setTangentialAccel(float t)
{
	GLAssert( _emitterMode == kParticleModeGravity, "Particle Mode should be Gravity");
	_mode.A.tangentialAccel = t;
}
float GLParticleSystem::tangentialAccel()
{
	GLAssert( _emitterMode == kParticleModeGravity, "Particle Mode should be Gravity");
	return _mode.A.tangentialAccel;
}

void GLParticleSystem::setTangentialAccelVar(float t)
{
	GLAssert( _emitterMode == kParticleModeGravity, "Particle Mode should be Gravity");
	_mode.A.tangentialAccelVar = t;
}
float GLParticleSystem::tangentialAccelVar()
{
	GLAssert( _emitterMode == kParticleModeGravity, "Particle Mode should be Gravity");
	return _mode.A.tangentialAccelVar;
}

void GLParticleSystem::setRadialAccel(float t)
{
	GLAssert( _emitterMode == kParticleModeGravity, "Particle Mode should be Gravity");
	_mode.A.radialAccel = t;
}
float GLParticleSystem::radialAccel()
{
	GLAssert( _emitterMode == kParticleModeGravity, "Particle Mode should be Gravity");
	return _mode.A.radialAccel;
}

void GLParticleSystem::setRadialAccelVar(float t)
{
	GLAssert( _emitterMode == kParticleModeGravity, "Particle Mode should be Gravity");
	_mode.A.radialAccelVar = t;
}
float GLParticleSystem::radialAccelVar()
{
	GLAssert( _emitterMode == kParticleModeGravity, "Particle Mode should be Gravity");
	return _mode.A.radialAccelVar;
}

void GLParticleSystem::setGravity(kmVec2 g)
{
	GLAssert( _emitterMode == kParticleModeGravity, "Particle Mode should be Gravity");
	_mode.A.gravity = g;
}
kmVec2 GLParticleSystem::gravity()
{
	GLAssert( _emitterMode == kParticleModeGravity, "Particle Mode should be Gravity");
	return _mode.A.gravity;
}

void GLParticleSystem::setSpeed(float speed)
{
	GLAssert( _emitterMode == kParticleModeGravity, "Particle Mode should be Gravity");
	_mode.A.speed = speed;
}
float GLParticleSystem::speed()
{
	GLAssert( _emitterMode == kParticleModeGravity, "Particle Mode should be Gravity");
	return _mode.A.speed;
}

void GLParticleSystem::setSpeedVar(float sv)
{
	GLAssert( _emitterMode == kParticleModeGravity, "Particle Mode should be Gravity");
	_mode.A.speedVar = sv;
}
float GLParticleSystem::speedVar()
{
	GLAssert( _emitterMode == kParticleModeGravity, "Particle Mode should be Gravity");
	return _mode.A.speedVar;
}

#pragma mark ParticleSystem - Properties of Radius Mode

void GLParticleSystem::setStartRadius(float startRadius)
{
	GLAssert( _emitterMode == kParticleModeRadius, "Particle Mode should be Radius");
	_mode.B.startRadius = startRadius;
}
float GLParticleSystem::startRadius()
{
	GLAssert( _emitterMode == kParticleModeRadius, "Particle Mode should be Radius");
	return _mode.B.startRadius;
}

void GLParticleSystem::setStartRadiusVar(float startRadiusVar1)
{
	GLAssert( _emitterMode == kParticleModeRadius, "Particle Mode should be Radius");
	_mode.B.startRadiusVar = startRadiusVar1;
}
float GLParticleSystem::startRadiusVar()
{
	GLAssert( _emitterMode == kParticleModeRadius, "Particle Mode should be Radius");
	return _mode.B.startRadiusVar;
}

void GLParticleSystem::setEndRadius(float endRadius)
{
	GLAssert( _emitterMode == kParticleModeRadius, "Particle Mode should be Radius");
	_mode.B.endRadius = endRadius;
}
float GLParticleSystem::endRadius()
{
	GLAssert( _emitterMode == kParticleModeRadius, "Particle Mode should be Radius");
	return _mode.B.endRadius;
}

void GLParticleSystem::setEndRadiusVar(float endRadiusVar)
{
	GLAssert( _emitterMode == kParticleModeRadius, "Particle Mode should be Radius");
	_mode.B.endRadiusVar = endRadiusVar;
}
float GLParticleSystem::endRadiusVar()
{
	GLAssert( _emitterMode == kParticleModeRadius, "Particle Mode should be Radius");
	return _mode.B.endRadiusVar;
}

void GLParticleSystem::setRotatePerSecond(float degrees)
{
	GLAssert( _emitterMode == kParticleModeRadius, "Particle Mode should be Radius");
	_mode.B.rotatePerSecond = degrees;
}
float GLParticleSystem::rotatePerSecond()
{
	GLAssert( _emitterMode == kParticleModeRadius, "Particle Mode should be Radius");
	return _mode.B.rotatePerSecond;
}

void GLParticleSystem::setRotatePerSecondVar(float degrees)
{
	GLAssert( _emitterMode == kParticleModeRadius, "Particle Mode should be Radius");
	_mode.B.rotatePerSecondVar = degrees;
}
float GLParticleSystem::rotatePerSecondVar()
{
	GLAssert( _emitterMode == kParticleModeRadius, "Particle Mode should be Radius");
	return _mode.B.rotatePerSecondVar;
}

#pragma mark Particle - Helpers

void GLParticleSystem::updateBlendFunc()
{
    // all the texture has premuliplied alpha
	bool premultiplied = true;// [_texture hasPremultipliedAlpha];
    
	_opacityModifyRGB = false;
    
	if( _ptexture && _blendFunc.isDefualt() )
    {
		if( premultiplied )
        {
			_opacityModifyRGB = true;
        }
        else
        {
            _blendFunc.src = true;
            _blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
        }
	}
}



#pragma mark -
#pragma mark particle system quad


void GLParticleSystemQuad::usePorgram()
{
    UsingProgram(GLParticleSysProgram::getInstance()->programID());

    GLParticleSysProgram::getInstance()->setupObjectMatrix(this->_transformMatrix.mat);
    GLParticleSysProgram::getInstance()->setupProjectionMatrix(GLSupport::projectionMatrix->mat);
    GLParticleSysProgram::getInstance()->setupModelViewMatrix(GLSupport::modelViewMatrix->mat);
}

GLParticleSystemQuad::~GLParticleSystemQuad()
{
    // release memory
    if (_particleList)
    {
        delete [] _particleList;
        _particleList = NULL;
    }
    
    if(_particleIdxCache)
    {
        delete [] _particleIdxCache;
        _particleIdxCache = NULL;
    }
    
    glDeleteBuffers(2, &_buffersVBO[0]);
    glDeleteVertexArrays(1, &_VAOname);
    
    LOG("GLParticleSystemQuad::~GLParticleSystemQuad");
//super dealloc
//    GLParticleSystem::~GLParticleSystem();
}


void GLParticleSystemQuad::init(unsigned int particleCount)
{
    // super init
    GLParticleSystem::init(particleCount);
    
    // self init
    _particleList = new Quad[particleCount];

    
    _particleIdxCache = new GLushort[particleCount*6];
	for( unsigned long i = 0; i < particleCount; i++)
    {
		const unsigned long i6 = i*6;
		const unsigned long i4 = i*4;
		_particleIdxCache[i6+0] = (GLushort) i4+0;
		_particleIdxCache[i6+1] = (GLushort) i4+1;
		_particleIdxCache[i6+2] = (GLushort) i4+2;
        
		_particleIdxCache[i6+5] = (GLushort) i4+1;
		_particleIdxCache[i6+4] = (GLushort) i4+2;
		_particleIdxCache[i6+3] = (GLushort) i4+3;
	}
    
    this->createVAO();
}

void GLParticleSystemQuad::createVAO()
{
    
    glDeleteBuffers(2, &_buffersVBO[0]);
    glDeleteVertexArrays(1, &_VAOname);
    
	// VAO requires GL_APPLE_vertex_array_object in order to be created on a different thread
	// https://devforums.apple.com/thread/145566?tstart=0
    
    glGenVertexArrays(1, &_VAOname);
    GLBindVAO(_VAOname);
    
#define kQuadSize sizeof(PerPointInfo)
    
    glGenBuffers(2, &_buffersVBO[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Quad) * _totalParticles, _particleList, GL_DYNAMIC_DRAW);
    
    // vertices
    glEnableVertexAttribArray(GLProgram::ATTRIB_VERTEX);
    glVertexAttribPointer(GLProgram::ATTRIB_VERTEX, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( PerPointInfo, vertices));
    
    // colors
    glEnableVertexAttribArray(GLProgram::ATTRIB_COLOR);
    glVertexAttribPointer(GLProgram::ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof( PerPointInfo, color));
    
    // tex coords
    glEnableVertexAttribArray(GLProgram::ATTRIB_TEXCOORD);
    glVertexAttribPointer(GLProgram::ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( PerPointInfo, texCoords));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_particleIdxCache[0]) * _totalParticles * 6, _particleIdxCache, GL_STATIC_DRAW);
    
    // Must unbind the VAO before changing the element buffer.
    GLBindVAO(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    

    CHECK_GL_ERROR();
}


void GLParticleSystemQuad::visit()
{
    
    this->transform();
    this->usePorgram();
    _blendFunc.blend();
    
    if(_ptexture) _ptexture->bind();
    
    
	GLAssert( _particleIdx == _particleCount, "Abnormal error in particle quad");
    
#if 1
    
    GLBindVAO(_VAOname);
    glDrawElements(GL_TRIANGLES, (GLsizei) _particleIdx*6, GL_UNSIGNED_SHORT, 0);
    CHECK_GL_ERROR();
    
#else
    
    int strike = sizeof(PerPointInfo);
	char * startAddr = (char *)_particleList;
    
    
    glEnableVertexAttribArray( GLProgram::ATTRIB_VERTEX );
    glVertexAttribPointer(GLProgram::ATTRIB_VERTEX, 2,GL_FLOAT, GL_FALSE,strike, (void*)startAddr);
    
    glEnableVertexAttribArray( GLProgram::ATTRIB_TEXCOORD );
    glVertexAttribPointer(GLProgram::ATTRIB_TEXCOORD, 2,GL_FLOAT, GL_FALSE,strike, (void*)(startAddr + sizeof(float) * 2));
    
    glEnableVertexAttribArray( GLProgram::ATTRIB_COLOR );
    glVertexAttribPointer(GLProgram::ATTRIB_COLOR, 4,GL_UNSIGNED_BYTE, GL_TRUE, strike, (void*)(startAddr + sizeof(float) * 4));
    glDrawElements(GL_TRIANGLES, (GLsizei) _particleIdx*6, GL_UNSIGNED_SHORT, _particleIdxCache);
    
#endif
}

void GLParticleSystemQuad::draw()
{
    //alpha value not used in particle system
    if( /*_alpha < 0.0001 ||*/ _hidden ) return;
    
    this->visit();
    
    // particale system can not have child.
    // it only can be add as child.
}


void GLParticleSystemQuad::setTexture(GLTexture* texture)
{
    GLParticleSystem::setTexture(texture);
    
    if(_ptexture == nullptr) return;
    
    int tw = _ptexture->maxT();
    int th = _ptexture->maxS();
    kmVec4 frame = _ptexture->frame();
    
    float min_x = frame.x / tw;// min x
    float min_y = frame.y / th;// min y
        
    float max_x = (frame.x + frame.z) / tw;// max x
    float max_y = (frame.y + frame.w) / th;// max y
    
    
    for (int i = 0; i < _totalParticles; i++)
    {
        Quad * tmp = &_particleList[ i ];
        
        // left-down
        tmp->ld.texCoords[0] = min_x;
        tmp->ld.texCoords[1] = min_y;
        
        // left-up
        tmp->lu.texCoords[0] = min_x;
        tmp->lu.texCoords[1] = max_y;
        
        
        // right-up
        tmp->ru.texCoords[0] = max_x;
        tmp->ru.texCoords[1] = max_y;
        
        // right-down
        tmp->rd.texCoords[0] = max_x;
        tmp->rd.texCoords[1] = min_y;
    }
}

void GLParticleSystemQuad::updateQuadWithParticle(Particle* p ,kmVec2 *newPos)
{
    Quad * quad = &_particleList[ _particleIdx ];
    
    // update color
    
	Color4B color;
    if(_opacityModifyRGB && 0)
    {
        color = {
            (unsigned char)(p->color.r*p->color.a*255),
            (unsigned char)(p->color.g*p->color.a*255),
            (unsigned char)(p->color.b*p->color.a*255),
            (unsigned char)(p->color.a*255)
        };
    }
    else
    {
        color = {
            (unsigned char)(p->color.r*255),
            (unsigned char)(p->color.g*255),
            (unsigned char)(p->color.b*255),
            (unsigned char)(p->color.a*255)
        };
    }
    
	quad->ld.color = color;
	quad->lu.color = color;
	quad->rd.color = color;
	quad->ru.color = color;
    
    // update coordinate
    
    
	// vertices
	GLfloat size_2 = p->size/2;
	if( p->rotation ) {
		GLfloat x1 = -size_2;
		GLfloat y1 = -size_2;
        
		GLfloat x2 = size_2;
		GLfloat y2 = size_2;
		GLfloat x = newPos->x;
		GLfloat y = newPos->y;
        
		GLfloat r = (GLfloat)-angle_to_radius(p->rotation);
		GLfloat cr = cosf(r);
		GLfloat sr = sinf(r);
		GLfloat ax = x1 * cr - y1 * sr + x;
		GLfloat ay = x1 * sr + y1 * cr + y;
		GLfloat bx = x2 * cr - y1 * sr + x;
		GLfloat by = x2 * sr + y1 * cr + y;
		GLfloat cx = x2 * cr - y2 * sr + x;
		GLfloat cy = x2 * sr + y2 * cr + y;
		GLfloat dx = x1 * cr - y2 * sr + x;
		GLfloat dy = x1 * sr + y2 * cr + y;
        
		// bottom-left
		quad->ld.vertices[0] = ax;
		quad->ld.vertices[1] = ay;
        
		// bottom-right vertex:
		quad->rd.vertices[0] = bx;
		quad->rd.vertices[1] = by;
        
		// top-left vertex:
		quad->lu.vertices[0] = dx;
		quad->lu.vertices[1] = dy;
        
		// top-right vertex:
		quad->ru.vertices[0] = cx;
		quad->ru.vertices[1] = cy;
	} else {
		// bottom-left vertex:
		quad->ld.vertices[0] = newPos->x - size_2;
		quad->ld.vertices[1] = newPos->y - size_2;
        
		// bottom-right vertex:
		quad->rd.vertices[0] = newPos->x + size_2;
		quad->rd.vertices[1] = newPos->y - size_2;
        
		// top-left vertex:
		quad->lu.vertices[0] = newPos->x - size_2;
		quad->lu.vertices[1] = newPos->y + size_2;
        
		// top-right vertex:
		quad->ru.vertices[0] = newPos->x + size_2;
		quad->ru.vertices[1] = newPos->y + size_2;
	}

}

void GLParticleSystemQuad::postStep()
{
	glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0] );
#if TARGET_MAC
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Quad)*_particleCount, _particleList);
#elif TARGET_IOS
	// Option 3: Orphaning + glMapBuffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(Quad)*_totalParticles, NULL, GL_STREAM_DRAW);
	void *buf = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(buf, _particleList, sizeof(Quad) * _particleCount);
	glUnmapBuffer(GL_ARRAY_BUFFER);
#endif

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLParticleSystemQuad::initGLParticleSystemWithBundleRes(const char * dataFile)
{
    size_t length = 0;
    char *buff = NULL;
    length = loadBundleFileContextWithNameAPPLE(dataFile,&buff);
    if (length > 0)
    {
        this->initGLParticleSystem(buff,length);
        free(buff);    
    }
    else
    {
        Exception("Data Error", "GLParticleSystemQuad: can not init from null data.");
    }
}
void GLParticleSystemQuad::initGLParticleSystemWithBundleRes(const string & dataFile)
{
    this->initGLParticleSystemWithBundleRes(dataFile.c_str());
}

void GLParticleSystemQuad::initGLParticleSystem(unsigned int particleCount)
{
    GLParticleSystem::initGLParticleSystem(particleCount);
}
void GLParticleSystemQuad::initGLParticleSystem(const char * json_data, unsigned long length)
{
    GLParticleSystem::initGLParticleSystem(json_data, length);
}
