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


#ifndef __dAutomataState_h_
#define __dAutomataState_h_


class dAutomataState 
{	
	public:
	class Transition 
	{
		public:
		Transition (int character, dAutomataState* const targetdAutomataState);
		int GetCharater () const;
		dAutomataState* GetState() const;

		private:
		int m_character;
		dAutomataState* m_targetdAutomataState; 
	};

	dAutomataState (int id);
	dAutomataState (dTree<dAutomataState*,dAutomataState*>& subSet, int id);
	~dAutomataState ();

	void dAutomataState::GetStateArray (dList<dAutomataState*>& statesList);

	int m_id;
	int m_mark;
	bool m_exitState;
	dList<Transition> m_transtions;
	dList<dAutomataState*> m_myNFANullStates;
};


#endif