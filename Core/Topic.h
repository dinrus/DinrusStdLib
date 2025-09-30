#ifndef _Core_Topic_h_
#define _Core_Topic_h_

struct Topic : Moveable<Topic> {
    Txt title;
    Txt text;
    Txt link;
    Txt label;

    operator const Txt&() const { return text; }
    operator tukk () const  { return text; }
};

struct TopicLink {
    Txt package;
    Txt group;
    Txt topic;
    Txt label;

    operator bool() const { return !IsNull(topic); }
};

Txt     TopicLinkTxt(const TopicLink& tl);
TopicLink  ParseTopicLink(tukk link);

Topic      GetTopic(const Txt& package, const Txt& group, const Txt& topic);
Topic      GetTopic(tukk path);
Topic      GetTopicLNG(tukk path);

VecMap<Txt, VecMap<Txt, Vec<Txt> > >& TopicBase();

#endif
