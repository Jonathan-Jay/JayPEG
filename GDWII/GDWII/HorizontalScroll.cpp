#include "HorizontalScroll.h"

HorizontalScroll::HorizontalScroll()
{
}

void HorizontalScroll::Update()
{
	//Right of focus
	if (m_cam->GetPositionX() < m_rightLimit) {
		if (m_focus->GetPosition().x > m_cam->m_localPosition.x + m_offset)
		{
			//Calculate the amount the focus has "pushed" the camera right by
			float difference = m_focus->GetPosition().x - (m_cam->m_localPosition.x + m_offset);

			//Adjust the camera
			m_cam->SetPosition(vec3(m_cam->GetPositionX() + difference, m_cam->GetPositionY(), m_cam->GetPositionZ()));
		}
	}
	else {
		m_cam->SetPosition(vec3(m_rightLimit, m_cam->GetPositionY(), m_cam->GetPositionZ()));
	}

	//Left of focus
	if (m_cam->GetPositionX() > m_leftLimit) {
		if (m_focus->GetPosition().x < m_cam->m_localPosition.x - m_offset)
		{
			//Calculate the amount the focus has "pushed" the camera left by
			float difference = m_focus->GetPosition().x - (m_cam->m_localPosition.x - m_offset);

			//Adjust the camera
			m_cam->SetPosition(vec3(m_cam->GetPositionX() + difference, m_cam->GetPositionY(), m_cam->GetPositionZ()));
		}
	}
	else {
		m_cam->SetPosition(vec3(m_leftLimit, m_cam->GetPositionY(), m_cam->GetPositionZ()));
	}
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
