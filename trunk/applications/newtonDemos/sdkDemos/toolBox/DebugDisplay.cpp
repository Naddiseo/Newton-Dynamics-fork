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

// dGeometry.cpp: implementation of the dGeometry class.
//
//////////////////////////////////////////////////////////////////////
#include <toolbox_stdafx.h>
#include <DebugDisplay.h>

void RenderContactPoints (NewtonWorld* const world)
{
	glDisable (GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_LINES);
	float length = 0.5f;
	for (NewtonBody* body = NewtonWorldGetFirstBody(world); body; body = NewtonWorldGetNextBody(world, body)) {
		for (NewtonJoint* joint = NewtonBodyGetFirstContactJoint(body); joint; joint = NewtonBodyGetNextContactJoint(body, joint)) {
			for (void* contact = NewtonContactJointGetFirstContact (joint); contact; contact = NewtonContactJointGetNextContact (joint, contact)) {
				dVector point;
				dVector normal;	
				NewtonMaterial* const material = NewtonContactGetMaterial (contact);
				NewtonMaterialGetContactPositionAndNormal (material, &point.m_x, &normal.m_x);

				// if we are display debug info we need to block other threads from writing the data at the same time
				dVector p0 (point + normal.Scale (length));
				dVector p1 (point - normal.Scale (length));
				glVertex3f (p0.m_x, p0.m_y, p0.m_z);
				glVertex3f (p1.m_x, p1.m_y, p1.m_z);
			}
		}
	}
	glEnd();

	glPointSize(8.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	for (NewtonBody* body = NewtonWorldGetFirstBody(world); body; body = NewtonWorldGetNextBody(world, body)) {
		for (NewtonJoint* joint = NewtonBodyGetFirstContactJoint(body); joint; joint = NewtonBodyGetNextContactJoint(body, joint)) {
			for (void* contact = NewtonContactJointGetFirstContact (joint); contact; contact = NewtonContactJointGetNextContact (joint, contact)) {
				dVector point;
				dVector normal;	
				NewtonMaterial* const material = NewtonContactGetMaterial (contact);
				NewtonMaterialGetContactPositionAndNormal (material, &point.m_x, &normal.m_x);

				// if we are display debug info we need to block other threads from writing the data at the same time
				glVertex3f (point.m_x, point.m_y, point.m_z);

			}
		}
	}
	glEnd();
	glPointSize(1.0f);
}




void DebugShowGeometryCollision (void* userData, int vertexCount, const dFloat* faceVertec, int id)
{
	int i = vertexCount - 1;
	dVector p0 (faceVertec[i * 3 + 0], faceVertec[i * 3 + 1], faceVertec[i * 3 + 2]);
	for (int i = 0; i < vertexCount; i ++) {
		dVector p1 (faceVertec[i * 3 + 0], faceVertec[i * 3 + 1], faceVertec[i * 3 + 2]);
		glVertex3f (p0.m_x, p0.m_y, p0.m_z);
		glVertex3f (p1.m_x, p1.m_y, p1.m_z);
		p0 = p1;
	}
}



static void DebugShowBodyCollision (const NewtonBody* const body)
{
	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;

	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);	
	if (mass > 0.0f) {

		int sleepState = NewtonBodyGetSleepState(body);
		if (sleepState == 1) {
			// indicate when body is sleeping 
			glColor3f(0.42f, 0.73f, 0.98f);
		} else {
			// body is active
			glColor3f(1.0f, 1.0f, 1.0f);
		}

		dMatrix matrix;
		NewtonBodyGetMatrix(body, &matrix[0][0]);
		NewtonCollisionForEachPolygonDo (NewtonBodyGetCollision(body), &matrix[0][0], DebugShowGeometryCollision, NULL);

	} else {

		NewtonCollisionInfoRecord info;

		glColor3f(1.0f, 1.0f, 0.0f);
		NewtonCollision* const collision = NewtonBodyGetCollision (body);
		NewtonCollisionGetInfo (collision, &info);

		switch (info.m_collisionType) 
		{
			//case SERIALIZE_ID_TREE:
			//case SERIALIZE_ID_SCENE:
			case SERIALIZE_ID_USERMESH:
			case SERIALIZE_ID_HEIGHTFIELD:
			{
				break;
			}

			default: 
			{
				dMatrix matrix;
				NewtonBodyGetMatrix(body, &matrix[0][0]);
				NewtonCollisionForEachPolygonDo (NewtonBodyGetCollision(body), &matrix[0][0], DebugShowGeometryCollision, NULL);
				break;
			}
		}
	}
}


void DebugRenderWorldCollision (const NewtonWorld* const world)
{
	glDisable (GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

//	dVector aabbP0 (-1000.0f, -500.0f, -1000.0f);
//	dVector aabbP1 (1000.0f, 500.0f, 1000.0f);

	glBegin(GL_LINES);
//	NewtonWorldForEachBodyInAABBDo (world, &aabbP0[0], &aabbP1[0], DebugShowBodyCollision, NULL);
	for (NewtonBody* body = NewtonWorldGetFirstBody(world); body; body = NewtonWorldGetNextBody(world, body)) {
		DebugShowBodyCollision (body);
	}
	glEnd();
}
