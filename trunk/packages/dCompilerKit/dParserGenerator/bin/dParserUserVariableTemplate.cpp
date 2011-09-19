	class dUserVariable
	{
		public:
		dUserVariable () 
			:m_token (dToken (0)), m_data("")
		{
		}


		dUserVariable (dToken token, const char* const data)
			:m_token(token), m_data (data) 
		{
		}
		dToken m_token;
		string m_data;
	};

