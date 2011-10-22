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

// a very naive qsort for integers
class qsort
{

	qsort (int[] a, int size)
	{
z = 1;	
int i = z;	
/*	
		// first make sure the first element is the smallest of the array 
		// and that the last element is the largest of the array. 
		for (int i = 1; i < size; i = i + 1) {
			if (a[0] > a[i]) {
				int x = a[0];
				a[0] = a[i];
				a[i] = x;
			}
			if (a[size - i - 1] > a[size - 1]) {
				int x = a[size - i - 1];
				a[size - i - 1] = a[size - 1];
				a[size - 1] = x;
			}
		}
		
		// now do a recursive qsort 
		sort (a, 1, size -1);
*/		
	}

/*
	// very primitive qsort 
	private void sort (int[] a, int m, int n)
	{

		if (n <= m) {
			return ;
		}

		int i = m - 1;
		int j = n;
		int v = a[n];
		int x;
		
		while (1) {
	
			do 
				i = i + 1;
			while (a[i] < v);
			
			do 
				j = j - 1;
			while (a[j] > v);

			if (i >= j) {
				break;
			}
			
			x = a[i];
			a[i] = a[j];
			a[j] = x;
		}
		
		x = a[i];
		a[i] = a[n];
		a[n] = x;
		sort (a, m, j);
		sort (a, i + 1, n);
	}
*/
}

