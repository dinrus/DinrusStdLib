// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifndef _GatherTpp_h_
#define _GatherTpp_h_

namespace drx {

class GatherTpp {
public:
    void AddFolder(tukk folder) {rootFolders.Add(folder);};
    bool Load(Txt indexFile, Gate2<i32, i32> progress = false);

    Txt Www(tukk topic, i32 lang, Txt topicLocation = "topic://uppweb/www/");

    i32 FindTopic(const Txt name);
    Topic &GetTopic(i32 id);
    Topic &AddTopic(const Txt name);
    Txt GatherTopics(tukk topic, Txt& title);

    bool MakeHtml(tukk folder,  Gate2<i32, i32> progress = false);
    bool MakePdf(tukk filename, Gate2<i32, i32> progress = false);


private:
    drx::Array <Txt> rootFolders;
    Txt dir;
    VecMap<Txt, Txt> escape;
public:
    VecMap<Txt, Txt> labels;
    VecMap<Txt, Txt> links;
    VecMap<Txt, Txt> reflink;
    VecMap<Txt, Topic> tt;

private:
    Txt keywords;    //
    Txt title;       //
    Txt indexTopic;

    Txt TopicFName(tukk topic);
    Txt GatherTopics(tukk topic);
public:
    void GatherRefLinks(tukk upp);
private:
    void ExportPage(i32 i, Txt htmlFolder, Txt keywords = "");
    Txt QtfAsHtml(tukk qtf, Index<Txt>& css, const VecMap<Txt, Txt>& links,
        const VecMap<Txt, Txt>& labels, const Txt& outdir, const Txt& fn = Null);
};

Txt ChangeTopicLanguage(const Txt &topic, i32 lang);

}

#endif
