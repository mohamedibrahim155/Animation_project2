#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/easing.hpp>
#include <vector>
#include "../model.h"
#include <functional>

enum class EasingType
{
	Linear,
	sineEaseIn,
	sineEaseOut,
	sineEaseInOut
};


struct PositionKeyFrame
{
	PositionKeyFrame(const glm::vec3& _position, double _time, EasingType _easeType = EasingType::Linear) :
		position(_position), 
		time(_time), 
		easeType(_easeType)
	{
	}

	glm::vec3 position;
	EasingType easeType;
	double time;

};



struct RotationKeyFrame
{
	RotationKeyFrame(const glm::quat& _rotation, double _time, EasingType _easeType = EasingType::Linear) :
		rotation(_rotation),
		time(_time),
		easeType(_easeType)
	{
	}

	RotationKeyFrame(const glm::vec3& _rotation, double _time, EasingType _easeType = EasingType::Linear) :
		rotation_vec3(_rotation),
		time(_time),
		easeType(_easeType)
	{
	}

	glm::quat rotation =  glm::quat(1,0,0,1);
	glm::vec3 rotation_vec3 =  glm::vec3(0);
	EasingType easeType;
	double time;

};



struct ScaleKeyFrame
{
	ScaleKeyFrame(const glm::vec3& _scale, double _time, EasingType _easeType = EasingType::Linear) :
		scale(_scale),
		time(_time),
		easeType(_easeType)
	{
	}

	glm::vec3 scale;
	EasingType easeType;
	double time;

};

struct ColorKeyFrame
{
	ColorKeyFrame(const glm::vec3& _color, double _time, EasingType _easeType = EasingType::Linear) :
		color(_color),
		time(_time),
		easeType(_easeType)
	{}

	glm::vec3 color;
	EasingType easeType;
	double time;
};

struct EventKeyFrame
{
	EventKeyFrame(double _time, std::function<void()> OnEventCallback) : time(_time), callBack(OnEventCallback)
	{
	}

	double time;
	std::function<void()> callBack;
	bool isEventTriggered = false;
};


class Animation
{
public:

	void AddPositionKeyFrame(const glm::vec3& position, double time, EasingType easeType = EasingType::Linear);
	void AddRotationKeyFrame(const glm::quat& rotation, double time, EasingType easeType = EasingType::Linear);
	void AddRotationKeyFrame(const glm::vec3& rotation, double time, EasingType easeType = EasingType::Linear);
	void AddScaleKeyFrame(const glm::vec3& scale, double time, EasingType easeType = EasingType::Linear);
	void AddColoreKeyFrame(const glm::vec3& color, double time, EasingType easeType = EasingType::Linear);
	void AddEventKeyFrame( double time, std::function<void()> OnEventCallback = nullptr);

	void SetAnimationTime(float time);

	std::vector<PositionKeyFrame> positionKeyFrameList;
	std::vector<RotationKeyFrame> rotationKeyFrameList;
	std::vector<ScaleKeyFrame> scaleKeyFrameList;
	std::vector<ColorKeyFrame> colorKeyFrameList;
	std::vector<EventKeyFrame*> eventKeyFrameList;


	double time;
	bool isPaused = false;

};





