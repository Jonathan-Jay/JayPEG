#include "VerticalScroll.h"

VerticalScroll::VerticalScroll()
{
}

void VerticalScroll::Update()
{
	//Above Focus
	if (m_cam->GetPositionY() < m_topLimit) {
		if (m_focus->GetPosition().y > m_cam->m_localPosition.y + m_offset)
		{
			float difference = m_focus->GetPosition().y - (m_cam->m_localPosition.y + m_offset);

 			m_cam->SetPosition(vec3(m_cam->GetPositionX(), m_cam->GetPositionY() + difference, m_cam->GetPositionZ()));
		}
	}
	else {
		m_cam->SetPosition(vec3(m_cam->GetPositionX(), m_topLimit, m_cam->GetPositionZ()));
	}

	//Below focus
	if (m_cam->GetPositionY() > m_bottomLimit) {
		if (m_focus->GetPosition().y < m_cam->m_localPosition.y - m_offset)
		{
			float difference = m_focus->GetPosition().y - (m_cam->m_localPosition.y - m_offset);

			m_cam->SetPosition(vec3(m_cam->GetPositionX(), m_cam->GetPositionY() + difference, m_cam->GetPositionZ()));
		}
	}
	else {
		m_cam->SetPosition(vec3(m_cam->GetPositionX(), m_bottomLimit, m_cam->GetPositionZ()));
	}
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
