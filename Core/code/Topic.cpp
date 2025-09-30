#include "../Core.h"
//#include <drx/Core/Compress.h>

namespace drx {

struct TopicData__ : Moveable<TopicData__> {
	Txt      title;
	u8k *data;
	i32         len;
};

VecMap<Txt, VecMap<Txt, VecMap<Txt, TopicData__ > > >& Topics__()
{
	static VecMap<Txt, VecMap<Txt, VecMap<Txt, TopicData__ > > > x;
	return x;
}

VecMap<Txt, VecMap<Txt, Vec<Txt> > >& TopicBase()
{
	static VecMap<Txt, VecMap<Txt, Vec<Txt> > > tb;
	return tb;
}

Txt TopicLinkTxt(const TopicLink& tl)
{
	Txt s;
	s << "topic://" << tl.package << '/' << tl.group << '/' << tl.topic;
	if(tl.label.GetCount())
		s << '#' << tl.label;
	return s;
}

TopicLink ParseTopicLink(tukk link)
{
	TopicLink tl;
	tukk end = link + strlen(link);
	tukk lbl = strchr(link, '#');
	if(lbl) {
		end = lbl;
		tl.label = lbl + 1;
	}
	if(memcmp(link, "topic://", 8) == 0)
		link += 8;
	do {
		if(!IsNull(tl.package))
			tl.package << '/';
		tl.package.Cat(tl.group);
		tl.group = tl.topic;
		tukk b = link;
		while(link < end && *link != '/')
			link++;
		tl.topic = Txt(b, link);
	}
	while(link++ < end);
	return tl;
}

static StaticMutex sTopicLock;

Topic GetTopic(const Txt& package, const Txt& group, const Txt& topic_)
{
	Mutex::Lock __(sTopicLock);
	VecMap<Txt, VecMap<Txt, TopicData__> > *p = Topics__().FindPtr(package);
	if(p) {
		VecMap<Txt, TopicData__> *g = p->FindPtr(group);
		if(g) {
			Txt topic = topic_;
			for(i32 pass = 0; pass < 2; pass++) {
				const TopicData__ *d = g->FindPtr(topic);
				if(d) {
					Topic t;
					t.title = d->title;
					t.text = ZDecompress(d->data, d->len);
					return t;
				}

				i32 q = topic.ReverseFind('$'); // we have change lang separator from $ to _, keep compatibility
				if(q >= 0)
					topic.Set(q, '_');
				else
				if((q = topic.ReverseFind('_')) >= 0)
					topic.Set(q, '$');
				else
					break;
			}
		}
	}
	return Topic();
}

Topic GetTopic(tukk path)
{
	TopicLink tl = ParseTopicLink(path);
	Topic t = GetTopic(tl.package, tl.group, tl.topic);
	t.label = tl.label;
	tl.label.Clear();
	t.link = TopicLinkTxt(tl);
	return t;
}

Topic GetTopicLNG(tukk path)
{
	return GetTopic(path +
	                ("$" + ToLower(LNGAsText(SetLNGCharset(GetCurrentLanguage(), CHARSET_DEFAULT)))));
}

}

void RegisterTopic__(tukk topicfile, tukk topic, tukk title,
                     u8k *data, i32 len)
{
    drx::Mutex::Lock __(drx::sTopicLock);
    ASSERT(*topicfile == '<');
    ASSERT(*DRX::GetFName(topicfile).Last() == '>');
    DRX::Txt q = DRX::GetFileFolder(topicfile + 1);
    DRX::Txt group = DRX::GetFileTitle(q);
    DRX::Txt package = DRX::UnixPath(DRX::GetFileFolder(q));
    DRX::TopicData__& d = DRX::Topics__().GetAdd(package).GetAdd(group).GetAdd(topic);
    d.title = title;
    d.data = data;
    d.len = len;
    DRX::TopicBase().GetAdd(package).GetAdd(group).Add(topic);
}