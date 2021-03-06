#ifndef __VERTICALSCROLL_H__
#define __VERTICALSCROLL_H__

#include "Camera.h"
#include "Timer.h"

class VerticalScroll
{
public:
	VerticalScroll();

	void Update();

	//Getters
	Camera* GetCam() const;
	Transform* GetFocus() const;
	float GetOffset() const;
	float GetTopLimit() const;
	float GetBottomLimit() const;

	//Setters
	void SetCam(Camera* cam);
	void SetFocus(Transform* focus);
	void SetOffset(float offset);
	void SetLimits(float bottom, float top);
	void SetTopLimit(float top);
	void SetBottomLimit(float bottom);
	void DoScreenShake(float time, float strength, float* endPos = nullptr);
		
private:
	Camera* m_cam = nullptr;
	Transform* m_focus = nullptr;

	float m_offset = 5.f;
	float m_topLimit = INT_MAX;
	float m_bottomLimit = INT_MIN;
	float m_shakeTime = 0.f;
	float m_shakeStrength = 0.f;
	float* m_shakeEndPos = nullptr;
};

//Sends vertical scrolling camera TO json file
inline void to_json(nlohmann::json& j, const VerticalScroll& vertScroll)
{
	j["Offset"] = vertScroll.GetOffset();
}

//Sends vertical scrolling camera TO json file
inline void from_json(const nlohmann::json& j, VerticalScroll& vertScroll)
{
	vertScroll.SetOffset(j["Offset"]);
	
}

#endif // !__VERTICALSCROLL_H__
