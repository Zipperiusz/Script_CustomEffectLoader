#ifndef SCRIPTLEARNPOINTS_H_INCLUDED
#define SCRIPTLEARNPOINTS_H_INCLUDED

#include "Script.h"

gSScriptInit & GetScriptInit();

#include "Game.h"

#pragma warning( push )
#pragma warning( disable : 4127 )
#pragma warning( pop )

class mCCustomEffectLoader :
	public eCEngineComponentBase
{
	public: virtual void	Process(void);
public: virtual			~mCCustomEffectLoader(void);

	private:
		static bTPropertyObject<mCCustomEffectLoader, eCEngineComponentBase> ms_PropertyObjectInstance_mCCustomEffectLoader;

	public:
		mCCustomEffectLoader(void);

	private:
								mCCustomEffectLoader(mCCustomEffectLoader const &);
		mCCustomEffectLoader const &	operator = (mCCustomEffectLoader const &);
};

#endif