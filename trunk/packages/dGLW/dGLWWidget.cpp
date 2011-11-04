#include "dGLWstdafx.h"
#include "dGLWWidget.h"
#include "dGLW.h"
#include "dGLWDrawContext.h"

dRttiRootClassSupportImplement(dGLWWidget);

dGLWWidget::dGLWWidget(dGLW* const glw, dGLWWidget* const parent, dFrameType type)
	:m_glw(glw)
	,m_parent(parent)
	,m_nativeHandle(0)
	,m_children()
	,m_type(type)
{
	m_rect.m_x = 0;
	m_rect.m_y = 0;
	m_rect.m_width = 1;
	m_rect.m_height = 1;
	m_client = m_rect;

	m_bkColor.m_red = 128;
	m_bkColor.m_green = 128;
	m_bkColor.m_blue = 128;
	m_bkColor.m_alpha = 255;

	if (m_parent) {
		m_parent->AddChild(this);
	} else {
		m_glw->AddRootWidget(this);
	}

	Create ();
	ShowWindow (m_nativeHandle, SW_SHOWNORMAL);
}


dGLWWidget::~dGLWWidget(void)
{
	dList<dGLWWidget*>::dListNode* nextNode;
	for (dList<dGLWWidget*>::dListNode* node = m_children.GetFirst(); node; node = nextNode) {
		nextNode = node->GetNext();
		dGLWWidget* const widget = node->GetInfo();
		_ASSERTE (widget->m_parent == this);
		SendMessage (widget->m_nativeHandle, WM_DESTROY, 0, 0);
	}

	if (m_parent) {
		m_parent->RemoveChild(this);
	} else {
		m_glw->RemoveRootWidget(this);
	}
}

dGLW* dGLWWidget::GetGLW() const
{
	return m_glw;
}


void dGLWWidget::AddChild(dGLWWidget* const child)
{
	for (dList<dGLWWidget*>::dListNode* node = m_children.GetFirst(); node; node = node->GetNext()) {
		if (node->GetInfo() == child) {
			return;
		}
	}
	m_children.Append(child);
}

void dGLWWidget::RemoveChild(dGLWWidget* const child)
{
	for (dList<dGLWWidget*>::dListNode* node = m_children.GetFirst(); node; node = node->GetNext()) {
		if (node->GetInfo() == child) {
			m_children.Remove(node);
			break;
		}
	}
}


void dGLWWidget::Create ()
{
	DWORD flags = 0;
	int x = CW_USEDEFAULT;
	int y = CW_USEDEFAULT;
	int w = CW_USEDEFAULT;
	int h = CW_USEDEFAULT;

	switch (m_type)
	{
		case m_child:
			x = 00;
			y = 00;
			w = 200;
			h = 200;

			flags = WS_CHILD;
			flags |= WS_BORDER;
			//flags |= WS_DLGFRAME;
			flags |= WS_VISIBLE;
			break;

		case m_frame:
			flags = WS_CAPTION;
			flags |= WS_OVERLAPPEDWINDOW;
			break;

		default:;
			_ASSERTE (0);
	}

//flags = WS_OVERLAPPEDWINDOW;
/*
//	DWORD flags = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;
//	DWORD flags = WS_OVERLAPPEDWINDOW;
//	DWORD flags = WS_DLGFRAME;
	DWORD flags = WS_CHILD;
static int xxx;
if (!xxx)
 flags = WS_CAPTION ;
else {
 flags = WS_CHILD;
}
//flags = WS_POPUPWINDOW; 
xxx ++;
//flags |= BS_AUTO3STATE; 
flags |= BS_PUSHBUTTON;
*/

	dGLW_HANDLE parentHandle = m_parent ? m_parent->m_nativeHandle : NULL;
	m_nativeHandle = CreateWindowEx(0, m_glw->m_winClass.lpszClassName, NULL, flags, x, y, w, h, parentHandle, NULL, m_glw->m_instance, this);

}


void dGLWWidget::OnPosition(int x, int y)
{
	RECT rect;
	GetWindowRect(m_nativeHandle, &rect);
	m_rect.m_x = x;
	m_rect.m_y = y;
	if (m_type == m_frame) {
//		m_client.m_x = x - rect.left;
//		m_client.m_y = y - rect.top;
		m_client.m_x = 0;
		m_client.m_y = 0;
	} else {
		m_client.m_x = x - (rect.left - m_parent->m_rect.m_x);
		m_client.m_y = y - (rect.top - m_parent->m_rect.m_y);
	}
}

void dGLWWidget::Update()
{
	UpdateWindow(m_nativeHandle);
}

void dGLWWidget::SetSize(int width, int height)
{
	SetWindowPos(m_nativeHandle, HWND_TOP, m_rect.m_x, m_rect.m_y, width, height, 0);
}

void dGLWWidget::OnSize(int width, int height)
{
	RECT rect;
	GetWindowRect(m_nativeHandle, &rect);
	m_rect.m_width = rect.right - rect.left;
	m_rect.m_height =  rect.bottom -  rect.top;

	m_client.m_width = width;
	m_client.m_height = height;
}

void dGLWWidget::SetBackgroundColor(const dGLWColor& color)
{
	m_bkColor = color;
	Update();
}

dGLWWidget::dFrameType dGLWWidget::GetWidndowType() const
{
	return m_type;
}

dGLWColor dGLWWidget::GetBackgroundColor() const 
{	
	return m_bkColor;
}

void dGLWWidget::PredictNewSize(int& width, int& height) const
{
	width = m_rect.m_width;
	height = m_rect.m_height;
}

dGLWRect dGLWWidget::GetClientRect() const
{
	return m_client;
}

dGLWRect dGLWWidget::GetRect() const
{
	return m_rect;
}

/*
void dGLWWidget::OnSize(int width, int height)
{
//	unsigned size = (width << 16) + height & 0xffff;
//	SendMessage (m_nativeHandle, WM_SIZE, 0, size);

	SetWindowPos(m_nativeHandle, HWND_TOP, m_rect.m_x, m_rect.m_y, width, height, 0);
	Update();
}
*/

void dGLWWidget::OnPaint(dGLWDrawContext& gdc)
{
	gdc.SetBrushColor (m_bkColor);
//	gdc.ClearRectangle (m_client.m_x, m_client.m_height, m_client.m_y + m_client.m_width, m_client.m_height + m_client.m_height);
//	gdc.ClearRectangle (m_client.m_x, m_client.m_height, m_client.m_x + m_client.m_width, m_client.m_y + m_client.m_height);
	gdc.ClearRectangle (m_client.m_x, m_client.m_y, m_client.m_x + m_client.m_width, m_client.m_y + m_client.m_height);
//	gdc.DrawLine( 100, 100, 200, 200);
}