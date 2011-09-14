	class dUserVariable: public string 
	{	
		public:
		dUserVariable ()
			:string(), m_token (dToken(0))
		{
		}

		dUserVariable (dToken token, const char* const text)
			:string(text), m_token (token)
		{
		}
		dToken m_token;
	};


