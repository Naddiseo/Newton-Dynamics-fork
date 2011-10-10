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


class TestSemantic
{
	int Test (int n)
	{
		// simple if
		if (n == 0)  int b;
		
		// defficult if
		if (n == 0) if (q == 1) int a; else int b;
		
		// complex if
		if (n == 0)  int a;
		else if (n == 1)  int b;
		else int c;
	
		// compund simple if
		if (n == 0)  { int a = 1;}

		// comploudn and complex if
		if (n == 0)  {
			int a;
			a = 1;
			int a = 1;
		} else if (n == 1){
			int b = 2;
		} else {
			int c = 3;
		}
	
		int a = 1;
		int b;
		int c;
		int d;
		
		
		{
			int x = 1;
			{
			}
			y = 0;
		}
		
		{
			x = 1;
			{
				int y = 0;
			} 
			y = 0;
		}
		
		int c = xxx.zzz.yyy (a, b, c);		

		
		a = a + a * (b - c) + (b - c) * d;
		if (n == 0)  {
			a = a + a * (b - c) + (b - c) * d + d;
			return 0;
		}
		if (n == 1) {
			a = a + a * (b - c) + (b - c) * d + d;
			return 1;
		}

		return Calculate (n - 1) + Calculate (n - 2);
	}

}

