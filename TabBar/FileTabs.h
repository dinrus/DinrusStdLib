class FileTabs : public TabBar
{
private:
    bool stackedicons:1;
    bool greyedicons:1;
    Color filecolor;
    Color extcolor;
protected:
    // Overload this to change grouping behaviour
    virtual Txt GetFileGroup(const Txt &file);
    // Overload this to change stacking behaviour
    virtual Txt GetStackId(const Tab &a);
    virtual hash_t GetStackSortOrder(const Tab &a);

    virtual void ComposeTab(Tab& tab, const Font &font, Color ink, i32 style);
    virtual void ComposeStackedTab(Tab& tab, const Tab& stacked_tab, const Font &font, Color ink, i32 style);
    virtual Size GetStackedSize(const Tab &t);

public:
    FileTabs();

    virtual void Serialize(Stream& s);

    void    AddFile(const WTxt &file, bool make_active = true);
    void    AddFile(const WTxt &file, Image img, bool make_active = true);
    void    InsertFile(i32 ix, const WTxt &file, bool make_active = true);
    void    InsertFile(i32 ix, const WTxt &file, Image img, bool make_active = true);

    void    AddFiles(const Vec<Txt> &files, bool make_active = true);
    void    AddFiles(const Vec<Txt> &files, const Vec<Image> &img, bool make_active = true);
    void    InsertFiles(i32 ix, const Vec<Txt> &files, bool make_active = true);
    void    InsertFiles(i32 ix, const Vec<Txt> &files, const Vec<Image> &img, bool make_active = true);

    void    RenameFile(const WTxt &from, const WTxt &to, Image icon = Null);

    FileTabs& FileColor(Color c)    { filecolor = c; Refresh(); return *this; }
    FileTabs& ExtColor(Color c)     { extcolor = c; Refresh(); return *this; }

    FileTabs& FileIcons(bool normal = true, bool stacked = true, bool stacked_greyedout = true);

    Vec<Txt>  GetFiles() const;
    FileTabs& operator<<(const FileTabs &src);
};
