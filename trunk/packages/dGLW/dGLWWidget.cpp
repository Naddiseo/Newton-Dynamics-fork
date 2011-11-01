#include "dGLWstdafx.h"
#include "dGLWWidget.h"
#include "dGLW.h"

dRttiRootClassSupportImplement(dGLWWidget);

dGLWWidget::dGLWWidget(dGLW* const glw, dGLWWidget* const parent, dFrameType type)
	:m_glw(glw)
	,m_parent(parent)
	,m_nativeHandle(0)
	,m_children()
{
	if (m_parent) {
		m_parent->AddChild(this);
	} else {
		m_glw->AddRootWidget(this);
	}

	Create (type);
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


void dGLWWidget::Create (dFrameType type)
{
	DWORD flags = 0;
	switch (type)
	{
		case m_child:
			flags = WS_CHILD;
			break;

		case m_frame:
			flags = WS_CAPTION;
			flags |= WS_OVERLAPPEDWINDOW;;
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

	int x = CW_USEDEFAULT;
	int y = CW_USEDEFAULT;
	int w = CW_USEDEFAULT;
	int h = CW_USEDEFAULT;
	dGLW_HANDLE parentHandle = m_parent ? m_parent->m_nativeHandle : NULL;
	m_nativeHandle = CreateWindowEx(0, m_glw->m_winClass.lpszClassName, NULL, flags, x, y, w, h, parentHandle, NULL, m_glw->m_instance, this);
//	m_nativeHandle = CreateWindowEx(0, "BUTTON", NULL, flags, x, y, w, h, parentHandle, NULL, m_glw->m_instance, this);
}

