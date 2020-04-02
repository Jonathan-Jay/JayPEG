#include "HorizontalScroll.h"

HorizontalScroll::HorizontalScroll()
{
}

void HorizontalScroll::Update()
{
	float difference = m_cam->GetPositionX();
	if (m_shakeTime > 0) {
		m_shakeTime -= Timer::deltaTime;
		if (m_shakeTime == 0)
			m_shakeTime = -1;

		difference += rand() % int(2 * m_shakeStrength) - m_shakeStrength;
	}
	else if (m_shakeTime < 0) {
		if (m_shakeEndPos != nullptr)
			difference = *m_shakeEndPos;

		m_shakeTime = 0;
		m_shakeStrength = 0;
		m_shakeEndPos = nullptr;
	}
	else {
		if (m_focus->GetPositionX() > m_cam->GetPositionX() + m_offset) {
			//Calculate the amount the focus has "pushed" the camera right by
			difference += m_focus->GetPositionX() - (m_cam->GetPositionX() + m_offset);
		}
		else if (m_focus->GetPositionX() < m_cam->GetPositionX() - m_offset) {
			//Calculate the amount the focus has "pushed" the camera left by
			difference += m_focus->GetPositionX() - (m_cam->GetPositionX() - m_offset);
		}
	}

	float adjustment = m_cam->GetOrthoSize().y * m_cam->GetAspect();
	if (difference + adjustment > m_rightLimit) {		//Right of focus
		difference = m_rightLimit - adjustment;
	} else if (difference - adjustment < m_leftLimit) {	//Left of focus
		difference = m_leftLimit + adjustment;
	}

	//Adjust the camera
	m_cam->SetPosition(vec3(difference, m_cam->GetPositionY(), m_cam->GetPositionZ()));
}

Camera * HorizontalScroll::GetCam() const
{
	return m_cam;
}

Transform * HorizontalScroll::GetFocus() const
{
	return m_focus;
}

float HorizontalScroll::GetOffset() const
{
	return m_offset;
}

float HorizontalScroll::GetLeftLimit() const
{
	return m_leftLimit;
}

float HorizontalScroll::GetRightLimit() const
{
	return m_rightLimit;
}

void HorizontalScroll::SetCam(Camera * cam)
{
	m_cam = cam;
}

void HorizontalScroll::SetFocus(Transform * focus)
{
	m_focus = focus;
}

void HorizontalScroll::SetOffset(float offset)
{
	m_offset = offset;
}

void HorizontalScroll::SetLimits(float left, float right)
{
	if (left < right) {
		m_leftLimit = left;
		m_rightLimit = right;
	}
}

void HorizontalScroll::SetRightLimit(float right)
{
	if (m_leftLimit < right) {
		m_rightLimit = right;
	}
}

void HorizontalScroll::SetLeftLimit(float left)
{
	if (left < m_rightLimit)
		m_leftLimit = left;
}

void HorizontalScroll::DoScreenShake(float time, float strength, float* endPos) {
	m_shakeTime = time;
	m_shakeStrength = strength;

	if (endPos != nullptr)
		m_shakeEndPos = endPos;
}
