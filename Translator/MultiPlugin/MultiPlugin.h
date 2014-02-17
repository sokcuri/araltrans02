#ifndef __MULTIPLUGIN_H__
#define __MULTIPLUGIN_H__

#include "tstring.h"
#include <vector>

using namespace std;

typedef struct _tagATPLUGIN_ARGUMENT {
	tstring strPluginName;
	tstring strOption;
} ATPLUGIN_ARGUMENT, *PATPLUGIN_ARGUMENT;

typedef vector<ATPLUGIN_ARGUMENT> ATPLUGIN_ARGUMENT_ARRAY;

#include "ATPluginContainer.h"

typedef struct {
	tstring strName;
	CATPluginContainer cPlugin;
} PLUGININFO;


class CMultiPlugin {
	protected:

		typedef vector<PLUGININFO *> PLUGINPOINTERARRAY;

		PLUGINPOINTERARRAY m_apFilters;

		PLUGININFO *m_pTranslator;

		bool m_bIsHooker;
	
	public:
		CMultiPlugin() : m_pTranslator(NULL), m_bIsHooker(false) {}
		~CMultiPlugin() { RemoveAll(); }

		const PLUGININFO *GetTranslatorPluginInfo() const { return m_pTranslator; }

		BOOL Add(const ATPLUGIN_ARGUMENT &pluginArg, bool bIsTranslator);
		BOOL Add(const ATPLUGIN_ARGUMENT_ARRAY &aPluginArgs);
		BOOL Remove(int nIndex);

		BOOL MoveUp(int nIndex);
		BOOL MoveDown(int nIndex);

		BOOL MoveTop(int nIndex);
		BOOL MoveBottom(int nIndex);

		void RemoveAll();

		int GetFilterCount() { return m_apFilters.size(); }
		void GetPluginArgs(ATPLUGIN_ARGUMENT_ARRAY &aPluginArgs);

		// 메세지 전송 메소드
		BOOL OnPluginStart();
		BOOL OnPluginStop();
		BOOL Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize);

		BOOL OnPluginOption(int nIndex);

		const ATPLUGIN_VERSION *OnPluginVersion(int nIndex);

	protected:
		PLUGININFO *LoadPlugin(const TCHAR *pszPluginDirectory, const ATPLUGIN_ARGUMENT &pluginArg);
};

extern CMultiPlugin g_cMultiPlugin;

#endif // __MULTIPLUGIN_H__