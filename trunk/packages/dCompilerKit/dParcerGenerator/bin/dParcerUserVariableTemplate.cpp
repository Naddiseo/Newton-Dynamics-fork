	class $(userVariable): public string 
	{	
		public:
		$(userVariable) ()
			:string()
		{
		}

		$(userVariable) (Token token, const char* const text)
			:string(text), m_token ()
		{
		}

		Token m_token;
	};


