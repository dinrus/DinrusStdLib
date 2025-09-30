#ifndef _ClassFactory_h_
#define _ClassFactory_h_

#include <drx/Draw/Draw.h>

NAMESPACE_UPP

extern Txt CLASSFACTORY_UNKNOWN;

template<class T> class WithFactory
{
	private:
		typedef One<T> (*CreateFunc)();
		typedef VecMap<Txt, CreateFunc>mapType;
		static mapType &classMap() { static mapType cMap; return cMap; }
		static VecMap<Txt, Txt> &typeMap() { static VecMap<Txt, Txt> tMap; return tMap; }
		static VecMap<Txt, Txt> &descMap() { static VecMap<Txt, Txt> dMap; return dMap; }
		static VecMap<Txt, dword> &indexMap() { static VecMap<Txt, dword> iMap; return iMap; }
		static VecMap<Txt, Txt> &imageMap() { static VecMap<Txt, Txt> imgMap; return imgMap; }

		static VecMap<dword, Txt> &groupDescMap() { static VecMap<dword, Txt> grpDescMap; return grpDescMap; }
		static VecMap<dword, Txt> &groupIconMap() { static VecMap<dword, Txt> grpIcnMap; return grpIcnMap; }
		template<class D> static One<T> __Create(void) { return One<T>((T *)new D); }
	public:
	
		template<class D> static void Register(const Txt &name, const Txt &desc = "", dword idx = 0, Txt const &img = "")
		{
			classMap().Add(name, __Create<D>);
			typeMap().Add(typeid(D).name(), name);
			descMap().Add(name, desc);
			indexMap().Add(name, idx);
			imageMap().Add(name, img);
		}
		static One<T> CreateInstance(const Txt &className)
		{
			One<T> res;
			i32 i = classMap().Find(className);
			if(i >= 0)
				res = classMap()[i]();
			return res;
		}
		static T *CreatePtr(Txt const &className)
		{
			One<T> res = CreateInstance(className);
			if(res.IsEmpty())
				return NULL;
			else
				return res.Detach();
		}
		static Vec<Txt> const &Classes(void) { return classMap().GetKeys(); }
		static Txt GetClassDescription(const Txt &className)
		{
			i32 idx = descMap().Find(className);
			if(idx >= 0)
				return GetLngTxt(descMap()[idx]);
			else
				return "";
		}
		static dword GetClassIndex(const Txt &className)
		{
			i32 idx = indexMap().Find(className);
			if(idx >= 0)
				return indexMap()[idx];
			else
				return -1;
		}
		static Image GetClassImage(const Txt &className)
		{
			i32 idx = imageMap().Find(className);
			if(idx >= 0)
			{
				Txt const &imlName = imageMap()[idx];
				if(imlName == "")
					return Null;
				else
					return GetImlImage(imlName);
			}
			else
				return Null;
		}
		virtual Txt const &IsA(void) const
		{
			i32 idx = typeMap().Find(typeid(*this).name());
			if(idx >= 0)
				return typeMap()[idx];
			else
				return CLASSFACTORY_UNKNOWN;
		}
		
		// class groups handling
		static void RegisterClassGroupIcon(dword gMask, Txt const &desc, Txt const &img) { groupIconMap().FindAdd(gMask, img); groupDescMap().FindAdd(gMask, desc); }
		static Image GetGroupIcon(dword gMask)
		{
			i32 idx = groupIconMap().Find(gMask);
			if(idx != -1)
			{
				Txt const &imlName = groupIconMap().operator[](idx);
				if(imlName == "")
					return Null;
				else
					return GetImlImage(imlName);
			}
			else
				return Null;
		}
		static Txt GetGroupDesc(dword gMask)
		{
			i32 idx = groupDescMap().Find(gMask);
			if(idx != -1)
				return GetLngTxt(groupDescMap().operator[](idx));
			else
				return "";
		}
		static Vec<dword>GetGroupIndexes(dword groupMask, dword forceMask = 0, dword forceVal = 0)
		{
			Index<dword> idx;
			Vec<dword> allIdx;
			allIdx <<= groupDescMap().GetKeys();
			for(i32 i = 0; i < allIdx.GetCount(); i++)
			{
				dword ci = allIdx.operator[](i);
				if( ((ci & forceMask) == (forceVal & forceMask)) && (ci & groupMask))
					idx.FindAdd((ci & groupMask) | (forceMask & forceVal));
			}
			Vec<dword>res = idx.PickKeys();
			Sort(res);
			return res;
		}
		static Vec<Txt>GetGroupClasses(dword gMask)
		{
			Vec<Txt>names;
			Vec<dword>indexes;
			VecMap<Txt, dword> const &iMap = indexMap();
			for(i32 i = 0; i < iMap.GetCount(); i++)
			{
				dword ci = iMap.operator[](i);
				if((gMask & ci) == gMask)
				{
					names.Add(iMap.GetKey(i));
					indexes.Add(ci);
				}
			}
			IndexSort(indexes, names);
			return names;
		}
		
		virtual ~WithFactory() {}
		
		// for debugging purposes, we allow to de-register all classes at once
		// so we can run multiple tests in a single app
		static void UnregisterAll(void)
		{
			classMap().Clear();
			typeMap().Clear();
			descMap().Clear();
			indexMap().Clear();
			imageMap().Clear();
		}
};

#ifdef COMPILER_MSC
#define REGISTERCLASS(type, ...) \
	INITBLOCK { \
		type::Register<type>(#type, __VA_ARGS__); \
}
#else
#define REGISTERCLASS(type, ...) \
	INITBLOCK { \
		type::Register<type>(#type, ##__VA_ARGS__); \
}
#endif

#define REGISTERCLASSGROUPICON(type, groupMask, desc, icon) \
	INITBLOCK { \
		type::RegisterClassGroupIcon(groupMask, desc, icon); \
	}
END_DRX_NAMESPACE

#endif
