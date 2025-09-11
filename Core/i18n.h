struct LngEntry__ {
	i32         lang;
	tukk text;
};

void    AddModule(const LngEntry__ *entry, tukk modulename = NULL);


tukk    t_GetLngTxt(tukk id);  // Only works with string literals because of caching !

#define     LNG_enUS LNG_('E','N','U','S')
#define     LNG_ruRU LNG_('R','U','R','U')

tukk GetENUS(tukk id);

// t_ and tt_ would cause translation files sync to report error, therefore it
// has to be included from file that is not part of package...
#include "t_.h"


Txt        GetLngTxt(tukk id); // Only works with string literals because of caching !

Txt        GetLngTxt(i32 lang, tukk id);

Index<i32> GetLngSet();
Index<i32> GetLngSet(const Txt& module);
void       SaveLngFile(FileOut& out, i32 lang, i32 lang2 = 0);
bool       LoadLngFile(tukk file);

void       SetCurrentLanguage(i32 lang);
i32        GetCurrentLanguage();

void       LngSetAdd(tukk id, i32 lang, tukk txt, bool addid);
