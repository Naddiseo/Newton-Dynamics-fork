/* Copyright (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#include <toolbox_stdafx.h>
#include "OpenGlUtil.h"
#include "dRuntimeProfiler.h"
#include "dHeightResolutionTimer.h"


dRuntimeProfiler::dRuntimeProfiler(int origin_x, int origin_y)
{
	m_oringin_x = origin_x;
	m_oringin_y = origin_y;
	m_frameIndex = 0;
	memset (m_perfomanceTracks, 0, sizeof (m_perfomanceTracks));
}

dRuntimeProfiler::~dRuntimeProfiler(void)
{
}

unsigned dRuntimeProfiler::GetTimeInMicrosenconds()
{
	return unsigned (dGetTimeInMicrosenconds());
}

void dRuntimeProfiler::DrawLabel (dFloat x, dFloat y, const char* label, QPainter& context)
{
	QRect viewport (context.viewport());
	context.drawText(x, viewport.height() + 1 - y, label);
}


void dRuntimeProfiler::DrawTrack (dFloat x0, dFloat y0, int start, unsigned* track, QPainter& context)
{
	unsigned buffer[2048];

	int index = 0;
	for (int i = start + 1; i < MAX_FRAMES; i ++) {
		buffer[index] = track[i];
		index ++;
	}

	for (int i = 0; i < start; i ++) {
		buffer[index] = track[i];
		index ++;
	}
	buffer[index] = track[start];


	QRect viewport (context.viewport());
	int height = viewport.height();
	dFloat xx0 = x0;
	dFloat yy0 = y0 + dFloat (buffer[0]) * 1.0e-3f * (CHART_HIEGHT / 16.666f);;
	for (int i = 1; i < MAX_FRAMES - 1; i ++) {
		dFloat xx1 = x0 + i * MAX_FRAMES_STEP;
		dFloat yy1 = y0 + dFloat (buffer[i]) * 1.0e-3f * (CHART_HIEGHT / 16.666f);
		context.drawLine(xx0, height - yy0, xx1, height - yy1);
		xx0 = xx1;
		yy0 = yy1;
	}
}


void dRuntimeProfiler::Render (NewtonWorld* nWorld, int mask, QPainter& context)
{
	//Retrieves the viewport and stores it in the variable
	for (int i = 0; i < MAX_TRACKS; i ++) {
		m_perfomanceTracks[i][m_frameIndex] = NewtonReadPerformanceTicks (nWorld, i);
	}

	QRect viewport (context.viewport());
	int height = viewport.height();
	dFloat x0 = dFloat (m_oringin_x);
	dFloat y0 = dFloat (m_oringin_y);
	dFloat x1 = x0 + MAX_FRAMES * MAX_FRAMES_STEP;
	dFloat y1 = y0 + CHART_HIEGHT;


	context.setPen(Qt::white);
	context.drawLine (x0, height - y0, x1, height - y0);
	context.drawLine (x0, height - y0, x0, height - y1);

	for (int i = 1; i < 4; i ++) {
		dFloat y = y0 + (y1 - y0) * i / 4;
		context.drawLine (x0 - 5, height - y, x0 + 5, height - y);
	}

	for (int i = 1; i < MAX_FRAMES; i += 16) {
		dFloat x = x0 + (x1 - x0) * i / MAX_FRAMES;
		context.drawLine (x, height - (y0 - 5), x, height - (y0 + 5));
	}

	// total engine time
	if (mask & 1) {
		context.setPen(Qt::white);
		DrawTrack (x0, y0, m_frameIndex, &m_perfomanceTracks[NEWTON_PROFILER_WORLD_UPDATE][0], context);
	}


	// draw collision performance
	if (mask & 2) { 
		context.setPen(Qt::blue);
//		DrawTrack (x0, y0, dVector (0.0f, 0.0f, 1.0f), m_frameIndex, &m_perfomanceTracks[NEWTON_PROFILER_COLLISION_UPDATE][0]);
		DrawTrack (x0, y0, m_frameIndex, &m_perfomanceTracks[NEWTON_PROFILER_COLLISION_UPDATE][0], context);
	}


	// draw force Update performance
	if (mask & 4) { 
//		DrawTrack (x0, y0, dVector (0.5f, 1.0f, 0.5f), m_frameIndex, &m_perfomanceTracks[NEWTON_PROFILER_FORCE_CALLBACK_UPDATE][0]);
	}

	if (mask & 8) { 
//		DrawTrack (x0, y0, dVector (0.0f, 0.0f, 1.0f), m_frameIndex, &m_perfomanceTracks[NEWTON_PROFILER_COLLISION_UPDATE_BROAD_PHASE][0]);
	}

	if (mask & 16) { 
//		DrawTrack (x0, y0, dVector (1.0f, 0.0f, 0.0f), m_frameIndex, &m_perfomanceTracks[NEWTON_PROFILER_COLLISION_UPDATE_NARROW_PHASE][0]);
	}


	// draw dynamics performance
	if (mask & 32) { 
		context.setPen(Qt::red);
		DrawTrack (x0, y0, m_frameIndex, &m_perfomanceTracks[NEWTON_PROFILER_DYNAMICS_UPDATE][0], context);
	}

	if (mask & 64) { 
//		DrawTrack (x0, y0, dVector (1.0f, 1.0f, 0.0f), m_frameIndex, &m_perfomanceTracks[NEWTON_PROFILER_DYNAMICS_SOLVE_CONSTRAINT_GRAPH][0]);
	}

	if (mask & 128) { 
//		DrawTrack (x0, y0, dVector (1.0f, 0.0f, 1.0f), m_frameIndex, &m_perfomanceTracks[NEWTON_PROFILER_DYNAMICS_CONSTRAINT_GRAPH][0]);
	}

	{
		context.setPen(Qt::white);
		DrawLabel (x0 - 42, y0 + (y1 - y0) * 0 / 4, "0.00", context);

		for (int i = 1; i < 5; i ++) {
			char label[32];
			sprintf (label, "%4.2f", (1000.0f / 60.0f) * (float)i / 4.0f );
			DrawLabel (x0 - 42, y0 + (y1 - y0) * i / 4, label, context);
		}
	}

	m_frameIndex = (m_frameIndex + 1) % MAX_FRAMES;
}


void dRuntimeProfiler::RenderThreadPerformance (NewtonWorld* nWorld, QPainter& context)
{
	int threadCount = NewtonGetThreadsCount(nWorld);
	if (threadCount > 0) {

		QRect viewport (context.viewport());

		float x0 = viewport.width() - 310; 
		float x1 = x0 + 256.0f;
		float y0 = 50.0f;
		for (int i = 0; i < threadCount; i ++) {
			char label[32];
			sprintf (label, "thread %d", i);
			DrawLabel (x0 - 50, y0 + i * 20 + 5, label, context);
		}
		
		DrawLabel (x0, y0 - 20, "0.0 ms", context);
		DrawLabel ((x1 + x0) * 0.5f, y0 - 20, "8.33 ms", context);
		DrawLabel (x1, y0 - 20, "16.66 ms", context);
		
		int height = viewport.height();
		QBrush brush (Qt::SolidPattern);
		brush.setColor (QColor(255, 255, 0, 64));
		context.fillRect(x0, height - (y0 + 20.0f * threadCount), x1 - x0, 20 * threadCount, brush);

		brush.setColor (QColor(255, 0, 0, 128));
		for (int i = 0; i < threadCount; i ++) {
			int thick = NewtonReadThreadPerformanceTicks (nWorld, i);
			dFloat time = dFloat (thick) * (1.0e-3f * 256.0f / 16.666f);
			context.fillRect(x0, height - (y0 + 15), time, 10, brush);
			y0 += 20.0f;
		}
	}
}

