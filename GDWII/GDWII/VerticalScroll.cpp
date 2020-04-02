#include "VerticalScroll.h"

VerticalScroll::VerticalScroll()
{
}

void VerticalScroll::Update()
{
	float difference = m_cam->GetPositionY();
	if (m_focus->GetPositionY() > m_cam->GetPositionY() + m_offset) {
		//Calculate the amount the focus has "pushed" the camera right by
		difference += m_focus->GetPositionY() - (m_cam->GetPositionY() + m_offset);
	}
	else if (m_focus->GetPositionY() < m_cam->GetPositionY() - m_offset) {
		//Calculate the amount the focus has "pushed" the camera left by
		difference += m_focus->GetPositionY() - (m_cam->GetPositionY() - m_offset);
	}

	if (m_shakeTime > 0) {
		m_shakeTime -= Timer::deltaTime;
		if (m_shakeTime == 0)
			m_shakeTime = -1;

		if (difference + m_cam->GetOrthoSize().w > m_topLimit) {				//Top of focus
			difference = m_topLimit - m_cam->GetOrthoSize().w;
		}
		else if (difference - m_cam->GetOrthoSize().w < m_bottomLimit) {	//Bottom of focus
			difference = m_bottomLimit + m_cam->GetOrthoSize().w;
		}
		difference += rand() % int(2 * m_shakeStrength) - m_shakeStrength;
	}
	else if (m_shakeTime < 0) {
		if (m_shakeEndPos != nullptr)
			difference = *m_shakeEndPos;

		m_shakeTime = 0;
		m_shakeStrength = 0;
		m_shakeEndPos = nullptr;
	}

	if (difference + m_cam->GetOrthoSize().w > m_topLimit) {				//Top of focus
		difference = m_topLimit - m_cam->GetOrthoSize().w;
	} else if (difference - m_cam->GetOrthoSize().w < m_bottomLimit) {	//Bottom of focus
		difference = m_bottomLimit + m_cam->GetOrthoSize().w;
	}

	//Adjust the camera
	m_cam->SetPosition(vec3(m_cam->GetPositionX(), difference, m_cam->GetPositionZ()));
}

Camera * VerticalScroll::GetCam() const
{
	return m_cam;
}

Transform * VerticalScroll::GetFocus() const
{
	return m_focus;
}

float VerticalScroll::GetOffset() const
{
	return m_offset;
}

float VerticalScroll::GetTopLimit() const
{
	return m_topLimit;
}

float VerticalScroll::GetBottomLimit() const
{
	return m_bottomLimit;
}

void VerticalScroll::SetCam(Camera * cam)
{
	m_cam = cam;
}

void VerticalScroll::SetFocus(Transform * focus)
{
	m_focus = focus;
}

void VerticalScroll::SetOffset(float offset)
{
	m_offset = offset;
}

void VerticalScroll::SetLimits(float bottom, float top)
{
	if (bottom < top) {
		m_bottomLimit = bottom;
		m_topLimit = top;
	}
}

void VerticalScroll::SetTopLimit(float top)
{
	if (m_bottomLimit < top) {
		m_topLimit = top;
	}
}

void VerticalScroll::SetBottomLimit(float bottom)
{
	if (bottom < m_topLimit) {
		m_bottomLimit = bottom;
	}
}

void VerticalScroll::DoScreenShake(float time, float strength, float* endPos) {
	m_shakeTime = time;
	m_shakeStrength = strength;

	if (endPos != nullptr)
		m_shakeEndPos = endPos;
}
