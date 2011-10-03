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

// naive method to calculate a fibonacci number
class fibonacci
{
	int Calculate (int n)
	{
	
		a = a + a * (b - c) + (b - c) * d;
//		if (n == 0)  {
//			return 0;
//		}
//		if (n == 1) {
//			return 1;
//		}

		return Calculate (n - 1) + Calculate (n - 2);
	}
}

