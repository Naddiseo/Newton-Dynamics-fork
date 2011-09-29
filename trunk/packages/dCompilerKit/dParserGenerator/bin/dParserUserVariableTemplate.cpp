	class dUserVariable
	{
		public:
		dUserVariable () 
			:m_scannerLine (0), m_scannerIndex(0), m_token (dToken (0)), m_data("")
		{
		}


		dUserVariable (dToken token, const char* const data, int scannerLine, int scannerIndex)
			:m_scannerLine (scannerLine), m_scannerIndex(scannerIndex), m_token(token), m_data (data) 
		{
		}
		int m_scannerLine;
		int m_scannerIndex;
		dToken m_token;
		string m_data;
	};

