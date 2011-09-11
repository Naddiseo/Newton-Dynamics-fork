	class $(userVariable): public string 
	{	
		public:
		$(userVariable) ()
			:string(), m_token (dToken(0))
		{
		}

		$(userVariable) (dToken token, const char* const text)
			:string(text), m_token (token)
		{
		}

		dToken m_token;
	};


