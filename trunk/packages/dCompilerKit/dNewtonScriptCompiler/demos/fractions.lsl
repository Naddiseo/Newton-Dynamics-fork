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

// a simple integer number fraction class 
class fractions
{
	fractions (int numerator, int denominator)
	{
int a = cast<byte> (a);		
int a = sizeof (a);
int a = sizeof (int);
	
		m_numerator = numerator;
		m_denominator = denominator;
		

		Reduce ();
	}

	//  Euclid's algorithm for calculating the greatest common divisor ofd tow integers
	private int gcd(int a, int b)
	{
		while(b) {
			int temp = a % b; 
			a = b; 
			b = temp;
		}

		return (a);
	}
	
	private void Reduce ()
	{
		int divisor = gcd(m_numerator, m_denominator);
		m_numerator = m_numerator / divisor;
		m_denominator = m_denominator / divisor;
	}

	
	int m_numerator;
	int m_denominator;
}

