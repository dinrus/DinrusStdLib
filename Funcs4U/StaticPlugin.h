// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifndef _Functions4U_StaticPlugin_h_
#define _Functions4U_StaticPlugin_h_


namespace drx {

#define PluginRegister(a, b, c)     a::Register<b>(c, typeid(a).name())
#define PluginInit(a, b)            (a)._Init(b, typeid(a).name())

class StaticPlugin {
private:
    uk data = 0;
    Txt name;
    Txt type;
    uk instance = 0;

    template <class T> static uk New()           {return new T;}
    template <class T> static void Delete(uk p)  {delete static_cast<T *>(p);}

protected:
    inline uk GetData() {return data;};

    struct PluginData {
        Txt name;
        Txt type;
        uk instance;
        uk (*New)();
        void (*Delete)(uk );
    };

    static Array<PluginData>& Plugins();

public:
    virtual ~StaticPlugin();

    void End();

    template <class T>
    static void Register(tukk name, tukk _type) {
        PluginData& x = Plugins().Add();
        x.type = _type;
        x.name = name;
        x.instance = 0;
        x.New = New<T>;
        x.Delete = Delete<T>;
    }
    bool _Init(tukk _name, tukk _type);

    Txt &GetType() {return name;};
    Txt &GetName() {return name;};
};

}

#endif
